#pragma once

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <OpenGL/gl3ext.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <algorithm>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <string>
#include <filesystem>

namespace fs = std::filesystem;

struct Shader {
    fs::path filename;
    std::string source_code;
};

struct Shaders {
    std::unordered_map<std::string, Shader> items;
};

struct Assets {
    Shaders shaders;
};

typedef struct {
    const char* title;
    GLFWwindow* window;

    GLuint shader_program;
    GLuint vao;

    Assets assets;
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
