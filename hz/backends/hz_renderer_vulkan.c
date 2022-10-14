#include "hz_renderer_vulkan.h"

#include <stdlib.h> // malloc, free

#define ARRAYSIZE(A) (sizeof(A)/sizeof((A)[0]))
#define OFFSETOF(thing, member) ((void *)&((thing*)0)->member)
#define HZ_STATIC_ASSERT(condition) __Static_assert(condition)
#define VR_FAILED(vr) ((vr) != VK_SUCCESS)

#define EXIT_ERROR(msg) do { fprintf(stderr, "%s\n", msg); exit(-1); } while(0)

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

typedef struct {
    VkImage image;
    VkDeviceMemory deviceMemory;
    VkDeviceSize deviceSize;
    VkImageView imageView;
    VkSampler sampler;
    VkImageLayout imageLayout;
} hz_vk_glyph_cache_t;

typedef struct {
    VkImageView     sdf_image_view;
    VkImageView     stencil_image_view;
    VkImage         sdf_image;
    VkImage         stencil_image;
    VkFramebuffer   framebuffer;
    VkSampler       sampler;
    VkImageLayout   layout;
} glyph_cache_data_t;

struct hz_vk_renderer_t {
    GLFWwindow *                window;
    VkInstance                  instance;
    VkDebugUtilsMessengerEXT    debugMessenger;
    VkPhysicalDevice            physicalDevice;
    VkDevice                    device;
    VkSurfaceKHR                surface;
    VkSwapchainKHR              swapchain;
    VkQueue                     graphicsQueue, presentQueue, computeQueue;
    uint32_t                    swapchainImageCount;
    VkImage *                   swapchainImages;
    VkFormat                    swapchainImageFormat;
    VkExtent2D                  swapchainExtent;
    VkImageView *               swapchainImageViews;

    VkDescriptorPool            descriptorPool;
    VkDescriptorSet *           computeDescriptorSets;
    size_t                      computeDescriptorSetCount;

    VkDescriptorSetLayout       setLayout;

    VkSemaphore                 imageAvailableSemaphore;
    VkSemaphore                 renderFinishedSemaphore;
    VkFence                     inFlightFence;
    glyph_cache_data_t          glyph_cache_data;

    VkCommandPool               computeCmdPool;
    VkCommandBuffer             computeCmdBuffer;

    VkCommandPool               graphicsCmdPool;
    VkCommandBuffer             graphicsCmdBuffer;

    VkRenderPass                renderPass;

    VkPipelineLayout            pipelineLayout;
    VkPipeline                  computePipeline;

    VkPipelineLayout            graphicsPipelineLayout;

    VkPipeline                  bezier_sdf_shader;
    VkPipeline                  bezier_mask_shader;


    hz_vector(VkFramebuffer)    framebuffers;


    hz_bool_t                   enableDebug, enableVSync;
};

static const char *sValidationLayers[] = {
    "VK_LAYER_KHRONOS_validation",
};

typedef struct { char **data; uint32_t size; } hz_string_list_t;

static hz_string_list_t *hz_string_list_create()
{
    hz_string_list_t *list = malloc(sizeof(*list));
    list->data = NULL;
    list->size = 0;
    return list;
}

static void hz_string_list_destroy(hz_string_list_t *list)
{
    for (uint32_t i = 0; i < list->size; ++i) {
        free((void *)list->data[i]);
    }

    list->size = 0;
    free((void *)list->data);
    free(list);
}

static hz_bool_t hz_string_list_empty(hz_string_list_t *list)
{
    return !list->size || list->data == NULL;
}

static void hz_string_list_grow(hz_string_list_t *list, uint32_t extra)
{
    uint32_t newsz = list->size + extra;

    if (hz_string_list_empty(list)) {
        list->data = malloc(newsz * sizeof(char*));
    } else {
        list->data = realloc(list->data, newsz * sizeof(char*));
    }

    list->size = newsz;
}

static void hz_string_list_add(hz_string_list_t *list, const char *p)
{
    uint32_t dstidx = list->size;
    hz_string_list_grow(list, 1);

    size_t sl = strlen(p);
    list->data[dstidx] = malloc(sl+1);
    strcpy(list->data[dstidx], p);
}

static void hz_string_list_add_range(hz_string_list_t *list,
                                     const char **beg, const char **end)
{
    /* grow data array */
    ptrdiff_t extra = (ptrdiff_t)(end - beg);

    uint32_t dstidx = list->size;
    hz_string_list_grow(list, extra);

#if 0
    /* clear array extra */
    memset(list->data + list->sz, 0, extra * sizeof(char*));
#endif

    char **wptr = list->data + dstidx;
    for (const char **p = beg; p < end; ++p, ++wptr) {
        size_t sl = strlen(*p);
        *wptr = malloc(sl+1);
        strcpy(*wptr, *p);
    }
}

typedef struct {
    uint8_t *data;
    size_t size;
    int was_loaded;
} File;

static File *load_entire_file(const char *filename)
{
    File *buffer = malloc(sizeof(*buffer));
    buffer->was_loaded = 0;
    FILE *fp = fopen(filename, "rb");

    if (fp != NULL) {
        fseek(fp,  0, SEEK_END);
        buffer->size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        if (buffer->size != 0) {
            buffer->data = (unsigned char *)malloc(buffer->size);
            fread(buffer->data, 1, buffer->size, fp);
            buffer->was_loaded = 1;
        }
    }

    fclose(fp);
    return buffer;
}

static void destroy_file(File *buffer)
{
    if (buffer->size > 0 && buffer->data != NULL) {
        free(buffer->data);
    }

    free(buffer);
}


static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                     VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                     const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                     void* user_data)
{
    if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        fprintf(stderr, "VALIDATION: %s\n\n", callback_data->pMessage);
    }

    return VK_FALSE;
}

VkResult createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    }
}

