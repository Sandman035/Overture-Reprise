#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>

#include "graphics/vulkan.h"
#include "graphics/vulkan_utils.h"
#include "core/log.h"

// TODO: proper error handling, outputing an error message and then letting the program segfault on its own is not the way to go

const char* validation_layers[] = {
    "VK_LAYER_KHRONOS_validation"
};

const char* device_exts[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static VkInstance instance;
static VkDebugUtilsMessengerEXT debug_messenger;

const VkBool32 enable_validation_layers = VK_TRUE;

static VKAPI_ATTR VkBool32 VKAPI_CALL vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: TRACE(pCallbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: INFO(pCallbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: WARN(pCallbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: ERROR(pCallbackData->pMessage); break;
        default: break;
    }
    return VK_FALSE;
}

void setup_vulkan() {
    VkApplicationInfo app_info;
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = PROJECT_NAME;
    app_info.applicationVersion = VK_MAKE_VERSION(VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    app_info.pEngineName = ENGINE_NAME;
    app_info.engineVersion = VK_MAKE_VERSION(ENGINE_VERSION_MAJOR, ENGINE_VERSION_MINOR, ENGINE_VERSION_PATCH);
    app_info.apiVersion = VK_API_VERSION_1_0;
    app_info.pNext = NULL;

    VkInstanceCreateInfo create_info;
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;
    create_info.flags = 0;

    uint32_t validation_layers_count = sizeof(validation_layers)/sizeof(validation_layers[0]);
    VkBool32 validation_layer_support = check_vulkan_layer_support(validation_layers, validation_layers_count);

    uint32_t ext_count = 0;
    const char** exts;
    exts = glfwGetRequiredInstanceExtensions(&ext_count);

    const char** exts_with_debug_utils = NULL;
    VkDebugUtilsMessengerCreateInfoEXT debug_create_info;
    if (enable_validation_layers == VK_TRUE && validation_layer_support == VK_TRUE) {
        exts_with_debug_utils = malloc(sizeof(char*) * (ext_count + 1));
        memcpy(exts_with_debug_utils, exts, sizeof(char*) * ext_count);
        exts_with_debug_utils[ext_count] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

        create_info.enabledExtensionCount = ext_count + 1;
        create_info.ppEnabledExtensionNames = exts_with_debug_utils;

        create_info.enabledLayerCount = validation_layers_count;
        create_info.ppEnabledLayerNames = validation_layers;

        debug_create_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_create_info.pfnUserCallback = vulkan_debug_callback;
        debug_create_info.pUserData = NULL;
        debug_create_info.pNext = NULL;
        debug_create_info.flags = 0;

        create_info.pNext = &debug_create_info;
    } else {
        create_info.enabledExtensionCount = ext_count;
        create_info.ppEnabledExtensionNames = exts;

        create_info.enabledLayerCount = 0;
        create_info.pNext = NULL;
    }

    if (vkCreateInstance(&create_info, NULL, &instance) != VK_SUCCESS) {
        FATAL("Failed to create vulkan instance.");
    }
    TRACE("Created vulkan instance.");

    free(exts_with_debug_utils);

    /* setup debug messenger */

    if (enable_validation_layers == VK_TRUE && validation_layer_support) {
        if (create_debug_utils_messenger(instance, &debug_create_info, NULL, &debug_messenger) != VK_SUCCESS) {
            ERROR("Failed to create debug messenger.");
        } else {
            TRACE("Created vulkan debug messenger.");
        }
    }
}

void setup_vulkan_window(window_t* window) {
    if (glfwCreateWindowSurface(instance, window->window, NULL, &window->vulkan_info.surface)) {
        ERROR("Failed to create surface");
    }

    TRACE("Created window surface.");

    /* pick physical device*/

    window->vulkan_info.physical_device = VK_NULL_HANDLE;

    uint32_t device_count = 0;
    vkEnumeratePhysicalDevices(instance, &device_count, NULL);

    if (device_count == 0) {
        ERROR("Cannot find device with vulkan support.");
    }

    VkPhysicalDevice* devices = malloc(device_count * sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(instance, &device_count, devices);

    uint32_t device_ext_count = sizeof(device_exts)/sizeof(device_exts[0]);
    uint32_t format_count = 0;
    uint32_t present_mode_count = 0;

    uint32_t queue_family_indices[2];

    for (uint32_t i = 0; i < device_count; i++) {
        VkPhysicalDeviceProperties device_props;
        VkPhysicalDeviceFeatures device_feats;
        vkGetPhysicalDeviceProperties(devices[i], &device_props);
        vkGetPhysicalDeviceFeatures(devices[i], &device_feats);

        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queue_family_count, NULL);

        VkQueueFamilyProperties* queue_families = malloc(queue_family_count * sizeof(VkQueueFamilyProperties));
        vkGetPhysicalDeviceQueueFamilyProperties(devices[i], &queue_family_count, queue_families);

        for (uint32_t j = 0; j < queue_family_count; j++) {
            uint32_t queue_num = 0;
            if (queue_families[j].queueFlags & VK_QUEUE_GRAPHICS_BIT && queue_families[j].queueFlags & VK_QUEUE_COMPUTE_BIT) {
                queue_family_indices[0] = j;
                queue_num++;
            }

            VkBool32 present_support = VK_FALSE;
            vkGetPhysicalDeviceSurfaceSupportKHR(devices[i], j, window->vulkan_info.surface, &present_support);
            if (present_support == VK_TRUE) {
                queue_family_indices[1] = j;
                queue_num++;
            }

            if (queue_num == 2 && check_vulkan_extention_support(devices[i], device_exts, device_ext_count)) {
                vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], window->vulkan_info.surface, &format_count, NULL);
                vkGetPhysicalDeviceSurfacePresentModesKHR(devices[i], window->vulkan_info.surface, &present_mode_count, NULL);

                if (format_count != 0 || present_mode_count != 0) {
                    window->vulkan_info.physical_device = devices[i];
                    break;
                }
            }
        }

        free(queue_families);

        if (device_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            if (window->vulkan_info.physical_device != VK_NULL_HANDLE) {
                TRACE("Found suitable discrete GPU.");
            }
            break;
        } }

    free(devices);

    if (window->vulkan_info.physical_device == VK_NULL_HANDLE) {
        ERROR("Cannot find suitable physical device.");
    }

    TRACE("Selected physical device.");

    // Create logical device
    
    uint8_t queue_num = 2;
    if (queue_family_indices[0] == queue_family_indices[1]) {
        queue_num = 1;
    }

    VkDeviceQueueCreateInfo* queue_create_infos = malloc(queue_num * sizeof(VkDeviceQueueCreateInfo));

    float queue_priority = 1.0f;
    for (uint8_t i = 0; i < queue_num; i++) {
        queue_create_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queue_create_infos[i].queueFamilyIndex = queue_family_indices[i];
        queue_create_infos[i].queueCount = 1;
        queue_create_infos[i].flags = 0;
        queue_create_infos[i].pNext = NULL;
        queue_create_infos[i].pQueuePriorities = &queue_priority;
    }

    VkPhysicalDeviceFeatures device_feats;
    device_feats.samplerAnisotropy = VK_TRUE;
    // TODO: select more device features if needed

    VkDeviceCreateInfo device_create_info;
    device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_create_info.flags = 0;
    device_create_info.pNext = NULL;

    device_create_info.pQueueCreateInfos = queue_create_infos;
    device_create_info.queueCreateInfoCount = queue_num;

    device_create_info.pEnabledFeatures = &device_feats;

    device_create_info.ppEnabledExtensionNames = device_exts;
    device_create_info.enabledExtensionCount = device_ext_count;

    uint32_t validation_layers_count = sizeof(validation_layers)/sizeof(validation_layers[0]);
    VkBool32 validation_layer_support = check_vulkan_layer_support(validation_layers, validation_layers_count);

    if (validation_layer_support == VK_TRUE && enable_validation_layers == VK_TRUE) {
        device_create_info.enabledLayerCount = validation_layers_count;
        device_create_info.ppEnabledLayerNames = validation_layers;
    } else {
        device_create_info.enabledLayerCount = 0;
    }

    if (vkCreateDevice(window->vulkan_info.physical_device, &device_create_info, NULL, &window->vulkan_info.device) != VK_SUCCESS) {
        ERROR("Failed to create logical device.");
    }

    free(queue_create_infos);

    vkGetDeviceQueue(window->vulkan_info.device, queue_family_indices[0], 0, &window->vulkan_info.graphics_queue); // error handling???
    vkGetDeviceQueue(window->vulkan_info.device, queue_family_indices[1], 0, &window->vulkan_info.present_queue);

    TRACE("Created logical device.");

    /* Pick swap chain surface format */

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(window->vulkan_info.physical_device, window->vulkan_info.surface, &capabilities);

    VkSurfaceFormatKHR* formats = malloc(format_count * sizeof(VkSurfaceFormatKHR));
    VkPresentModeKHR* present_modes = malloc(present_mode_count * sizeof(VkPresentModeKHR));

    vkGetPhysicalDeviceSurfaceFormatsKHR(window->vulkan_info.physical_device, window->vulkan_info.surface, &format_count, formats);
    vkGetPhysicalDeviceSurfacePresentModesKHR(window->vulkan_info.physical_device, window->vulkan_info.surface, &present_mode_count, present_modes);

    VkSurfaceFormatKHR format = formats[0];
    for (uint32_t i = 0; i < format_count; i++) {
        if (formats[i].format == VK_FORMAT_B8G8R8A8_SRGB && formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            format = formats[i];
        }
    }

    free(formats);

    /* pick mode from input settings */
    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (uint32_t i = 0; i < present_mode_count; i++) {
        if (present_modes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            present_mode = present_modes[i];
        }
    }

    free(present_modes);

    VkExtent2D extent;
    if (capabilities.currentExtent.width != UINT32_MAX) {
        extent = capabilities.currentExtent;
    } else {
        int32_t width, height;
        glfwGetFramebufferSize(window->window, &width, &height);

        if (width > capabilities.maxImageExtent.width) {
            extent.width = capabilities.maxImageExtent.width;
        } else if (width < capabilities.minImageExtent.width) {
            extent.width = capabilities.minImageExtent.width;
        } else {
            extent.width = width;
        }

        if (height > capabilities.maxImageExtent.height) {
            extent.height = capabilities.maxImageExtent.height;
        } else if (height < capabilities.minImageExtent.height) {
            extent.height = capabilities.minImageExtent.height;
        } else {
            extent.height = height;
        }
    }

    uint32_t image_count = capabilities.minImageCount + 1;

    if (capabilities.maxImageCount > 0 && image_count > capabilities.maxImageCount) {
        image_count = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swap_chain_create_info;
    swap_chain_create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swap_chain_create_info.surface = window->vulkan_info.surface;
    swap_chain_create_info.minImageCount = image_count;
    swap_chain_create_info.imageFormat = format.format;
    swap_chain_create_info.imageColorSpace = format.colorSpace;
    swap_chain_create_info.imageExtent = extent;
    swap_chain_create_info.imageArrayLayers = 1;
    swap_chain_create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    if (queue_family_indices[0] != queue_family_indices[1]) {
        swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swap_chain_create_info.queueFamilyIndexCount = 2;
        swap_chain_create_info.pQueueFamilyIndices = queue_family_indices;
    } else {
        swap_chain_create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swap_chain_create_info.queueFamilyIndexCount = 0;
        swap_chain_create_info.pQueueFamilyIndices = NULL;
    }

    swap_chain_create_info.preTransform = capabilities.currentTransform;
    swap_chain_create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // maybe change this... who knows
    swap_chain_create_info.presentMode = present_mode;
    swap_chain_create_info.clipped = VK_TRUE;
    swap_chain_create_info.oldSwapchain = NULL;
    swap_chain_create_info.flags = 0;
    swap_chain_create_info.pNext = NULL;

    if (vkCreateSwapchainKHR(window->vulkan_info.device, &swap_chain_create_info, NULL, &window->vulkan_info.swap_chain)) {
        ERROR("Failed to create swap chain.");
    }

    TRACE("Created swap chain.");

    vkGetSwapchainImagesKHR(window->vulkan_info.device, window->vulkan_info.swap_chain, &image_count, NULL);
    window->vulkan_info.images = malloc(image_count * sizeof(VkImage));
    vkGetSwapchainImagesKHR(window->vulkan_info.device, window->vulkan_info.swap_chain, &image_count, window->vulkan_info.images);

    TRACE("Aquired swap chain images.");

    window->vulkan_info.image_format = format.format;
    window->vulkan_info.extent = extent;
    window->vulkan_info.image_count = image_count;

    window->vulkan_info.image_views = malloc(image_count * sizeof(VkImageView));

    for (uint32_t i = 0; i < image_count; i++) {
        VkImageViewCreateInfo image_view_create_info;
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_create_info.image = window->vulkan_info.images[i];
        image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_create_info.format = format.format;
        image_view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        image_view_create_info.subresourceRange.baseMipLevel = 0;
        image_view_create_info.subresourceRange.levelCount = 1;
        image_view_create_info.subresourceRange.baseArrayLayer = 0;
        image_view_create_info.subresourceRange.layerCount = 1;
        image_view_create_info.flags = 0;
        image_view_create_info.pNext = NULL;

        if (vkCreateImageView(window->vulkan_info.device, &image_view_create_info, NULL, &window->vulkan_info.image_views[i])) {
            ERROR("Failed to create image view.");
        }
    }

    TRACE("Created swap chain image views.");

    /* Create Render Pass */

    // maybe change some of these values
    VkAttachmentDescription color_attachment;
    color_attachment.format = format.format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    color_attachment.flags = 0;

    VkAttachmentReference color_attachment_ref;
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // this is will be used for mulitple "render passes" for defered and post processing etc
    // TODO: expand this
    VkSubpassDescription subpass;
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = NULL;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = NULL;
    subpass.pResolveAttachments = NULL;
    subpass.pDepthStencilAttachment = NULL;
    subpass.flags = 0;

    VkRenderPassCreateInfo render_pass_info;
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 0;
    render_pass_info.pDependencies = NULL;
    render_pass_info.flags = 0;
    render_pass_info.pNext = NULL;

    if (vkCreateRenderPass(window->vulkan_info.device, &render_pass_info, NULL, &window->vulkan_info.render_pass) != VK_SUCCESS) {
        ERROR("Failed to create render pass.");
    }

    TRACE("Created render pass.");

    /* Create frame buffers */

    window->vulkan_info.framebuffers = malloc(image_count * sizeof(VkFramebuffer));

    for (size_t i = 0; i < image_count; i++) {
        VkImageView attachments[] = {
            window->vulkan_info.image_views[i]
        };

        VkFramebufferCreateInfo framebuffer_info;
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = window->vulkan_info.render_pass;
        framebuffer_info.attachmentCount = 1;
        framebuffer_info.pAttachments = attachments;
        framebuffer_info.width = extent.width;
        framebuffer_info.height = extent.height;
        framebuffer_info.layers = 1;
        framebuffer_info.flags = 0;
        framebuffer_info.pNext = NULL;

        if (vkCreateFramebuffer(window->vulkan_info.device, &framebuffer_info, NULL, &window->vulkan_info.framebuffers[i]) != VK_SUCCESS) {
            ERROR("Failed to create framebuffer.");
        }
    }

    TRACE("Created framebuffers.");

    /* create command pool  and buffer */

    VkCommandPoolCreateInfo pool_info;
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.queueFamilyIndex = queue_family_indices[0];
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    pool_info.pNext = NULL;

    // would like to have one pool for the app but since it relies on device (queue_family_indices) it might not happen
    if (vkCreateCommandPool(window->vulkan_info.device, &pool_info, NULL, &window->vulkan_info.command_pool) != VK_SUCCESS) {
        ERROR("Failed to create command pool.");
    }

    window->vulkan_info.command_buffers = malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkCommandBuffer));

    VkCommandBufferAllocateInfo alloc_info;
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = window->vulkan_info.command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = MAX_FRAMES_IN_FLIGHT;
    
    if (vkAllocateCommandBuffers(window->vulkan_info.device, &alloc_info, window->vulkan_info.command_buffers) != VK_SUCCESS) {
        ERROR("Failed to create command buffer.");
    }

    TRACE("Created command pool and buffer.");

    VkSemaphoreCreateInfo semaphore_info;
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_info.flags = 0;
    semaphore_info.pNext = NULL;

    VkFenceCreateInfo fence_info;
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    fence_info.pNext = NULL;

    window->vulkan_info.image_semaphores = malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkSemaphore));
    window->vulkan_info.render_semaphores = malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkSemaphore));
    window->vulkan_info.fences = malloc(MAX_FRAMES_IN_FLIGHT * sizeof(VkFence));

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (
            vkCreateSemaphore(window->vulkan_info.device, &semaphore_info, NULL, &window->vulkan_info.image_semaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(window->vulkan_info.device, &semaphore_info, NULL, &window->vulkan_info.render_semaphores[i]) != VK_SUCCESS ||
            vkCreateFence(window->vulkan_info.device, &fence_info, NULL, &window->vulkan_info.fences[i]) != VK_SUCCESS
        ) {
            ERROR("Failed to create seamaphores.");
        }
    }

    window->vulkan_info.current_frame = 0;
}

