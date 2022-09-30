#ifndef HZ_VULKAN_H
#define HZ_VULKAN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <hz/hz.h>
#include <hz/hz-glyph-cache.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hz_impl_vulkan_t hz_impl_vulkan_t;

hz_impl_vulkan_t *hz_vk_create_impl(GLFWwindow *window, int enableDebug);

void hz_vk_render_frame(hz_impl_vulkan_t *impl, stbtt_fontinfo *fontinfo, hz_index_t glyph_index);

void hz_vk_terminate(hz_impl_vulkan_t *impl);

void hz_vk_wait_idle(hz_impl_vulkan_t *impl);

#ifdef __cplusplus
};
#endif

#endif // HZ_VULKAN_H