static hz_string_list_t *
get_required_vk_extensions(hz_vk_renderer_t *renderer)
{
    hz_string_list_t *names = hz_string_list_create();

    uint32_t glfw_extension_count;
    const char** glfw_extensions;
    glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    hz_string_list_add_range(names, glfw_extensions, glfw_extensions + glfw_extension_count);

    if (renderer->enableDebug) {
        hz_string_list_add(names, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return names;
}

static void
populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT *create_info)
{
    create_info->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    create_info->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    create_info->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    create_info->pfnUserCallback = debug_callback;
    create_info->pUserData = NULL; // Optional
}

static int create_instance(hz_vk_renderer_t *renderer)
{
    hz_string_list_t *required_extensions = get_required_vk_extensions(renderer);

    VkApplicationInfo app_info = {0};

    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.apiVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Hamza";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pApplicationName = "Hamza";

    VkInstanceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.enabledExtensionCount = required_extensions->size;
    create_info.ppEnabledExtensionNames = (const char *const *) required_extensions->data;

    VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {0};

    if (renderer->enableDebug) {
        create_info.ppEnabledLayerNames = sValidationLayers;
        create_info.enabledLayerCount = ARRAYSIZE(sValidationLayers);

        // Setup specific debug messenger for vkCreateInstance and vkDestroyInstance
        populate_debug_messenger_create_info(&debug_create_info);
        create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debug_create_info;
    } else {
        create_info.ppEnabledLayerNames = NULL;
        create_info.enabledLayerCount = 0;
    }

    return vkCreateInstance(&create_info, NULL, &renderer->instance) == VK_SUCCESS;
}

static int create_debug_messenger(hz_vk_renderer_t *renderer)
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {0};
    populate_debug_messenger_create_info(&createInfo);
    return createDebugUtilsMessengerEXT(renderer->instance, &createInfo, NULL, &renderer->debugMessenger) == VK_SUCCESS;
}

typedef struct {
    hz_bool_t hasGraphicsFamily;
    hz_bool_t hasPresentFamily;
    hz_bool_t hasComputeFamily;
    uint32_t graphicsFamily;
    uint32_t presentFamily;
    uint32_t computeFamily;
} QueueFamilyIndices;

static QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilyIndices indices = {0};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties *queueFamilies = malloc(queueFamilyCount * sizeof (*queueFamilies));
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (int i = 0; i < queueFamilyCount; ++i) {
        VkQueueFamilyProperties queueFamily = queueFamilies[i];

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.hasGraphicsFamily = HZ_TRUE;
            indices.graphicsFamily = i;
        }

        if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
            indices.hasComputeFamily = HZ_TRUE;
            indices.computeFamily = i;
        }

        VkBool32 presentSupport = 0;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) {
            indices.hasPresentFamily = HZ_TRUE;
            indices.presentFamily = i;
        }
    }

    free(queueFamilies);
    return indices;
}

static int check_device_extension_name(const char *extension,
                                       VkExtensionProperties properties[],
                                       uint32_t extensionCount)
{
    for (uint32_t i = 0; i < extensionCount; ++i)
        if (!strcmp(properties[i].extensionName, extension)) return 1;

    return 0;
}

static const char *sDeviceExtensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static int check_device_extension_support(VkPhysicalDevice device)
{
    uint32_t extension_count;
    vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, NULL);
    VkExtensionProperties *available_extensions = malloc(extension_count * sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, available_extensions);

    for (uint32_t i = 0; i < ARRAYSIZE(sDeviceExtensions); ++i)
    {
        const char *device_extension = sDeviceExtensions[i];
        if (!check_device_extension_name(device_extension, available_extensions, extension_count))
            return 0;
    }

    free(available_extensions);
    return 1;
}

static inline int
queue_families_complete(const QueueFamilyIndices *indices)
{
    return indices->hasGraphicsFamily && indices->hasPresentFamily && indices->hasComputeFamily;
}

typedef struct {
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR formats[16];
    uint32_t formatCount;
    VkPresentModeKHR presentModes[16];
    uint32_t presentModeCount;
} SwapchainSupportDetails;

static SwapchainSupportDetails query_swapchain_support(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    SwapchainSupportDetails details = {0};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, NULL);

    if (details.formatCount > 0) {
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details.formatCount, details.formats);
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, NULL);

    if (details.presentModeCount > 0) {
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details.presentModeCount, details.presentModes);
    }

    return details;
}

static VkSurfaceFormatKHR choose_swapchain_surface_format(VkSurfaceFormatKHR availableFormats[],
                                                          size_t formatCount)
{
    for (size_t i = 0; i < formatCount; ++i) {
        // if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
        //     return availableFormats[i];
        // }
        if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM && availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormats[i];
        }
    }

    return availableFormats[0];
}

static VkPresentModeKHR choose_swapchain_present_mode(VkPresentModeKHR availablePresentModes[],
                                                      size_t presentModeCount)
{
    for (size_t i = 0; i < presentModeCount; ++i) {
        if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentModes[i];
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

static uint32_t clampu32(uint32_t x, uint32_t a, uint32_t b)
{
    return x < a ? a : (x > b ? b : x);
}

static VkExtent2D choose_swapchain_extent(GLFWwindow *window, const VkSurfaceCapabilitiesKHR *capabilities)
{
    if (capabilities->currentExtent.width != UINT32_MAX) {
        return capabilities->currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        VkExtent2D actualExtent = { (uint32_t)width, (uint32_t)height };

        actualExtent.width = clampu32(actualExtent.width, capabilities->minImageExtent.width, capabilities->maxImageExtent.width);
        actualExtent.height = clampu32(actualExtent.height, capabilities->minImageExtent.height, capabilities->maxImageExtent.height);

        return actualExtent;
    }
}

static void create_swapchain(GLFWwindow *window, hz_vk_renderer_t *renderer)
{
    SwapchainSupportDetails swapchainSupport = query_swapchain_support(renderer->physicalDevice, renderer->surface);

    VkSurfaceFormatKHR surfaceFormat = choose_swapchain_surface_format(swapchainSupport.formats, swapchainSupport.formatCount);
    VkPresentModeKHR presentMode = choose_swapchain_present_mode(swapchainSupport.presentModes, swapchainSupport.presentModeCount);
    VkExtent2D extent = choose_swapchain_extent(window, &swapchainSupport.capabilities);

    renderer->swapchainExtent = extent;
    renderer->swapchainImageFormat = surfaceFormat.format;

    uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;

    if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
        imageCount = swapchainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = renderer->surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = find_queue_families(renderer->physicalDevice, renderer->surface);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = NULL; // Optional
    }

    createInfo.preTransform = swapchainSupport.capabilities.currentTransform;

    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(renderer->device, &createInfo, NULL, &renderer->swapchain) != VK_SUCCESS) {
        EXIT_ERROR("failed to create swap chain!");
    }
}

static int is_device_suitable(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilyIndices indices = find_queue_families(device, surface);
    int extensionsSupported = check_device_extension_support(device);

    int swapchainAdequate = 0;
    if (extensionsSupported) {
        SwapchainSupportDetails swapchainSupport = query_swapchain_support(device, surface);
        swapchainAdequate = swapchainSupport.formatCount && swapchainSupport.presentModeCount;
    }

    return extensionsSupported && swapchainAdequate
        && queue_families_complete(&indices);
}

