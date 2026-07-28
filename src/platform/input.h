#ifndef OVERTURE_INPUT
#define OVERTURE_INPUT

#include <stdint.h>
#include <GLFW/glfw3.h>

uint32_t key_just_pressed(uint32_t key);
uint32_t key_pressed(uint32_t key);
uint32_t key_just_released(uint32_t key);
uint32_t key_released(uint32_t key);

void key_callback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods);

#endif
