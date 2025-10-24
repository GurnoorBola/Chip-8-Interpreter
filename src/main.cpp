#include <iostream>
#include <glad.h>
#include <glfw3.h>
#include <Shaders/shader.h>
#include <cmath>
#include <chip8/chip8.h>

int main() {
    Chip8 hello = Chip8();
    hello.initDisplay();
    hello.start();
    return 0;
}