void begin_vulkan_window_render(window_t* window) {
    TRACE("Waiting for fence.");
    vkWaitForFences(window->vulkan_info.device, 1, &window->vulkan_info.fences[window->vulkan_info.current_frame], VK_TRUE, UINT64_MAX);
    vkAcquireNextImageKHR(window->vulkan_info.device, window->vulkan_info.swap_chain, UINT64_MAX, window->vulkan_info.image_semaphores[window->vulkan_info.current_frame], VK_NULL_HANDLE, &window->vulkan_info.current_image);

    vkResetFences(window->vulkan_info.device, 1, &window->vulkan_info.fences[window->vulkan_info.current_frame]);

    vkResetCommandBuffer(window->vulkan_info.command_buffers[window->vulkan_info.current_frame], /*VkCommandBufferResetFlagBits*/ 0);

    VkCommandBufferBeginInfo begin_info;
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = 0;
    begin_info.pInheritanceInfo = NULL;
    begin_info.pNext = NULL;
    
    if (vkBeginCommandBuffer(window->vulkan_info.command_buffers[window->vulkan_info.current_frame], &begin_info) != VK_SUCCESS) {
        ERROR("Failed to begin command buffer recording.");
    }

    TRACE("Started recording to command buffer.");

    VkRenderPassBeginInfo render_pass_info;
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = window->vulkan_info.render_pass;
    render_pass_info.framebuffer = window->vulkan_info.framebuffers[window->vulkan_info.current_image];
    
    VkOffset2D offset;
    offset.x = 0;
    offset.y = 0;
    render_pass_info.renderArea.offset = offset;
    render_pass_info.renderArea.extent = window->vulkan_info.extent;
    
    VkClearValue clear_value;
    VkClearColorValue clear_color = {0.0, 0.0, 0.0, 1.0};
    clear_value.color = clear_color;
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_value;
    render_pass_info.pNext = NULL;

    vkCmdBeginRenderPass(window->vulkan_info.command_buffers[window->vulkan_info.current_frame], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    TRACE("Started render pass.");

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) window->vulkan_info.extent.width;
    viewport.height = (float) window->vulkan_info.extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(window->vulkan_info.command_buffers[window->vulkan_info.current_frame], 0, 1, &viewport);

    VkRect2D scissor;
    scissor.offset = offset;
    scissor.extent = window->vulkan_info.extent;
    vkCmdSetScissor(window->vulkan_info.command_buffers[window->vulkan_info.current_frame], 0, 1, &scissor);
}

