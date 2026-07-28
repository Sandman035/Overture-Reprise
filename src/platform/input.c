#include "platform/input.h"
#include "core/log.h"
#include "core/systems.h"
#include <stdint.h>

typedef enum {
    RELEASED,
    JUST_RELEASED,
    PRESSED,
    JUST_PRESSED,
} key_state_t;

// TODO: maybe per window key input???

key_state_t key_states[GLFW_KEY_LAST + 1];

uint32_t key_just_pressed(uint32_t key) {
    if (key > GLFW_KEY_LAST) {
        WARN("Invalid key: %d", key);
        return 0;
    }
    key_state_t state = key_states[key];

    return state == JUST_PRESSED;
}

uint32_t key_pressed(uint32_t key) {
    if (key > GLFW_KEY_LAST) {
        WARN("Invalid key: %d", key);
        return 0;
    }
    key_state_t state = key_states[key];

    return state == JUST_PRESSED || state == PRESSED;
}

uint32_t key_just_released(uint32_t key) {
    if (key > GLFW_KEY_LAST) {
        WARN("Invalid key: %d", key);
        return 0;
    }
    key_state_t state = key_states[key];

    return state == JUST_RELEASED;
}

uint32_t key_released(uint32_t key) {
    if (key > GLFW_KEY_LAST) {
        WARN("Invalid key: %d", key);
        return 0;
    }
    key_state_t state = key_states[key];

    return state == JUST_RELEASED || state == RELEASED;
}

void update_key_states() {
    for (size_t i = 0; i < GLFW_KEY_LAST + 1; i++) {
        switch (key_states[i]) {
            case JUST_PRESSED:
                TRACE("Key '%s' is still pressed.", glfwGetKeyName(i, 0));
                key_states[i] = PRESSED;
                break;
            case JUST_RELEASED:
                TRACE("Key '%s' is still released.", glfwGetKeyName(i, 0));
                key_states[i] = RELEASED;
                break;
            default:
                break;
        }
    }
}

REGISTER_SYSTEM_FRONT(update_key_states, PRE_UPDATE);

void key_callback(GLFWwindow* window, int32_t key, int32_t scancode, int32_t action, int32_t mods) {
    switch (action) {
        case GLFW_PRESS:
            TRACE("Key '%s' was just pressed.", glfwGetKeyName(key, 0));
            key_states[key] = JUST_PRESSED;
            break;
        case GLFW_RELEASE:
            TRACE("Key '%s' was just released.", glfwGetKeyName(key, 0));
            key_states[key] = JUST_RELEASED;
            break;
        default:
            break;
    }
}
