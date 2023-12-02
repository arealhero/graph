#ifndef GRAPH_PLUG_H_
#define GRAPH_PLUG_H_

#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#include <stdbool.h>

typedef struct {
    const char* title;
    GLFWwindow* window;
} State;

typedef bool (*plug_init_t)(State*);
typedef void (*plug_terminate_t)(State*);

typedef void (*plug_pre_update_t)(State*);
typedef void (*plug_update_t)(State*);
typedef void (*plug_post_update_t)(State*);

#endif // GRAPH_PLUG_H_
