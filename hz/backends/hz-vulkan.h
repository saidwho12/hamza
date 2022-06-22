#ifndef HZ_VULKAN_H
#define HZ_VULKAN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <hz/hz.h>
#include <hz/hz-glyph-cache.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct HzImplVulkan HzImplVulkan;

HzImplVulkan *hz_vk_create_impl(GLFWwindow *window, int enableDebug);

void hz_impl_vulkan_render_frame(HzImplVulkan *impl);

void hz_impl_vulkan_terminate(HzImplVulkan *impl);

void hz_vk_wait_idle(HzImplVulkan *impl);

#ifdef __cplusplus
};
#endif

#endif // HZ_VULKAN_H
