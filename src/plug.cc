#define GL_SILENCE_DEPRECATION

#include "plug.h"

#include <stdio.h>
#include <stdbool.h>

extern "C" {
#define PLUG(name, retval, args...) retval name(args)
PLUG_DECLARATIONS
#undef PLUG
}

void window_focus_callback(GLFWwindow* window, int focused) {
    // TODO(vsharshukov): cap FPS to like 30 if window is not in focus
    if (focused) {
        puts("focused!");
    } else {
        puts("unfocused!");
    }
}

void fb_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

static void set_callbacks(State* state) {
    glfwSetFramebufferSizeCallback(state->window, fb_size_callback);
    glfwSetWindowFocusCallback(state->window, window_focus_callback);

    glfwSetWindowUserPointer(state->window, state);
}

static void unset_callbacks(State* state) {
    glfwSetFramebufferSizeCallback(state->window, NULL);
    glfwSetWindowFocusCallback(state->window, NULL);

    glfwSetWindowUserPointer(state->window, NULL);
}

bool plug_init(State* state)
{
    state->title = "Graph";

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    state->window = glfwCreateWindow(800, 600, state->title, NULL, NULL);
    if (!state->window) {
        fprintf(stderr, "Failed to create window in GLFW\n");
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(state->window);

    glViewport(0, 0, 800, 600);
    set_callbacks(state);

    return true;
}

void plug_pre_update(State *state) {
    unset_callbacks(state);
}

void plug_update(State* state)
{
    if (glfwGetKey(state->window, GLFW_KEY_ESCAPE) == GLFW_PRESS ||
        glfwGetKey(state->window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(state->window, GL_TRUE);
    }

    if (glfwGetKey(state->window, GLFW_KEY_P) == GLFW_PRESS) {
        puts("Hello from Graph");
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(state->window);
    glfwPollEvents();
}

void plug_post_update(State * state) {
    set_callbacks(state);
}

void plug_terminate(State* state)
{
    glfwTerminate();
}