void end_vulkan_window_render(window_t* window) {
    vkCmdEndRenderPass(window->vulkan_info.command_buffers[window->vulkan_info.current_frame]);

    if (vkEndCommandBuffer(window->vulkan_info.command_buffers[window->vulkan_info.current_frame]) != VK_SUCCESS) {
        ERROR("Failed to record command buffer.");
    }

    TRACE("Stopped render pass");
}

void vulkan_display_to_window(window_t* window) {
    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {window->vulkan_info.image_semaphores[window->vulkan_info.current_frame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &window->vulkan_info.command_buffers[window->vulkan_info.current_frame];

    VkSemaphore signalSemaphores[] = {window->vulkan_info.render_semaphores[window->vulkan_info.current_frame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    submitInfo.pNext = NULL;

    if (vkQueueSubmit(window->vulkan_info.graphics_queue, 1, &submitInfo, window->vulkan_info.fences[window->vulkan_info.current_frame]) != VK_SUCCESS) {
        ERROR("Failed to submit draw command buffer.");
    }

    VkPresentInfoKHR presentInfo;
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {window->vulkan_info.swap_chain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &window->vulkan_info.current_image;
    presentInfo.pResults = NULL;
    presentInfo.pNext = NULL;

    vkQueuePresentKHR(window->vulkan_info.present_queue, &presentInfo);

    window->vulkan_info.current_frame = (window->vulkan_info.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void cleanup_vulkan_window(window_t* window) {
    vkDeviceWaitIdle(window->vulkan_info.device);

    for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(window->vulkan_info.device, window->vulkan_info.image_semaphores[i], NULL);
        vkDestroySemaphore(window->vulkan_info.device, window->vulkan_info.render_semaphores[i], NULL);
        vkDestroyFence(window->vulkan_info.device, window->vulkan_info.fences[i], NULL);
    }

    free(window->vulkan_info.image_semaphores);
    free(window->vulkan_info.render_semaphores);
    free(window->vulkan_info.fences);

    free(window->vulkan_info.command_buffers);

    vkDestroyCommandPool(window->vulkan_info.device, window->vulkan_info.command_pool, NULL);

    vkDestroyRenderPass(window->vulkan_info.device, window->vulkan_info.render_pass, NULL);

    destroy_swapchain(window);

    vkDestroySurfaceKHR(instance, window->vulkan_info.surface, NULL);

    vkDestroyDevice(window->vulkan_info.device, NULL);
}

void recreate_swapchain(window_t *window) {
    vkDeviceWaitIdle(window->vulkan_info.device);

    destroy_swapchain(window);

    /* TODO: recreate swapchain */
}

void destroy_swapchain(window_t *window) {
    for (uint32_t i = 0; i < window->vulkan_info.image_count; i++) {
        vkDestroyImageView(window->vulkan_info.device, window->vulkan_info.image_views[i], NULL);
        vkDestroyFramebuffer(window->vulkan_info.device, window->vulkan_info.framebuffers[i], NULL);
    }

    free(window->vulkan_info.images);
    free(window->vulkan_info.image_views);
    free(window->vulkan_info.framebuffers);

    vkDestroySwapchainKHR(window->vulkan_info.device, window->vulkan_info.swap_chain, NULL);
}

void cleanup_vulkan() {
    uint32_t validation_layers_count = sizeof(validation_layers)/sizeof(validation_layers[0]);
    VkBool32 validation_layer_support = check_vulkan_layer_support(validation_layers, validation_layers_count);

    if (enable_validation_layers == VK_TRUE && validation_layer_support == VK_TRUE) {
        destroy_debug_utils_messenger(instance, debug_messenger, NULL);
    }

    vkDestroyInstance(instance, NULL);
}

void create_pipeline(window_t* window, pipeline_t* pipeline,  const char* vertex_shader, const char* fragment_shader, vertex_binding_t vertex_binding) {
    VkShaderModule vert_mod;
    VkShaderModule frag_mod;

    shader_buffer_t vert_buff = load_shader(vertex_shader);

    VkShaderModuleCreateInfo vert_mod_info;
    vert_mod_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    vert_mod_info.codeSize = vert_buff.len;
    vert_mod_info.pCode = vert_buff.data;
    vert_mod_info.flags = 0;
    vert_mod_info.pNext = NULL;

    if (vkCreateShaderModule(window->vulkan_info.device, &vert_mod_info, NULL, &vert_mod) != VK_SUCCESS) {
        ERROR("Failed to create vertex shader module!");
    }

    free(vert_buff.data);

    shader_buffer_t frag_buff = load_shader(fragment_shader);

    VkShaderModuleCreateInfo frag_mod_info;
    frag_mod_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    frag_mod_info.codeSize = frag_buff.len;
    frag_mod_info.pCode = frag_buff.data;
    frag_mod_info.flags = 0;
    frag_mod_info.pNext = NULL;

    if (vkCreateShaderModule(window->vulkan_info.device, &frag_mod_info, NULL, &frag_mod) != VK_SUCCESS) {
        ERROR("Failed to create fragment shader module!");
    }

    free(frag_buff.data);

    TRACE("Created shader modules.");

    VkPipelineShaderStageCreateInfo vert_stage_info;
    vert_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vert_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vert_stage_info.module = vert_mod;
    vert_stage_info.pName = "main";
    vert_stage_info.pSpecializationInfo = 0;
    vert_stage_info.flags = 0;
    vert_stage_info.pNext = NULL;

    VkPipelineShaderStageCreateInfo frag_stage_info;
    frag_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    frag_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    frag_stage_info.module = frag_mod;
    frag_stage_info.pName = "main";
    frag_stage_info.pSpecializationInfo = NULL;
    frag_stage_info.flags = 0;
    frag_stage_info.pNext = NULL;

    VkPipelineShaderStageCreateInfo shader_stages[] = {vert_stage_info, frag_stage_info};

    VkPipelineVertexInputStateCreateInfo vertex_input_info;
    vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount = 1;
    vertex_input_info.vertexAttributeDescriptionCount = vertex_binding.attribute_count;
    vertex_input_info.pVertexBindingDescriptions = &vertex_binding.binding_description;
    vertex_input_info.pVertexAttributeDescriptions = vertex_binding.attribute_description;
    vertex_input_info.flags = 0;
    vertex_input_info.pNext = NULL;

    VkPipelineInputAssemblyStateCreateInfo input_assembly;
    input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly.primitiveRestartEnable = VK_FALSE;
    input_assembly.flags = 0;
    input_assembly.pNext = NULL;

    VkPipelineViewportStateCreateInfo viewport_state;
    viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_state.viewportCount = 1;
    viewport_state.scissorCount = 1;
    viewport_state.flags = 0;
    viewport_state.pNext = NULL;

    VkPipelineRasterizationStateCreateInfo rasterizer;
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.flags = 0;
    rasterizer.pNext = NULL;

    VkPipelineMultisampleStateCreateInfo multisampling;
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.pSampleMask = NULL;
    multisampling.flags = 0;
    multisampling.pNext = NULL;

    VkPipelineColorBlendAttachmentState blend_attachement;
    blend_attachement.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    blend_attachement.blendEnable = VK_FALSE;
    blend_attachement.colorBlendOp = 0;
    blend_attachement.alphaBlendOp = 0;
    blend_attachement.dstColorBlendFactor = 0;
    blend_attachement.dstAlphaBlendFactor = 0;
    blend_attachement.srcColorBlendFactor = 0;
    blend_attachement.srcAlphaBlendFactor = 0;

    VkPipelineColorBlendStateCreateInfo color_blending;
    color_blending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    color_blending.logicOpEnable = VK_FALSE;
    color_blending.logicOp = VK_LOGIC_OP_COPY;
    color_blending.attachmentCount = 1;
    color_blending.pAttachments = &blend_attachement;
    color_blending.blendConstants[0] = 0.0f;
    color_blending.blendConstants[1] = 0.0f;
    color_blending.blendConstants[2] = 0.0f;
    color_blending.blendConstants[3] = 0.0f;
    color_blending.flags = 0;
    color_blending.pNext = NULL;

    VkDynamicState dynamic_states[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamic_state;
    dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamic_state.dynamicStateCount = 2;
    dynamic_state.pDynamicStates = dynamic_states;
    dynamic_state.flags = 0;
    dynamic_state.pNext = NULL;

    VkPipelineLayoutCreateInfo layout_info;
    layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layout_info.setLayoutCount = 0;
    layout_info.pushConstantRangeCount = 0;
    layout_info.flags = 0;
    layout_info.pNext = NULL;

    if (vkCreatePipelineLayout(window->vulkan_info.device, &layout_info, NULL, &pipeline->layout) != VK_SUCCESS) {
        ERROR("Failed to create pipeline layout!");
    }

    TRACE("Created pipeline layout.");

    VkGraphicsPipelineCreateInfo pipeline_info;
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.stageCount = 2;
    pipeline_info.pStages = shader_stages;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.pInputAssemblyState = &input_assembly;
    pipeline_info.pViewportState = &viewport_state;
    pipeline_info.pRasterizationState = &rasterizer;
    pipeline_info.pMultisampleState = &multisampling;
    pipeline_info.pColorBlendState = &color_blending;
    pipeline_info.pDynamicState = &dynamic_state;
    pipeline_info.layout = pipeline->layout;
    pipeline_info.renderPass = window->vulkan_info.render_pass;
    pipeline_info.subpass = 0;
    pipeline_info.basePipelineIndex = 0;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    pipeline_info.pDepthStencilState = NULL;
    pipeline_info.pTessellationState = NULL;
    pipeline_info.flags = 0;
    pipeline_info.pNext = NULL;

    if (vkCreateGraphicsPipelines(window->vulkan_info.device, VK_NULL_HANDLE, 1, &pipeline_info, NULL, &pipeline->pipeline) != VK_SUCCESS) {
        ERROR("Failed to create graphics pipeline!");
    }

    TRACE("Created pipeline.");

    vkDestroyShaderModule(window->vulkan_info.device, frag_mod, NULL);
    vkDestroyShaderModule(window->vulkan_info.device, vert_mod, NULL);
}

void destroy_pipeline(window_t* window, pipeline_t* pipeline) {
    vkDeviceWaitIdle(window->vulkan_info.device);
    vkDestroyPipeline(window->vulkan_info.device, pipeline->pipeline, NULL);
    vkDestroyPipelineLayout(window->vulkan_info.device, pipeline->layout, NULL);
}

void create_vertex_buffer(window_t* window, buffer_t* vertex_buffer, void* verticies, size_t size) {
    VkDeviceSize buffer_size = size;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_mem;
    
    VkBufferCreateInfo staging_buff_info;
    staging_buff_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    staging_buff_info.size = buffer_size;
    staging_buff_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    staging_buff_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    staging_buff_info.pQueueFamilyIndices = NULL;
    staging_buff_info.flags = 0;
    staging_buff_info.pNext = NULL;

    if (vkCreateBuffer(window->vulkan_info.device, &staging_buff_info, NULL, &staging_buffer) != VK_SUCCESS) {
        ERROR("Failed to create buffer.");
    }

    VkMemoryRequirements staging_mem_requirements;
    vkGetBufferMemoryRequirements(window->vulkan_info.device, staging_buffer, &staging_mem_requirements);

    VkMemoryAllocateInfo staging_alloc_info;
    staging_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    staging_alloc_info.allocationSize = staging_mem_requirements.size;
    staging_alloc_info.pNext = NULL;

    VkPhysicalDeviceMemoryProperties staging_mem_props;
    vkGetPhysicalDeviceMemoryProperties(window->vulkan_info.physical_device, &staging_mem_props);

    uint32_t staging_mem_type_index = 0;
    for (uint32_t i = 0; i < staging_mem_props.memoryTypeCount; i++) {
        if ((staging_mem_requirements.memoryTypeBits & (1 << i)) && (staging_mem_props.memoryTypes[i].propertyFlags & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            staging_mem_type_index = i;
            break;
        }
    }
    staging_alloc_info.memoryTypeIndex = staging_mem_type_index;

    if (vkAllocateMemory(window->vulkan_info.device, &staging_alloc_info, NULL, &staging_mem) != VK_SUCCESS) {
        ERROR("Failed to allocate buffer memory.");
    }

    vkBindBufferMemory(window->vulkan_info.device, staging_buffer, staging_mem, 0);

    TRACE("Created staging vertex buffer.");

    void* data;
    vkMapMemory(window->vulkan_info.device, staging_mem, 0, buffer_size, 0, &data);
        memcpy(data, verticies, (size_t) buffer_size);
    vkUnmapMemory(window->vulkan_info.device, staging_mem);

    VkBufferCreateInfo buff_info;
    buff_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buff_info.size = buffer_size;
    buff_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    buff_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buff_info.pQueueFamilyIndices = NULL;
    buff_info.flags = 0;
    buff_info.pNext = NULL;

    if (vkCreateBuffer(window->vulkan_info.device, &buff_info, NULL, &vertex_buffer->buffer) != VK_SUCCESS) {
        ERROR("Failed to create buffer.");
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(window->vulkan_info.device, vertex_buffer->buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info;
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.pNext = NULL;

    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(window->vulkan_info.physical_device, &mem_props);

    uint32_t mem_type_index = 0;
    for (uint32_t i = 0; i < mem_props.memoryTypeCount; i++) {
        if ((mem_requirements.memoryTypeBits & (1 << i)) && (mem_props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            mem_type_index = i;
            break;
        }
    }
    alloc_info.memoryTypeIndex = mem_type_index;

    if (vkAllocateMemory(window->vulkan_info.device, &alloc_info, NULL, &vertex_buffer->memory) != VK_SUCCESS) {
        ERROR("Failed to allocate buffer memory.");
    }

    vkBindBufferMemory(window->vulkan_info.device, vertex_buffer->buffer, vertex_buffer->memory, 0);

    TRACE("Created vertex buffer.");

    VkCommandBufferAllocateInfo command_buffer_alloc_info;
    command_buffer_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_alloc_info.commandPool = window->vulkan_info.command_pool;
    command_buffer_alloc_info.commandBufferCount = 1;
    command_buffer_alloc_info.pNext = NULL;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(window->vulkan_info.device, &command_buffer_alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info;
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    begin_info.pInheritanceInfo = NULL;
    begin_info.pNext = NULL;

    vkBeginCommandBuffer(command_buffer, &begin_info);

    VkBufferCopy copyRegion;
    copyRegion.size = buffer_size;
    copyRegion.dstOffset = 0;
    copyRegion.srcOffset = 0;
    vkCmdCopyBuffer(command_buffer, staging_buffer, vertex_buffer->buffer, 1, &copyRegion);

    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command_buffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = NULL;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = NULL;
    submitInfo.pWaitDstStageMask = NULL;
    submitInfo.pNext = NULL;

    vkQueueSubmit(window->vulkan_info.graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(window->vulkan_info.graphics_queue);

    vkFreeCommandBuffers(window->vulkan_info.device, window->vulkan_info.command_pool, 1, &command_buffer);

    TRACE("Copied staging buffer into vertex buffer.");

    vkDestroyBuffer(window->vulkan_info.device, staging_buffer, NULL);
    vkFreeMemory(window->vulkan_info.device, staging_mem, NULL);
}

void destroy_vertex_buffer(window_t* window, buffer_t* vertex_buffer) {
    vkDeviceWaitIdle(window->vulkan_info.device);
    vkDestroyBuffer(window->vulkan_info.device, vertex_buffer->buffer, NULL);
    vkFreeMemory(window->vulkan_info.device, vertex_buffer->memory, NULL);
}

void create_index_buffer(window_t* window, buffer_t* index_buffer, void* indicies, size_t size) {
    VkDeviceSize buffer_size = size;

    VkBuffer staging_buffer;
    VkDeviceMemory staging_mem;
    
    VkBufferCreateInfo staging_buff_info;
    staging_buff_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    staging_buff_info.size = buffer_size;
    staging_buff_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    staging_buff_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    staging_buff_info.pQueueFamilyIndices = NULL;
    staging_buff_info.flags = 0;
    staging_buff_info.pNext = NULL;

    if (vkCreateBuffer(window->vulkan_info.device, &staging_buff_info, NULL, &staging_buffer) != VK_SUCCESS) {
        ERROR("Failed to create buffer.");
    }

    VkMemoryRequirements staging_mem_requirements;
    vkGetBufferMemoryRequirements(window->vulkan_info.device, staging_buffer, &staging_mem_requirements);

    VkMemoryAllocateInfo staging_alloc_info;
    staging_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    staging_alloc_info.allocationSize = staging_mem_requirements.size;
    staging_alloc_info.pNext = NULL;

    VkPhysicalDeviceMemoryProperties staging_mem_props;
    vkGetPhysicalDeviceMemoryProperties(window->vulkan_info.physical_device, &staging_mem_props);

    uint32_t staging_mem_type_index = 0;
    for (uint32_t i = 0; i < staging_mem_props.memoryTypeCount; i++) {
        if ((staging_mem_requirements.memoryTypeBits & (1 << i)) && (staging_mem_props.memoryTypes[i].propertyFlags & (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) == (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
            staging_mem_type_index = i;
            break;
        }
    }
    staging_alloc_info.memoryTypeIndex = staging_mem_type_index;

    if (vkAllocateMemory(window->vulkan_info.device, &staging_alloc_info, NULL, &staging_mem) != VK_SUCCESS) {
        ERROR("Failed to allocate buffer memory.");
    }

    vkBindBufferMemory(window->vulkan_info.device, staging_buffer, staging_mem, 0);

    TRACE("Created staging index buffer.");

    void* data;
    vkMapMemory(window->vulkan_info.device, staging_mem, 0, buffer_size, 0, &data);
        memcpy(data, indicies, (size_t) buffer_size);
    vkUnmapMemory(window->vulkan_info.device, staging_mem);

    VkBufferCreateInfo buff_info;
    buff_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buff_info.size = buffer_size;
    buff_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    buff_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buff_info.pQueueFamilyIndices = NULL;
    buff_info.flags = 0;
    buff_info.pNext = NULL;

    if (vkCreateBuffer(window->vulkan_info.device, &buff_info, NULL, &index_buffer->buffer) != VK_SUCCESS) {
        ERROR("Failed to create buffer.");
    }

    VkMemoryRequirements mem_requirements;
    vkGetBufferMemoryRequirements(window->vulkan_info.device, index_buffer->buffer, &mem_requirements);

    VkMemoryAllocateInfo alloc_info;
    alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    alloc_info.allocationSize = mem_requirements.size;
    alloc_info.pNext = NULL;

    VkPhysicalDeviceMemoryProperties mem_props;
    vkGetPhysicalDeviceMemoryProperties(window->vulkan_info.physical_device, &mem_props);

    uint32_t mem_type_index = 0;
    for (uint32_t i = 0; i < mem_props.memoryTypeCount; i++) {
        if ((mem_requirements.memoryTypeBits & (1 << i)) && (mem_props.memoryTypes[i].propertyFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) == VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
            mem_type_index = i;
            break;
        }
    }
    alloc_info.memoryTypeIndex = mem_type_index;

    if (vkAllocateMemory(window->vulkan_info.device, &alloc_info, NULL, &index_buffer->memory) != VK_SUCCESS) {
        ERROR("Failed to allocate buffer memory.");
    }

    vkBindBufferMemory(window->vulkan_info.device, index_buffer->buffer, index_buffer->memory, 0);

    TRACE("Created index buffer.");

    VkCommandBufferAllocateInfo command_buffer_alloc_info;
    command_buffer_alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    command_buffer_alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    command_buffer_alloc_info.commandPool = window->vulkan_info.command_pool;
    command_buffer_alloc_info.commandBufferCount = 1;
    command_buffer_alloc_info.pNext = NULL;

    VkCommandBuffer command_buffer;
    vkAllocateCommandBuffers(window->vulkan_info.device, &command_buffer_alloc_info, &command_buffer);

    VkCommandBufferBeginInfo begin_info;
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    begin_info.pInheritanceInfo = NULL;
    begin_info.pNext = NULL;

    vkBeginCommandBuffer(command_buffer, &begin_info);

    VkBufferCopy copyRegion;
    copyRegion.size = buffer_size;
    copyRegion.dstOffset = 0;
    copyRegion.srcOffset = 0;
    vkCmdCopyBuffer(command_buffer, staging_buffer, index_buffer->buffer, 1, &copyRegion);

    vkEndCommandBuffer(command_buffer);

    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &command_buffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = NULL;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = NULL;
    submitInfo.pWaitDstStageMask = NULL;
    submitInfo.pNext = NULL;

    vkQueueSubmit(window->vulkan_info.graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(window->vulkan_info.graphics_queue);

    vkFreeCommandBuffers(window->vulkan_info.device, window->vulkan_info.command_pool, 1, &command_buffer);

    TRACE("Copied staging buffer into vertex buffer.");

    vkDestroyBuffer(window->vulkan_info.device, staging_buffer, NULL);
    vkFreeMemory(window->vulkan_info.device, staging_mem, NULL);
}

void destroy_index_buffer(window_t* window, buffer_t* index_buffer) {
    vkDeviceWaitIdle(window->vulkan_info.device);
    vkDestroyBuffer(window->vulkan_info.device, index_buffer->buffer, NULL);
    vkFreeMemory(window->vulkan_info.device, index_buffer->memory, NULL);
}
