#ifndef OVERTURE_VULKAN_UTILS
#define  OVERTURE_VULKAN_UTILS

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

#define MAX_FRAMES_IN_FLIGHT 2

typedef struct {
    VkSurfaceKHR surface;
    VkPhysicalDevice physical_device;
    VkDevice device;
    VkQueue graphics_queue;
    VkQueue present_queue;
    VkSwapchainKHR swap_chain;
    VkFormat image_format;
    VkExtent2D extent;
    uint32_t image_count;
    uint32_t current_image;
    VkImage* images;
    VkImageView* image_views;
    VkFramebuffer* framebuffers;
    VkRenderPass render_pass;
    VkCommandPool command_pool;
    VkCommandBuffer* command_buffers;
    VkSemaphore* image_semaphores;
    VkSemaphore* render_semaphores;
    VkFence* fences;
    uint32_t current_frame;
} vulkan_window_info_t;

typedef struct {
    VkVertexInputBindingDescription binding_description;
    VkVertexInputAttributeDescription* attribute_description;
    uint32_t attribute_count;
} vertex_binding_t;

typedef struct {
    VkBuffer buffer;
    VkDeviceMemory memory;
} buffer_t;

typedef struct {
    VkPipeline pipeline;
    VkPipelineLayout layout;
    // descriptor set layouts and count
} pipeline_t;

VkResult create_debug_utils_messenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);

void destroy_debug_utils_messenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

VkBool32 check_vulkan_layer_support(const char** layers, uint32_t n);
VkBool32 check_vulkan_extention_support(VkPhysicalDevice device, const char** extensions, uint32_t n);

typedef struct {
    uint32_t* data;
    uint32_t len;
} shader_buffer_t;

shader_buffer_t load_shader(const char* path);

#endif