uint32_t find_memory_type(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    fprintf(stderr, "Failed to find suitable memory type.\n");
}

static int compute_device_suitability(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    int score = 0;

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    // discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    // maximum possible size of textures affects graphics quality
    score += (int) deviceProperties.limits.maxImageDimension2D;

    // if device is not suitable, it can't be used
    if (!is_device_suitable(device, surface)) {
        score = 0;
    }

    printf("deviceName: %s, score: %d\n", deviceProperties.deviceName, score);

    return score;
}

typedef struct { int score; VkPhysicalDevice device; } CandidateDevice;

static int compare_devices(const void *a, const void *b)
{
    return ((CandidateDevice*)a)->score - ((CandidateDevice*)b)->score;
}

static VkPhysicalDevice choose_physical_device(VkInstance instance, VkSurfaceKHR surface)
{
    // Query physical devices and choose the one most fit
    VkPhysicalDevice device = VK_NULL_HANDLE;
    uint32_t physicalDeviceCount = 0;

    vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, NULL);

    if (physicalDeviceCount > 0) {
        VkPhysicalDevice *physicalDevices = malloc(physicalDeviceCount * sizeof(VkPhysicalDevice));
        vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, &physicalDevices[0]);

        CandidateDevice *entries = malloc(physicalDeviceCount * sizeof(CandidateDevice));

        for (int i = 0; i < physicalDeviceCount; ++i) {
            entries[i].device = physicalDevices[i];
            entries[i].score = compute_device_suitability(physicalDevices[i], surface);
        }

        qsort((void *)entries, physicalDeviceCount, sizeof(CandidateDevice), compare_devices);

        if (entries[0].score > 0) {
            device = entries[0].device;
        }

        free(physicalDevices);
        free(entries);
    }

    return device;
}

static void
create_swapchain_image_views(hz_vk_renderer_t *renderer)
{
    renderer->swapchainImageViews = malloc(sizeof(VkImageView) * renderer->swapchainImageCount);

    for (size_t i = 0; i < renderer->swapchainImageCount; i++) {
        VkImageViewCreateInfo createInfo = {0};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = renderer->swapchainImages[i];

        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = renderer->swapchainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(renderer->device, &createInfo, NULL, &renderer->swapchainImageViews[i]) != VK_SUCCESS) {
            EXIT_ERROR("failed to create image views!");
        }
    }
}

static VkShaderModule create_shader_module(VkDevice device, const uint8_t *code, size_t codeSize)
{
    VkShaderModule shaderModule;
    VkShaderModuleCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = codeSize;
    createInfo.pCode = (uint32_t const*)code;

    if (vkCreateShaderModule(device, &createInfo, NULL, &shaderModule) != VK_SUCCESS) {
        fprintf(stderr, "%s\n", "Failed to create shader module.");
    }

    return shaderModule;
}

typedef struct {
    VkPipeline pipeline;
    VkShaderModule modules[2];
} GraphicsPipeline;

VkPipelineLayout create_graphics_pipeline_layout(VkDevice device, VkDescriptorSetLayout setLayout)
{
    VkPipelineLayout pipelineLayout;
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;//1;
    pipelineLayoutInfo.pSetLayouts = NULL;//&setLayout;
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = NULL; // Optional

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, NULL, &pipelineLayout) != VK_SUCCESS) {
        fprintf(stderr, "failed to create pipeline layout!\n");
    }

    return pipelineLayout;
}

static VkPipeline create_pipeline(hz_vk_renderer_t *renderer,
    size_t vertex_binding_count,
    VkVertexInputBindingDescription *vertex_bindings,
    size_t vertex_attrib_count,
    VkVertexInputAttributeDescription *vertex_attribs,
    const char *vertex_shader_path,
    const char *fragment_shader_path,
    VkPipelineColorBlendAttachmentState *colorBlendAttachment)
{
    VkPipeline pipeline = VK_NULL_HANDLE;
    File *vertexShaderCode = load_entire_file(vertex_shader_path);
    File *fragmentShaderCode = load_entire_file(fragment_shader_path);

    if (!vertexShaderCode->was_loaded) {
        fprintf(stderr, "%s\n", "Failed to load vertex shader.");
        return pipeline;
    }

    if (!fragmentShaderCode->was_loaded) {
        fprintf(stderr, "%s\n", "Failed to load fragment shader.");
        return pipeline;
    }

    VkShaderModule vertexShaderModule = create_shader_module(renderer->device, vertexShaderCode->data, vertexShaderCode->size);
    VkShaderModule fragmentShaderModule = create_shader_module(renderer->device, fragmentShaderCode->data, fragmentShaderCode->size);

    // fill in details for the two stages
    VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {0};
    vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageInfo.module = vertexShaderModule;
    vertexShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = {0};
    fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageInfo.module = fragmentShaderModule;
    fragmentShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertexShaderStageInfo, fragmentShaderStageInfo};

    // Vertex input
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = vertex_binding_count;
    vertexInputInfo.pVertexBindingDescriptions = vertex_bindings; // Optional
    vertexInputInfo.vertexAttributeDescriptionCount = vertex_attrib_count;
    vertexInputInfo.pVertexAttributeDescriptions = vertex_attribs; // Optional

    // Input assembly
    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {0};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    // Viewport and scissor
    VkViewport viewport = {0};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) renderer->swapchainExtent.width;
    viewport.height = (float) renderer->swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {0};
    scissor.offset = (VkOffset2D){0, 0};
    scissor.extent = renderer->swapchainExtent;

    // Viewport state
    VkPipelineViewportStateCreateInfo viewportState = {0};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    // Rasterizer
    VkPipelineRasterizationStateCreateInfo rasterizer = {0};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    // Multisampling
    VkPipelineMultisampleStateCreateInfo multisampling = {0};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = NULL; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional
/*
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_MAX; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
*/
    VkPipelineColorBlendStateCreateInfo colorBlending = {0};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkGraphicsPipelineCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.stageCount = 2;
    createInfo.pStages = shaderStages;
    createInfo.pVertexInputState = &vertexInputInfo;
    createInfo.pInputAssemblyState = &inputAssembly;
    createInfo.pViewportState = &viewportState;
    createInfo.pRasterizationState = &rasterizer;
    createInfo.pMultisampleState = &multisampling;
    createInfo.pDepthStencilState = NULL; // Optional
    createInfo.pColorBlendState = &colorBlending;
    createInfo.pDynamicState = NULL; // Optional
    createInfo.layout = renderer->graphicsPipelineLayout;

    createInfo.renderPass = renderer->renderPass;
    createInfo.subpass = 0;

    createInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
    createInfo.basePipelineIndex = -1; // Optional

    if (vkCreateGraphicsPipelines(renderer->device, VK_NULL_HANDLE, 1, &createInfo, NULL, &pipeline) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create graphics pipeline!\n");
    }

    destroy_file(vertexShaderCode);
    destroy_file(fragmentShaderCode);

    return pipeline;
}

