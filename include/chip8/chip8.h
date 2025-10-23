#pragma once
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Shaders/shader.h>
#include <cmath>

#define MAX_MEM 4096
#define MAX_STACK 16
#define WIDTH 64
#define HEIGHT 32

class Chip8 {

private:
    unsigned char memory[MAX_MEM] = {};
    unsigned char screen[WIDTH*HEIGHT] = {};
    unsigned short PC;
    unsigned short I;
    unsigned short stack[MAX_STACK] = {};
    int top = -1;
    unsigned char delay;
    unsigned char sound;
    unsigned char registers[16] = {};

    //stack operations
    void push(unsigned short x);
    unsigned short pop();
    unsigned short peek();
   
    //CHIP-8 Functionality 
    unsigned short fetch();
    void decode(unsigned short instruction);


    //opcodes
    void clear(); //00E0 Clear Screen
    void jump(unsigned short addr); //1NNN Jump
    void return_subroutine(); //00EE Return from subroutine
    void start_subroutine(unsigned short addr); //2NNN Start subroutine at NNN
    void set(unsigned char reg, unsigned char val); //6XNN set register VX to NN
    void add(unsigned char reg, unsigned char val); //7XNN add NN to register VX
    void set_index(unsigned short index); //ANNN set index to NNN
    void display(unsigned char x, unsigned char y, unsigned char height);


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
    
    void initDisplay();

    void start();
};