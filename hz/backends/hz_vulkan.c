#include "hz_vulkan.h"

#include <stdlib.h> // malloc, free

#define ARRAYSIZE(A) (sizeof(A)/sizeof((A)[0]))
#define OFFSETOF(thing, member) ((void *)&((thing*)0)->member)
#define HZ_STATIC_ASSERT(condition) __Static_assert(condition)

#define EXIT_ERROR(msg) { fprintf(stderr, "%s\n", msg); exit(-1); }


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

static hz_bool hz_string_list_empty(hz_string_list_t *list)
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

    long sl = strlen(p);
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
        long sl = strlen(*p);
        *wptr = malloc(sl+1);
        strcpy(*wptr, *p);
    }
}


static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
                                                     VkDebugUtilsMessageTypeFlagsEXT message_type,
                                                     const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
                                                     void* user_data)
{
    if (message_severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        fprintf_s(stderr, "VALIDATION: %s\n", callback_data->pMessage);
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
get_required_vk_extensions(hz_vulkan_renderer_t *renderer)
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

static int create_instance(hz_vulkan_renderer_t *renderer)
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

static int create_debug_messenger(hz_vulkan_renderer_t *renderer)
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo = {0};
    populate_debug_messenger_create_info(&createInfo);
    return createDebugUtilsMessengerEXT(renderer->instance, &createInfo, NULL, &renderer->debugMessenger) == VK_SUCCESS;
}

typedef struct {
    int hasGraphicsFamily;
    int hasPresentFamily;
    uint32_t graphicsFamily;
    uint32_t presentFamily;
} QueueFamilyIndices;

static QueueFamilyIndices find_queue_families(VkPhysicalDevice device, VkSurfaceKHR surface)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, NULL);

    VkQueueFamilyProperties queueFamilies[queueFamilyCount];
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies);

    for (int i = 0; i < queueFamilyCount; ++i) {
        VkQueueFamilyProperties queueFamily = queueFamilies[i];

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.hasGraphicsFamily = 1;
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = 0;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport) {
            indices.hasPresentFamily = 1;
            indices.presentFamily = i;
        }
    }

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
    VkExtensionProperties available_extensions[extension_count];
    vkEnumerateDeviceExtensionProperties(device, NULL, &extension_count, available_extensions);

    for (uint32_t i = 0; i < ARRAYSIZE(sDeviceExtensions); ++i)
    {
        const char *device_extension = sDeviceExtensions[i];
        if (!check_device_extension_name(device_extension, available_extensions, extension_count))
            return 0;
    }

    return 1;
}

static inline int
queue_families_complete(const QueueFamilyIndices *indices)
{
    return indices->hasGraphicsFamily && indices->hasPresentFamily;
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

//static void free_swapchain_support_details(SwapchainSupportDetails *details)
//{
//    if (details->formatCount) free(details->formats);
//    if (details->presentModeCount) free(details->presentModes);
//}

static VkSurfaceFormatKHR choose_swapchain_surface_format(VkSurfaceFormatKHR availableFormats[],
                                                          size_t formatCount)
{
    for (size_t i = 0; i < formatCount; ++i) {
        if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
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

static void create_swapchain(GLFWwindow *window, hz_vulkan_renderer_t *renderer)
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
        return 0;
    }

    return score;
}

typedef struct {
    VkPipeline pipeline;
    VkShaderModule modules[2];
} GraphicsPipeline;


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
        VkPhysicalDevice physicalDevices[physicalDeviceCount];
        vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, &physicalDevices[0]);

        CandidateDevice *entries = malloc(physicalDeviceCount * sizeof(CandidateDevice));

        for (int i = 0; i < physicalDeviceCount; ++i) {
            entries[i].device = physicalDevices[i];
            entries[i].score = compute_device_suitability(physicalDevices[i], surface);
        }

        qsort((void *)entries, physicalDeviceCount, sizeof(CandidateDevice), compare_devices);

        if (entries[0].score > 0)
            device = entries[0].device;

        free(entries);
    }

    return device;
}

static void
create_swapchain_image_views(hz_vulkan_renderer_t *renderer)
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

hz_vulkan_renderer_t *
hz_vulkan_renderer_create(GLFWwindow *window, int enableDebug)
{
    hz_vulkan_renderer_t *renderer = malloc(sizeof(*renderer));
    renderer->enableDebug = enableDebug;

    if (!create_instance(renderer)) {
        fprintf_s(stderr, "%s\n", "failed to create Instance.");
        hz_vulkan_renderer_release(renderer);
        return NULL;
    }

    fprintf_s(stdout, "%s\n", "created vulkan instance.");

    if (renderer->enableDebug) {
        if (!create_debug_messenger(renderer)) {
            fprintf_s(stderr, "%s\n", "failed to create debug messenger!");
            hz_vulkan_renderer_release(renderer);
            return NULL;
        }

        fprintf_s(stdout, "%s\n", "created debug messenger.");
    }

    // Create window surface
    if (glfwCreateWindowSurface(renderer->instance, window, NULL, &renderer->surface) != VK_SUCCESS) {
        fprintf_s(stderr, "%s\n", "failed to create vulkan surface!");
        hz_vulkan_renderer_release(renderer);
        return NULL;
    }

    fprintf_s(stdout, "%s\n", "created vulkan surface.");

    renderer->physicalDevice = choose_physical_device(renderer->instance, renderer->surface);
    if (renderer->physicalDevice == VK_NULL_HANDLE) {
        fprintf_s(stderr, "%s\n", "could not find any suitable vulkan device.");
    }

    // Create logical device
    QueueFamilyIndices indices = find_queue_families(renderer->physicalDevice, renderer->surface);

    if (!queue_families_complete(&indices)) {
        fprintf(stderr, "%s\n", "Queue family indices are not complete.");
    }

    // Create device queues
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};
    VkDeviceQueueCreateInfo queueCreateInfos[2] = {0};

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

    if (vkCreateDevice(renderer->physicalDevice, &createInfo, NULL, &renderer->device) != VK_SUCCESS) {
        EXIT_ERROR("Failed to create logical device!");
    }

    vkGetDeviceQueue(renderer->device, indices.graphicsFamily, 0, &renderer->graphicsQueue);
    vkGetDeviceQueue(renderer->device, indices.presentFamily, 0, &renderer->presentQueue);

    // Create swapchain
    create_swapchain(window, renderer);

    vkGetSwapchainImagesKHR(renderer->device, renderer->swapchain, &renderer->swapchainImageCount, NULL);
    renderer->swapchainImages = malloc(sizeof(VkImage) * renderer->swapchainImageCount);
    vkGetSwapchainImagesKHR(renderer->device, renderer->swapchain, &renderer->swapchainImageCount, renderer->swapchainImages);


    // Create swapchain image views
    create_swapchain_image_views(renderer);



    return renderer;
}

void
hz_vulkan_renderer_release(hz_vulkan_renderer_t *renderer)
{
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