static VkPipeline create_compute_pipeline(hz_vk_renderer_t *renderer,
                                          const char *filename)
{
    VkPipeline pipeline = VK_NULL_HANDLE;
    File *shader_bin = load_entire_file(filename);

    if (shader_bin->was_loaded) {
        VkShaderModule computeShaderModule = create_shader_module(renderer->device, shader_bin->data, shader_bin->size);

        // Create pipeline stage
        VkPipelineShaderStageCreateInfo computeShaderStageInfo = {0};
        computeShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        computeShaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        computeShaderStageInfo.module = computeShaderModule;
        computeShaderStageInfo.pName = "main";

        // Create pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {0};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &renderer->setLayout;
        pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
        pipelineLayoutInfo.pPushConstantRanges = NULL; // Optional

        if (vkCreatePipelineLayout(renderer->device, &pipelineLayoutInfo, NULL, &renderer->pipelineLayout) != VK_SUCCESS) {
            fprintf(stderr, "failed to create pipeline layout!\n");
        }

        // create pipeline
        VkComputePipelineCreateInfo createInfo = {0};
        createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        createInfo.pNext = NULL;
        createInfo.layout = renderer->pipelineLayout;
        createInfo.stage = computeShaderStageInfo;

        if (vkCreateComputePipelines(renderer->device, VK_NULL_HANDLE, 1, &createInfo, NULL, &pipeline) != VK_SUCCESS) {
            fprintf(stderr, "Failed to create compute pipeline!\n");
        }
    } else {
        fprintf(stderr, "Failed to load compute shader!\n");
    }

    destroy_file(shader_bin);
    return pipeline;
}

static void create_render_pass(hz_vk_renderer_t *renderer)
{
    VkAttachmentDescription colorAttachment = {0};
    colorAttachment.format = renderer->swapchainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;

    // Applies to color and depth buffers
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

    // Stencil operations are ignored for now
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {0};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo = {0};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    // create subpass dependency
    VkSubpassDependency dependency = {0};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(renderer->device, &renderPassInfo, NULL, &renderer->renderPass) != VK_SUCCESS) {
        fprintf(stderr, "%s\n", "failed to create render pass!");
    }
}

void create_descriptor_pool(hz_vk_renderer_t *renderer)
{
    VkDescriptorPoolSize pool_sizes[] = {
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 4 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 4 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4 }
    };

    VkDescriptorPoolCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.maxSets = 1000;
    createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    createInfo.pPoolSizes = pool_sizes;
    createInfo.poolSizeCount = ARRAYSIZE(pool_sizes);

    if (vkCreateDescriptorPool(renderer->device, &createInfo, NULL, &renderer->descriptorPool) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create descriptor pool!\n");
    }
}

typedef struct { int32_t x, y; } Vec2i;
typedef struct { uint32_t x, y; } Vec2u;
typedef struct { float x, y; } Vec2f;
typedef struct { float x, y, z; } Vec3f;

typedef uint8_t Fixed8, Fixed8Norm, Fixed8UNorm;
typedef uint16_t Fixed16;

typedef struct { Fixed8UNorm r,g,b,a; } Color8BPC;

typedef struct {
    int16_t infoIdx; // 2 bytes
    Fixed8 slant; // 1 byte [-1,1]
    Fixed8 weight; // 1 byte [-1,1]
    Color8BPC color; // 4 bytes
    Vec2f position; // 8 bytes
    Vec2f size; // 8 bytes
} GlyphVertexInstanced2D; // 24 bytes

typedef struct {
    Fixed16 u0, v0, u1, v1;
} GlyphCacheUVRect;

typedef struct {
    Vec2i size;
    int gridDivisions;
    float margin;
    float distanceScale;
} GlyphCacheInfo;

void update_compute_target_texture(VkImage image)
{
//    VkWriteDescriptor
}

typedef struct {
    VkQueue queue;
    VkPipeline pipeline;
    VkPipelineLayout pipeline_layout;
    VkImage glyph_cache_image;
} Compute;

static void allocate_compute_descriptor_sets(hz_vk_renderer_t *renderer)
{
    renderer->computeDescriptorSets = (VkDescriptorSet *) malloc(sizeof(VkDescriptorSet));

    VkDescriptorSetAllocateInfo allocateInfo = {0};
    allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocateInfo.pNext = NULL;
    allocateInfo.descriptorPool = renderer->descriptorPool;
    allocateInfo.descriptorSetCount = 1;
    allocateInfo.pSetLayouts = &renderer->setLayout;

    if (VR_FAILED(vkAllocateDescriptorSets(renderer->device, &allocateInfo, renderer->computeDescriptorSets))) {
        fprintf(stderr, "Failed to allocate descriptor sets!\n");
    }
}

static void init_compute(Compute *compute)
{

}

static VkSampler createGlyphCacheSampler(VkDevice device)
{
    VkSampler sampler = VK_NULL_HANDLE;
    VkSamplerCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    createInfo.anisotropyEnable = VK_FALSE;
    createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;

    if (vkCreateSampler(device, &createInfo, NULL, &sampler) != VK_SUCCESS) {
        fprintf(stderr, "Error: %s\n", "Failed to create glyph cache sampler");
    }

    return sampler;
}

typedef struct {
    VkImage image;
    VkImageView imageView;
    VkSampler sampler;
    VkImageLayout imageLayout;
} Texture;

void create_descriptor_set_layout(hz_vk_renderer_t *renderer)
{
    // information about the binding.
    VkDescriptorSetLayoutBinding layoutBindings[2] = {0};
    layoutBindings[0].binding = 0;
    layoutBindings[0].descriptorCount = 1;
    layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    layoutBindings[0].pImmutableSamplers = NULL;
    layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    layoutBindings[1].binding = 1;
    layoutBindings[1].descriptorCount = 1;
    layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBindings[1].pImmutableSamplers = NULL;
    layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutCreateInfo setLayoutInfo = {0};
    setLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    setLayoutInfo.bindingCount = 2;
    setLayoutInfo.pBindings = layoutBindings;
    setLayoutInfo.flags = 0;
    setLayoutInfo.pNext = NULL;

    if (vkCreateDescriptorSetLayout(renderer->device, &setLayoutInfo, NULL, &renderer->setLayout) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create descriptor set layout!\n");
    }
}

