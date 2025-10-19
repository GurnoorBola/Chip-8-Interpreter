#include <chip8/chip8.h>
#include <cstring>

unsigned char fonts[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8(){
    PC = memory;
    //copy fonts to memory (0x050 - 0x09F)
    memcpy(&memory[0x50], fonts, sizeof(fonts));
};

void Chip8::framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, 800, 600);
}

void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

int Chip8::initDisplay(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //make window
    window = glfwCreateWindow(800, 600, "Test", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        return 1;
    }
    glfwMakeContextCurrent(window);


    //intialize GLAD for OpenGl functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        return 1;
    }

    //create opengl viewport and adjust it when window is resized
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, Chip8::framebuffer_size_callback);

    //set clear color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    //create shader object
    Shader shader("shaders/shader.vs", "shaders/shader.fs");
    shader.use();
    shader.setFloat("offset", 0.1f);
}

//get 2 byte instruction at PC location and increment by 2
unsigned short Chip8::fetch(){
    unsigned short mask = 0xFFFF;
    mask &= ((*PC << 8) + (*(PC + 1)));
    PC += 2;
    return mask;
}

int Chip8::decode(unsigned short instruction){
    
}