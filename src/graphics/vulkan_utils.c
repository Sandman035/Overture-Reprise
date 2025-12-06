#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vulkan/vulkan_core.h>
#include "graphics/vulkan_utils.h"
#include "core/log.h"

VkResult create_debug_utils_messenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != NULL) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void destroy_debug_utils_messenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != NULL) {
        func(instance, debugMessenger, pAllocator);
    }
}

VkBool32 check_vulkan_layer_support(const char** layers, uint32_t n) {
    uint32_t layer_count;
    vkEnumerateInstanceLayerProperties(&layer_count, NULL);

    VkLayerProperties* available_layers = malloc(sizeof(VkLayerProperties) * layer_count);
    vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

    for (uint32_t i = 0; i < n; i++) {
        VkBool32 layer_found = VK_FALSE;

        for (uint32_t j = 0; j < layer_count; j++) {
            if (strcmp(layers[i], available_layers[j].layerName) == 0) {
                layer_found = VK_TRUE;
                break;
            }
        }

        if (layer_found == VK_FALSE) {
            return VK_FALSE;
        }
    }

    return VK_TRUE;
}

VkBool32 check_vulkan_extention_support(VkPhysicalDevice device, const char** extensions, uint32_t n) {
    uint32_t ext_count;
    vkEnumerateDeviceExtensionProperties(device, NULL, &ext_count, NULL);

    VkExtensionProperties* available_exts = malloc(sizeof(VkExtensionProperties) * ext_count);
    vkEnumerateDeviceExtensionProperties(device, NULL, &ext_count, available_exts);

    for (uint32_t i = 0; i < n; i++) {
        VkBool32 etx_found = VK_FALSE;

        for (uint32_t j = 0; j < ext_count; j++) {
            if (strcmp(extensions[i], available_exts[j].extensionName) == 0) {
                etx_found = VK_TRUE;
                break;
            }
        }

        if (etx_found == VK_FALSE) {
            return VK_FALSE;
        }
    }

    return VK_TRUE;
}

//temp
shader_buffer_t load_shader(const char* path) {
    shader_buffer_t buffer;

    FILE* fp = fopen(path, "rb");
    if (!fp) {
        ERROR("Failed to open file");
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    unsigned char* buff = (unsigned char *)malloc(file_size);
    if (!buff) {
        ERROR("Failed to allocate memory");
        fclose(fp);
    }

    size_t bytes_read = fread(buff, 1, file_size, fp);
    if (bytes_read != file_size) {
        perror("Failed to read file");
        free(buff);
        fclose(fp);
    }

    fclose(fp);

    buffer.data = (unsigned int *)buff;
    buffer.len = file_size; // this includes the size of uint32_t

    return buffer;
}
