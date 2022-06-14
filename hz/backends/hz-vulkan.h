#ifndef HZ_VULKAN_H
#define HZ_VULKAN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <hz/hz.h>
#include <hz/hz-glyph-cache.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct hz_vk_context_t {
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
    VkImage                     glyphCacheImage;

    VkCommandPool               computeCmdPool;
    VkCommandBuffer             computeCmdBuffer;

    VkRenderPass                renderPass;
    VkPipelineLayout            pipelineLayout;
    VkPipeline                  computePipeline;

    hz_bool_t                   enableDebug, enableVSync;
} hz_vk_context_t;

hz_vk_context_t *
hz_vk_context_create(GLFWwindow *window, int enableDebug);

void hz_vk_render_frame(hz_vk_context_t *ctx);

void hz_vk_context_destroy(hz_vk_context_t *ctx);

#ifdef __cplusplus
};
#endif

#endif // HZ_VULKAN_H
