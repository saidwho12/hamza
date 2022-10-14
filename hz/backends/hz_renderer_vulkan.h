#ifndef HZ_RENDERER_VULKAN_H
#define HZ_RENDERER_VULKAN_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <hz/hz.h>
#include <hz/hz_glyph_cache.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct hz_vk_renderer_t hz_vk_renderer_t;

hz_vk_renderer_t *hz_vk_create_renderer(GLFWwindow *window, int enableDebug);

void hz_vk_render_frame(hz_vk_renderer_t *renderer, stbtt_fontinfo *fontinfo, hz_index_t glyph_index);

void hz_vk_terminate(hz_vk_renderer_t *renderer);

void hz_vk_wait_idle(hz_vk_renderer_t *renderer);

#ifdef __cplusplus
};
#endif

#endif // HZ_RENDERER_VULKAN_H
