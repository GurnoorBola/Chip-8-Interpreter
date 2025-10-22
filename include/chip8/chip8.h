#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Shaders/shader.h>
#include <cmath>

#define MAX_MEM 4096
#define WIDTH 64
#define HEIGHT 32

class Chip8 {

private:
    unsigned char memory[MAX_MEM] = {};
    unsigned char screen[64*32] = {};
    int PC;
    unsigned short I;
    unsigned short stack[16] = {};
    unsigned char delay;
    unsigned char sound;
    unsigned char registers[16] = {};

    //opengl window initialization
    GLFWwindow* window;
    Shader shader;
   
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    unsigned int texture;
    
    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void processInput(GLFWwindow *window);

public:
    Chip8();

    unsigned short fetch();

    int decode(unsigned short instruction);
    
    int initDisplay();

    int start();
};