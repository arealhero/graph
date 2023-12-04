#include "plug.h"

#include <stdio.h>
#include <stdlib.h>

#include <dlfcn.h>

State state = {0};

const char* plug_filename = "build/libplug.dylib";
void* plug_handle = NULL;

#define PLUG(name, retval, args...) name##_t name
PLUG_DECLARATIONS
#undef PLUG

bool load_plugin()
{
    if (plug_handle) dlclose(plug_handle);

    plug_handle = dlopen(plug_filename, RTLD_NOW);
    if (plug_handle == NULL) {
        fprintf(stderr, "Failed to load libplug.dylib: %s\n", dlerror());
        return false;
    }

#define PLUG(name, retval, args...)                                            \
  name = (name##_t) dlsym(plug_handle, #name);                                  \
  if (name == NULL) {                                                          \
    fprintf(stderr, "Could not find %s: %s", #name, dlerror());                \
    return false;                                                              \
  }
    PLUG_DECLARATIONS
#undef PLUG

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
