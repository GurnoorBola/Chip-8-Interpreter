#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Shaders/shader.h>
#include <cmath>


class Chip8 {

private:
    unsigned char memory[4096] = {};
    unsigned char screen[64*32] = {};
    unsigned char *PC;
    unsigned short I;
    unsigned short stack[16] = {};
    unsigned char delay;
    unsigned char sound;
    unsigned char registers[16] = {};

    //opengl window initialization
    GLFWwindow* window;
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void processInput(GLFWwindow *window);

public:
    Chip8();

    unsigned short fetch();

    int decode(unsigned short instruction);
    
    int initDisplay();
};