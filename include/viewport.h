#pragma once

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

class Viewport {
public:
    Viewport(int width, int height)
        : width_(width), height_(height) {
        if (!glfwInit()) {
            return;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window_ = glfwCreateWindow(width_, height_, "Ray Tracer Viewport", NULL, NULL);
        if (!window_) {
            glfwTerminate();
            return;
        }

        glfwMakeContextCurrent(window_);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            glfwDestroyWindow(window_);
            window_ = nullptr;
            glfwTerminate();
            return;
        }

        glViewport(0, 0, width_, height_);

        glGenTextures(1, &textureID_);
        glBindTexture(GL_TEXTURE_2D, textureID_);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenFramebuffers(1, &fboID_);
        glBindFramebuffer(GL_FRAMEBUFFER, fboID_);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureID_, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window_);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window_);
    }

    ~Viewport() {
        if (fboID_ != 0) {
            glDeleteFramebuffers(1, &fboID_);
        }
        if (textureID_ != 0) {
            glDeleteTextures(1, &textureID_);
        }
        if (window_ != nullptr) {
            glfwDestroyWindow(window_);
        }
        glfwTerminate();
    }

    GLFWwindow* window() const {
        return window_;
    }

    bool ready() const {
        return window_ != nullptr;
    }

    void present(const std::vector<unsigned char>& buffer) {
        if (!window_) {
            return;
        }

        glBindTexture(GL_TEXTURE_2D, textureID_);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());

        glBindFramebuffer(GL_READ_FRAMEBUFFER, fboID_);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, width_, height_, 0, 0, width_, height_, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        glfwSwapBuffers(window_);
        glfwPollEvents();
    }

private:
    GLFWwindow* window_ = nullptr;
    GLuint textureID_ = 0;
    GLuint fboID_ = 0;
    int width_ = 0;
    int height_ = 0;
};

#endif // VIEWPORT_H