int create_image(VkDevice device, VkFormat format, VkImage *image, uint32_t width, uint32_t height)
{
    VkResult vr;
    VkImageCreateInfo createInfo = {0};

    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    createInfo.flags = 0;
    createInfo.imageType = VK_IMAGE_TYPE_2D;
    createInfo.extent.width = width;
    createInfo.extent.height = height;
    createInfo.extent.depth = 1;
    createInfo.mipLevels = 1;
    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    createInfo.arrayLayers = 1;
    createInfo.format = VK_FORMAT_R8_UNORM;
    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    createInfo.initialLayout = VK_IMAGE_LAYOUT_GENERAL;
    createInfo.usage = VK_IMAGE_USAGE_STORAGE_BIT;

    if ((vr = vkCreateImage(device, &createInfo, NULL, &imageData->image)) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create target image! (%d)\n", vr);
    }
}

void create_glyph_cache_data(VkDevice device,
    glyph_cache_data_t *glyph_cache_data, hz_glyph_cache_opts_t *opts)
{
    
}

// VkImageView createGlyphCacheImageView(VkDevice device, VkImage image)
// {
//     VkImageViewCreateInfo createInfo = {0};
//     createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//     createInfo.pNext = NULL;
//     createInfo.image = image;
//     createInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
//     createInfo.flags = 0;
//     createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

//     createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//     createInfo.subresourceRange.baseArrayLayer = 0;
//     createInfo.subresourceRange.baseMipLevel = 0;
//     createInfo.subresourceRange.layerCount = 1;
//     createInfo.subresourceRange.levelCount = 1;

//     createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
//     createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
//     createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
//     createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

//     VkImageView imageView = VK_NULL_HANDLE;
//     VkResult vr;
//     if ((vr = vkCreateImageView(device, &createInfo, NULL, &imageView)) != VK_SUCCESS) {
//         fprintf(stderr, "Failed to create image view for glyph cache! (%d)\n", vr);
//     }

//     return imageView;
// }

VkCommandBuffer create_command_buffer(VkDevice device, VkCommandPool commandPool)
{
    VkCommandBufferAllocateInfo allocateInfo = {0};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = NULL;
    allocateInfo.commandPool = commandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuffer = VK_NULL_HANDLE;
    VkResult result;
    if ((result = vkAllocateCommandBuffers(device, &allocateInfo, &cmdBuffer)) != VK_SUCCESS) {
        fprintf(stderr, "Failed to allocate command buffers! (%d)\n", result);
    }

    return cmdBuffer;
}

VkBuffer create_buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties, VkDeviceMemory *bufferMemory)
{
    VkResult vr;
    VkBufferCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.flags = 0;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = NULL;

    VkBuffer buffer;

    if (VR_FAILED(vr = vkCreateBuffer(device, &createInfo, NULL, &buffer))) {
        fprintf(stderr, "failed to create buffer! %d\n", vr);
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {0};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = find_memory_type(physicalDevice, memRequirements.memoryTypeBits, properties);

    if (VR_FAILED(vr = vkAllocateMemory(device, &allocInfo, NULL, bufferMemory))) {
        fprintf(stderr, "failed to allocate buffer memory! %d\n", vr);
    }

    vkBindBufferMemory(device, buffer, *bufferMemory, 0);

    return buffer;
}

VkFence create_fence(VkDevice device, VkBool32 isSignaled)
{
    VkFence fence;
    VkFenceCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (isSignaled) createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    vkCreateFence(device, &createInfo, NULL, &fence);
    return fence;
}

VkSemaphore create_semaphore(VkDevice device)
{
    VkSemaphore semaphore = VK_NULL_HANDLE;
    VkSemaphoreCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    if (vkCreateSemaphore(device, &createInfo, NULL, &semaphore) != VK_SUCCESS) {
        fprintf(stderr, "%s\n", "Failed to create semaphore!");
    }
    return semaphore;
}

void create_framebuffers(hz_vk_renderer_t *renderer)
{
    hz_vector_resize(renderer->framebuffers, renderer->swapchainImageCount);

    for (size_t i = 0; i < renderer->swapchainImageCount; ++i) {
        VkImageView attachments[] = { renderer->swapchainImageViews[i] };

        VkFramebufferCreateInfo framebufferInfo = {0};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderer->renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = renderer->swapchainExtent.width;
        framebufferInfo.height = renderer->swapchainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(renderer->device, &framebufferInfo, NULL, &renderer->framebuffers[i]) != VK_SUCCESS) {
            fprintf(stderr, "Failed to create framebuffers!\n");
        }
    }
}

VkCommandPool create_command_pool(VkDevice device, uint32_t queueFamilyIndex)
{
    VkCommandPool commandPool = VK_NULL_HANDLE;
    VkCommandPoolCreateInfo createInfo = {0};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = queueFamilyIndex;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkResult vr;
    if ((vr = vkCreateCommandPool(device, &createInfo, NULL, &commandPool)) != VK_SUCCESS) {
        fprintf(stderr, "Failed to create command pool (%d)\n", vr);
    }

    return commandPool;
}

