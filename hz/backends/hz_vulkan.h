#ifndef HZ_VULKAN_H
#define HZ_VULKAN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <hz/hz.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hz_vulkan_renderer_t {
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
    VkDescriptorSet             computeDescriptorSet;
    VkDescriptorSetLayout       descriptorSetLayout;
    VkImage                     glyphCacheImage;


    VkRenderPass                renderPass;
    VkPipelineLayout            pipelineLayout;

    hz_bool_t                   enableDebug, enableVSync;
} hz_vulkan_renderer_t;

hz_vulkan_renderer_t *
hz_vulkan_renderer_create(GLFWwindow *window, int enableDebug);

void hz_vulkan_render_frame(hz_vulkan_renderer_t *renderer);

void
hz_vulkan_renderer_release(hz_vulkan_renderer_t *renderer);

#ifdef __cplusplus
};
#endif

#endif // HZ_VULKAN_H
