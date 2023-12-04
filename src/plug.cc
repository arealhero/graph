#define GL_SILENCE_DEPRECATION

#include "plug.h"

#include "types.h"

#include <stdio.h>
#include <stdbool.h>

#include <fstream>
#include <iostream>
#include <sstream>
#include <optional>
#include <filesystem>

extern "C" {
#define PLUG(name, retval, args...) retval name(args)
PLUG_DECLARATIONS
#undef PLUG
}

namespace fs = std::filesystem;

Maybe<Shader> find_shader(const Shaders& shaders, const fs::path& filename)
{
    auto it = shaders.items.find(filename);
    if (it != shaders.items.end()) {
        return it->second;
    }

    return Nothing;
}

Maybe<std::string_view> get_shader(Assets& assets, const fs::path& filename)
{
    auto cached_shader = find_shader(assets.shaders, filename);
    if (cached_shader.has_value()) {
        return cached_shader.value().source_code;
    }

    if (!fs::exists(filename)) {
        std::cerr << "File " << filename << " does not exist.\n";
        return Nothing;
    }

    auto file_size = fs::file_size(filename);

    std::ifstream fin(filename);
    if (fin.fail()) {
        std::cerr << "Could not open shader " << filename << '\n';
        return Nothing;
    }

    std::stringstream buffer;
    buffer << fin.rdbuf();
    fin.close();

    Shader shader{.filename = filename, .source_code = buffer.str()};
    const auto [it, ok] = assets.shaders.items.insert({filename, std::move(shader)});
    return it->second.source_code;
}

void window_focus_callback(GLFWwindow* window, int focused) {
    // TODO(vsharshukov): cap FPS to like 30 if window is not in focus
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

static Maybe<GLuint> compile_shader(State* state, GLenum type, const char* filename) {
    GLuint shader = glCreateShader(type);
    auto source_code = get_shader(state->assets, filename);
    if (!source_code.has_value()) {
        fprintf(stderr, "Shader not found: %s\n", filename);
        return false;
    }

    auto ptr = source_code.value().data();
    glShaderSource(shader, 1, &ptr, NULL);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetShaderInfoLog(shader, 512, NULL, info_log);
        fprintf(stderr, "Could not compile shader %s: %s\n", filename, info_log);
        return Nothing;
    }

    return shader;
}

bool create_shader_program(State* state)
{
    auto vertex_shader = compile_shader(state, GL_VERTEX_SHADER, "shaders/shader.vert");
    if (!vertex_shader.has_value()) {
        std::cerr << "Could not compile vertex shader\n";
        return false;
    }

    auto fragment_shader = compile_shader(state, GL_FRAGMENT_SHADER, "shaders/shader.frag");
    if (!fragment_shader.has_value()) {
        std::cerr << "Could not compile fragment shader\n";
        return false;
    }

    state->shader_program = glCreateProgram();
    glAttachShader(state->shader_program, vertex_shader.value());
    glAttachShader(state->shader_program, fragment_shader.value());
    glLinkProgram(state->shader_program);

    int success;
    glGetProgramiv(state->shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        char info_log[512];
        glGetProgramInfoLog(state->shader_program, 512, NULL, info_log);
        std::cerr << "Could not link shader program: " << info_log << "\n";
        return false;
    }

    glUseProgram(state->shader_program);
    glDeleteShader(vertex_shader.value());
    glDeleteShader(fragment_shader.value());

    return state->shader_program;
}

bool plug_init(State* state)
{
    state->title = "Graph";

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    state->window = glfwCreateWindow(800, 600, state->title, NULL, NULL);
    if (!state->window) {
        std::cerr << "Failed to create window in GLFW\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(state->window);

    glViewport(0, 0, 800, 600);
    set_callbacks(state);

    state->shader_program = create_shader_program(state);

    float vertices[] = {
      -0.5f, -0.5f, 0.0f,
       0.5f, -0.5f, 0.0f,
       0.0f,  0.5f, 0.0f,
    };
    GLuint vbo;
    glGenBuffers(1, &vbo);

    glGenVertexArrays(1, &state->vao);

    glBindVertexArray(state->vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    return true;
}

void clear_assets(State* state) {
    auto& assets = state->assets;
    assets.shaders.items.clear();
}

// FIXME: rename to plug_pre_reload
void plug_pre_update(State* state) {
    unset_callbacks(state);
    clear_assets(state);
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

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    {
        glUseProgram(state->shader_program);
        glBindVertexArray(state->vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    glfwSwapBuffers(state->window);
    glfwPollEvents();
}

// FIXME: rename to plug_post_reload
void plug_post_update(State* state) {
    set_callbacks(state);
    create_shader_program(state);
}

void plug_terminate(State* state)
{
    glfwTerminate();
}