hz_vk_renderer_t *
hz_vk_create_renderer(GLFWwindow *window, int enableDebug)
{
    hz_vk_renderer_t *renderer = malloc(sizeof(*renderer));
    renderer->framebuffers = NULL;

    renderer->enableDebug = enableDebug;

    if (!create_instance(renderer)) {
        fprintf(stderr, "%s\n", "failed to create Instance.");
        hz_vk_terminate(renderer);
        return NULL;
    }

    fprintf(stdout, "%s\n", "created vulkan instance.");

    if (renderer->enableDebug) {
        if (!create_debug_messenger(renderer)) {
            fprintf(stderr, "%s\n", "failed to create debug messenger!");
            hz_vk_terminate(renderer);
            return NULL;
        }

        fprintf(stdout, "%s\n", "created debug messenger.");
    }

    // Create window surface
    if (glfwCreateWindowSurface(renderer->instance, window, NULL, &renderer->surface) != VK_SUCCESS) {
        fprintf(stderr, "%s\n", "failed to create vulkan surface!");
        hz_vk_terminate(renderer);
        return NULL;
    }

    fprintf(stdout, "%s\n", "created vulkan surface.");

    renderer->physicalDevice = choose_physical_device(renderer->instance, renderer->surface);
    if (renderer->physicalDevice == VK_NULL_HANDLE) {
        fprintf(stderr, "%s\n", "could not find any suitable vulkan device.");
    }

    // Create logical device
    QueueFamilyIndices indices = find_queue_families(renderer->physicalDevice, renderer->surface);

    if (!queue_families_complete(&indices)) {
        fprintf(stderr, "%s\n", "Queue family indices are not complete.");
    }

    // Create device queues
    uint32_t queueFamilyIndices[3] = {indices.graphicsFamily, indices.presentFamily, indices.computeFamily};

//    printf("graphics queue family: %d\npresent queue family: %d\ncompute queue family: %d\n",
//           indices.graphicsFamily, indices.presentFamily, indices.computeFamily);

    VkDeviceQueueCreateInfo queueCreateInfos[3] = {0};

    float queuePriority = 1.0f;
    for (int i = 0; i < ARRAYSIZE(queueCreateInfos); ++i) {
        queueCreateInfos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfos[i].queueFamilyIndex = queueFamilyIndices[i];
        queueCreateInfos[i].queueCount = 1;
        queueCreateInfos[i].pQueuePriorities = &queuePriority;
    }

    VkPhysicalDeviceFeatures deviceFeatures = {0};
    VkDeviceCreateInfo createInfo = {0};

    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = queueCreateInfos;
    createInfo.queueCreateInfoCount = ARRAYSIZE(queueCreateInfos);
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = ARRAYSIZE(sDeviceExtensions);
    createInfo.ppEnabledExtensionNames = sDeviceExtensions;

    if (renderer->enableDebug) {
        createInfo.enabledLayerCount = ARRAYSIZE(sValidationLayers);
        createInfo.ppEnabledLayerNames = sValidationLayers;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkResult result;
    if ((result = vkCreateDevice(renderer->physicalDevice, &createInfo, NULL, &renderer->device)) != VK_SUCCESS) {
        printf("Create device error: %d\n", result);
        EXIT_ERROR("Failed to create logical device!");
    }

    vkGetDeviceQueue(renderer->device, indices.graphicsFamily, 0, &renderer->graphicsQueue);
    vkGetDeviceQueue(renderer->device, indices.presentFamily, 0, &renderer->presentQueue);
    vkGetDeviceQueue(renderer->device, indices.computeFamily, 0, &renderer->computeQueue);

    // Create swapchain
    create_swapchain(window, renderer);

    vkGetSwapchainImagesKHR(renderer->device, renderer->swapchain, &renderer->swapchainImageCount, NULL);
    renderer->swapchainImages = malloc(sizeof(VkImage) * renderer->swapchainImageCount);
    vkGetSwapchainImagesKHR(renderer->device, renderer->swapchain, &renderer->swapchainImageCount, renderer->swapchainImages);

    printf("Created swapchain with %d images\n", renderer->swapchainImageCount);

    // Create swapchain image views
    create_swapchain_image_views(renderer);

    // Create render pass
    create_render_pass(renderer);

    // Setup descriptor pool, set and layout (must be done before creating the pipelines)
    create_descriptor_pool(renderer);
    create_descriptor_set_layout(renderer);
    allocate_compute_descriptor_sets(renderer);

    // Create compute pipeline
    //renderer->computePipeline = create_compute_pipeline(g, "./shaders/bezier-to-sdf.comp.spv");
    // Create compute command pool
    //renderer->computeCmdPool = create_command_pool(renderer->device, indices.computeFamily);
    // Create compute command buffer
    //renderer->computeCmdBuffer = create_command_buffer(renderer->device, renderer->computeCmdPool);

    // Create graphics pipeline layout
    renderer->graphicsPipelineLayout = create_graphics_pipeline_layout(renderer->device, VK_NULL_HANDLE);

    {
        // Create single channel fast SDF generator shader
        VkVertexInputBindingDescription vertex_bindings[] = {
            { 0, sizeof(hz_bezier_vertex_t), VK_VERTEX_INPUT_RATE_INSTANCE } // Buffer
        };
        
        VkVertexInputAttributeDescription vertex_attribs[] = {
            { 0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(hz_bezier_vertex_t, v1) }, // v1
            { 1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(hz_bezier_vertex_t, v2) }, // v2
            { 2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(hz_bezier_vertex_t, c1) }, // c1
            { 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(hz_bezier_vertex_t, c2) }, // c1
            { 4, 0, VK_FORMAT_R32_SINT, offsetof(hz_bezier_vertex_t, type) } // type
        };

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_MAX; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        renderer->bezier_sdf_shader = create_pipeline(renderer,1,vertex_bindings,5,vertex_attribs,
            "./shaders/bezier_to_sdf.vert.spv", "./shaders/bezier_to_sdf.frag.spv",
            &colorBlendAttachment);
    }

    {
        // Create Kokojima et al. mask shader
        VkVertexInputBindingDescription vertex_bindings[] = {
            { 0, sizeof(hz_stencil_vertex_t), VK_VERTEX_INPUT_RATE_VERTEX } // Buffer
        };
 
        VkVertexInputAttributeDescription vertex_attribs[] = {
            { 0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(hz_stencil_vertex_t, pos) }, // pos
            { 1, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(hz_stencil_vertex_t, uv) }, // uv
        };

        VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_MAX; // Optional
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

        renderer->bezier_mask_shader = create_pipeline(renderer,1,vertex_bindings,2,vertex_attribs,
            "./shaders/bezier_mask.vert.spv", "./shaders/bezier_mask.frag.spv",
            &colorBlendAttachment);
    }



    // Create graphics pipeline
    //renderer->graphicsPipeline = create_graphics_pipeline(renderer, "./shaders/render-glyph-sdf.vert.spv", "./shaders/render-glyph-sdf.frag.spv");

    {
        GlyphCacheImageData cacheTexture;
        createGlyphCacheImage(renderer->device, &renderer->glyphCacheImageData, 1024, 1024);
        renderer->glyphCacheImageData.imageView = createGlyphCacheImageView(renderer->device, renderer->glyphCacheImageData.image);
        renderer->glyphCacheImageData.sampler = createGlyphCacheSampler(renderer->device);
    }

    {
        renderer->glyphCacheImageData.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        update_glyph_texture_descriptor(renderer->device, renderer->computeDescriptorSets[0], renderer->glyphCacheImageData);
    }

    renderer->inFlightFence = create_fence(renderer->device, VK_TRUE);
    renderer->graphicsCmdPool = create_command_pool(renderer->device, indices.graphicsFamily);
    renderer->graphicsCmdBuffer = create_command_buffer(renderer->device, renderer->graphicsCmdPool);

    create_framebuffers(renderer);

    renderer->imageAvailableSemaphore = create_semaphore(renderer->device);
    renderer->renderFinishedSemaphore = create_semaphore(renderer->device);

    return renderer;
}

/*
VkBuffer update_glyph_data_descriptor(VkDevice device, VkDescriptorSet descriptorSet, hz_bezier_shape_t *shape)
{
    VkDeviceMemory deviceMemory = VK_NULL_HANDLE;    
    VkDeviceSize bufferSize = sizeof(int) + shape->vertexCount * sizeof(hz_vertex_t);

    VkBuffer glyphDataBuffer = create_buffer(device,
        bufferSize,
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
        &deviceMemory);

    uint8_t *mappedData;
    vkMapMemory(device, deviceMemory, 0, VK_WHOLE_SIZE, 0, &mappedData);
    *(int *)mappedData = shape->vertexCount;
    memcpy(mappedData+4, shape->vertices, shape->vertexCount * sizeof(hz_vertex_t));
    vkUnmapMemory(device, deviceMemory);

    VkWriteDescriptorSet writeDescriptorSet;

    // glyph data descriptor write
    VkDescriptorBufferInfo bufferInfo = {0};
    bufferInfo.buffer = glyphDataBuffer;
    bufferInfo.offset = 0;
    bufferInfo.range = VK_WHOLE_SIZE;

    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.pNext = NULL;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.dstBinding = 1;
    writeDescriptorSet.dstSet = descriptorSet;
    writeDescriptorSet.pBufferInfo = &bufferInfo;
    writeDescriptorSet.pTexelBufferView = NULL;
    writeDescriptorSet.pImageInfo = NULL;

    vkUpdateDescriptorSets(device,1,&writeDescriptorSet,0,NULL);

    // destroy frame resources
    VkFreeMemory(device,deviceMemory,NULL);
    VkDestroyBuffer(device,glyphDataBuffer,NULL);
}
*/

#define WORKGROUP_SIZE_X 16
#define WORKGROUP_SIZE_Y 16

void record_compute_commands(hz_vk_renderer_t *renderer,
                             int textureWidth, int textureHeight)
{

    // Begin the command buffer
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = NULL;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = NULL;

    GlyphCacheInfo glyphCacheInfo;
    glyphCacheInfo.size = (Vec2i){textureWidth, textureHeight};
    glyphCacheInfo.distanceScale = 1.0f;
    glyphCacheInfo.gridDivisions = 64;
    glyphCacheInfo.margin = 0.05f;

    vkBeginCommandBuffer(renderer->computeCmdBuffer, &beginInfo);
    vkCmdBindPipeline(renderer->computeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, renderer->computePipeline);
    vkCmdBindDescriptorSets(renderer->computeCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, renderer->pipelineLayout, 0,
                            1, renderer->computeDescriptorSets, 0, NULL);

    uint32_t groupCountX = ((textureWidth) / WORKGROUP_SIZE_X) + 1;
    uint32_t groupCountY = ((textureHeight) / WORKGROUP_SIZE_Y) + 1;

    vkCmdDispatch(renderer->computeCmdBuffer, groupCountX, groupCountY, 1);
    vkEndCommandBuffer(renderer->computeCmdBuffer);
}

void record_graphics_command_buffer(hz_vk_renderer_t *renderer,
    VkCommandBuffer commandBuffer,
    uint32_t imageIndex,
    VkBuffer curve_buffer,
    uint32_t curve_count,
    VkBuffer mask_vbo,
    uint32_t mask_drawcount)
{
    VkCommandBufferBeginInfo beginInfo = {0};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pInheritanceInfo = NULL;
    beginInfo.flags = 0;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    // draw commands
    VkRenderPassBeginInfo renderPassInfo = {0};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderer->renderPass;
    renderPassInfo.framebuffer = renderer->framebuffers[imageIndex];

    // render area
    renderPassInfo.renderArea.offset = (VkOffset2D){0, 0};
    renderPassInfo.renderArea.extent = renderer->swapchainExtent;

    // clear color
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->bezier_sdf_shader);
    
    {
        // bind vertex buffer for curve segments
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &curve_buffer, offsets);
    }

    // draw bezier curve distances with min blending
    vkCmdDraw(commandBuffer, 6, curve_count, 0, 0);

    // create stencil mask for the bezier spline shape kokojima
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->bezier_mask_shader);

    // enable stencil test
    //vkCmdSetStencilTestEnable(commandBuffer, VK_TRUE);
    

    {
        // bind stencil triangles vertex buffers
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &mask_vbo, offsets);
    }

    // render the stencil triangles
    vkCmdDraw(commandBuffer, mask_drawcount, 1, 0, 0);


    //vkCmdSetStencilTestEnable(commandBuffer, VK_FALSE);
    // draw full-screen triangle with blending set to 1-dstCol


    vkCmdEndRenderPass(commandBuffer);
    vkEndCommandBuffer(commandBuffer);
}

