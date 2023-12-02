#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#include "plug.h"

#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>

State state = {0};

const char* plug_filename = "build/libplug.dylib";
void* plug_handle = NULL;

plug_init_t plug_init;
plug_pre_update_t plug_pre_update;
plug_update_t plug_update;
plug_post_update_t plug_post_update;
plug_terminate_t plug_terminate;

bool load_plugin()
{
    if (plug_handle) dlclose(plug_handle);

    plug_handle = dlopen(plug_filename, RTLD_NOW);
    if (plug_handle == NULL) {
        fprintf(stderr, "Failed to load libplug.dylib: %s\n", dlerror());
        return false;
    }

    plug_init = dlsym(plug_handle, "plug_init");
    if (plug_init == NULL) {
        fprintf(stderr, "Could not find plug_init: %s", dlerror());
        return false;
    }

    plug_pre_update = dlsym(plug_handle, "plug_pre_update");
    if (plug_pre_update == NULL) {
        fprintf(stderr, "Could not find plug_pre_update: %s", dlerror());
        return false;
    }

    plug_update = dlsym(plug_handle, "plug_update");
    if (plug_update == NULL) {
        fprintf(stderr, "Could not find plug_update: %s", dlerror());
        return false;
    }

    plug_post_update = dlsym(plug_handle, "plug_post_update");
    if (plug_post_update == NULL) {
        fprintf(stderr, "Could not find plug_post_update: %s", dlerror());
        return false;
    }

    plug_terminate = dlsym(plug_handle, "plug_terminate");
    if (plug_terminate == NULL) {
        fprintf(stderr, "Could not find plug_terminate: %s", dlerror());
        return false;
    }

    return true;
}

typedef struct {
    double* items;
    size_t count;
    size_t capacity;
} Array;

int main(void) {
    if (!load_plugin()) {
        return EXIT_FAILURE;
    }

    if (!plug_init(&state)) {
        return EXIT_FAILURE;
    }

    while (!glfwWindowShouldClose(state.window)) {
        if (glfwGetKey(state.window, GLFW_KEY_R) == GLFW_PRESS) {
          plug_pre_update(&state);
          if (!load_plugin()) {
            return EXIT_FAILURE;
          }
          plug_post_update(&state);
        }

        plug_update(&state);
    }

    plug_terminate(&state);
    return EXIT_SUCCESS;
}
