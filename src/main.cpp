#include <iostream>
#include <glad.h>
#include <glfw3.h>
#include <Shaders/shader.h>
#include <cmath>
#include <chip8/chip8.h>

int main() {
    Chip8 chip8 = Chip8();
    chip8.initDisplay();

    while (1) {
        chip8.processInput();

        if (chip8.stop) {
            break;
        }

        chip8.emulate_cycle();
    }
    chip8.terminate();
    return 0;
}