void
create_buffer_and_mmap(hz_vk_renderer_t *renderer, VkBuffer *buffer, VkDeviceMemory *memory, void *data, VkDeviceSize size, VkBufferUsageFlags usage,
    VkMemoryPropertyFlags properties)
{
    *buffer = create_buffer(renderer->device, renderer->physicalDevice, size, usage, properties, memory);
    uint8_t *hostptr;
    vkMapMemory(renderer->device, *memory, 0, VK_WHOLE_SIZE, 0, (void **)&hostptr);
    memcpy(hostptr, data, size);
    vkUnmapMemory(renderer->device, *memory);
}

void
hz_vk_render_frame(hz_vk_renderer_t *renderer, stbtt_fontinfo *fontinfo, hz_index_t glyph_index)
{
    vkWaitForFences(renderer->device, 1, &renderer->inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(renderer->device, 1, &renderer->inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(renderer->device, renderer->swapchain, UINT64_MAX,
                          renderer->imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

#if 0
    // setup uniform buffer object for glyph curve data
    hz_bezier_shape_t *glyph_shape = hz_stbtt_get_glyph_shape(fontinfo, glyph_index);

    {
        VkDeviceSize bufferSize = 4 + glyph_shape->vertex_count * sizeof(hz_bezier_vertex_t);

        glyphDataBuffer = create_buffer(renderer->device, renderer->physicalDevice,
            bufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            &glyphDataDeviceMemory);

        uint8_t *mappedData;
        vkMapMemory(renderer->device, glyphDataDeviceMemory, 0, VK_WHOLE_SIZE, 0, (void **)&mappedData);
        *(int *)mappedData = glyph_shape->vertex_count;
        memcpy(mappedData+4, glyph_shape->vertices, glyph_shape->vertex_count * sizeof(hz_vertex_t));
        vkUnmapMemory(renderer->device, glyphDataDeviceMemory);

        VkWriteDescriptorSet writeDescriptorSet;

        // glyph data descriptor write
        VkDescriptorBufferInfo bufferInfo = {0};
        bufferInfo.buffer = glyphDataBuffer;
        bufferInfo.offset = 0;
        bufferInfo.range = VK_WHOLE_SIZE;

        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.pNext = NULL;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.dstBinding = 1;
        writeDescriptorSet.dstSet = renderer->computeDescriptorSets[0];
        writeDescriptorSet.pBufferInfo = &bufferInfo;
        writeDescriptorSet.pTexelBufferView = NULL;
        writeDescriptorSet.pImageInfo = NULL;

        vkUpdateDescriptorSets(renderer->device,1,&writeDescriptorSet,0,NULL);
    }
    hz_glyph_shape_destroy(glyph_shape);
#endif


#if 0
    
    vkResetCommandBuffer(renderer->computeCmdBuffer, 0);
    record_compute_commands(impl, 1024, 1024);

    uint32_t Semaphores;

    {
        VkSubmitInfo submitInfo = {0};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = NULL;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &renderer->computeCmdBuffer;
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = NULL;
        submitInfo.signalSemaphoreCount = 0;
        submitInfo.pSignalSemaphores = NULL;
        submitInfo.pWaitDstStageMask = NULL;

        vkQueueSubmit(renderer->computeQueue, 1, &submitInfo, renderer->inFlightFence);

        vkWaitForFences(renderer->device, 1, &renderer->inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(renderer->device, 1, &renderer->inFlightFence);


        // destroy frame resources
        vkFreeMemory(renderer->device,glyphDataDeviceMemory,NULL);
        vkDestroyBuffer(renderer->device,glyphDataBuffer,NULL);
    }
#endif

    {
        hz_bezier_shape_t *shape = hz_stbtt_get_glyph_shape(fontinfo, glyph_index);

        VkDeviceMemory vertex_buffer_memory = VK_NULL_HANDLE;
        VkDeviceSize vertex_buffer_size = shape->vertex_count * sizeof(hz_bezier_vertex_t);
        VkBuffer vertex_buffer = VK_NULL_HANDLE;
        create_buffer_and_mmap(impl, &vertex_buffer, &vertex_buffer_memory, shape->vertices, vertex_buffer_size,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        // create vertex buffer for the stencil mask triangle fans (kokojima method)
        // both line-edged and curve-edged triangle fans have to be computed.
        hz_vector(hz_stencil_vertex_t) mask_verts = NULL;
        for (int i = 0; i < 1/*shape->contour_count*/; ++i) {
            hz_contour_t *contour = &shape->contours[i];
            for (int j = 0; j < contour->curve_count; ++j) {
                hz_stencil_vertex_t v1,v2,v3;
                v1.pos = contour->pos;
                v1.uv = hz_vec2(0.0f, 1.0f);
                v2.pos = shape->vertices[contour->start+j].v2;
                v2.uv = hz_vec2(0.0f, 1.0f);
                v3.pos = shape->vertices[contour->start+j].v1;
                v3.uv = hz_vec2(0.0f, 1.0f);
                hz_vector_push_back(mask_verts, v1);
                hz_vector_push_back(mask_verts, v2);
                hz_vector_push_back(mask_verts, v3);
            }
        }

        uint32_t mask_drawcount = hz_vector_size(mask_verts);
        VkDeviceMemory mask_vbo_memory = VK_NULL_HANDLE;
        VkDeviceSize mask_vbo_size = mask_drawcount * sizeof(hz_stencil_vertex_t);
        VkBuffer mask_vbo = VK_NULL_HANDLE;
        create_buffer_and_mmap(impl, &mask_vbo, &mask_vbo_memory, mask_verts, mask_vbo_size,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

        printf("mask vertices: %d\n", mask_drawcount);

        vkResetCommandBuffer(renderer->graphicsCmdBuffer, 0);
        record_graphics_command_buffer(impl, renderer->graphicsCmdBuffer, imageIndex,
            vertex_buffer, shape->vertex_count,
            mask_vbo, mask_drawcount);

        VkSubmitInfo submitInfo = {0};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        VkSemaphore waitSemaphores[] = {renderer->imageAvailableSemaphore};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = HZ_ARRLEN(waitSemaphores);
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        VkCommandBuffer commandBuffers[] = {renderer->graphicsCmdBuffer};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = commandBuffers;
        VkSemaphore signalSemaphores[] = {renderer->renderFinishedSemaphore};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkQueueSubmit(renderer->graphicsQueue, 1, &submitInfo, renderer->inFlightFence);
        vkWaitForFences(renderer->device, 1, &renderer->inFlightFence, VK_TRUE, UINT64_MAX);

        VkPresentInfoKHR presentInfo = {0};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        VkSwapchainKHR swapChains[] = {renderer->swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains; 
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = NULL; // Optional

        vkQueuePresentKHR(renderer->presentQueue, &presentInfo);
        vkWaitForFences(renderer->device, 1, &renderer->inFlightFence, VK_TRUE, UINT64_MAX);
        
        // destroy frame resources
        vkFreeMemory(renderer->device, vertex_buffer_memory, NULL);
        vkDestroyBuffer(renderer->device, vertex_buffer, NULL);
        vkFreeMemory(renderer->device, mask_vbo_memory, NULL);
        vkDestroyBuffer(renderer->device, mask_vbo, NULL);
        hz_vector_destroy(mask_verts);
    }
}

void
hz_vk_terminate(hz_vk_renderer_t *renderer)
{
    vkDestroyPipelineLayout(renderer->device, renderer->pipelineLayout, NULL);

    for (uint32_t i = 0; i < renderer->swapchainImageCount; ++i) {
        vkDestroyImageView(renderer->device, renderer->swapchainImageViews[i], NULL);
    }

    free(renderer->swapchainImageViews);

    vkDestroySwapchainKHR(renderer->device, renderer->swapchain, NULL);

    if (renderer->surface != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(renderer->instance, renderer->surface, NULL);
    }

    if (renderer->debugMessenger != VK_NULL_HANDLE) {
        destroyDebugUtilsMessengerEXT(renderer->instance, renderer->debugMessenger, NULL);
    }

    vkDestroyInstance(renderer->instance, NULL);
    free(renderer);
}

void hz_vk_wait_idle(hz_vk_renderer_t *renderer)
{
    vkDeviceWaitIdle(renderer->device);
}