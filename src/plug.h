#ifndef GRAPH_PLUG_H_
#define GRAPH_PLUG_H_

#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    const char* title;
    GLFWwindow* window;
} State;

#define PLUG_DECLARATIONS                                                      \
  PLUG(plug_init, bool, State *);                                              \
  PLUG(plug_terminate, void, State *);                                         \
  PLUG(plug_pre_update, void, State *);                                        \
  PLUG(plug_update, void, State *);                                            \
  PLUG(plug_post_update, void, State *);

#define PLUG(name, retval, args...) typedef retval (*name##_t)(args)
PLUG_DECLARATIONS
#undef PLUG



#endif // GRAPH_PLUG_H_
