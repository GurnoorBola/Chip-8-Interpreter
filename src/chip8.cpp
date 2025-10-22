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
    //copy fonts to memory (0x050 - 0x09F)
    memcpy(&memory[0x50], fonts, sizeof(fonts));

    int length = WIDTH*HEIGHT;
    for (int i=0; i < length; i++){
        screen[i] = 255;
    }

};

void Chip8::framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, WIDTH*20, HEIGHT*20);
}

void Chip8::processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

int Chip8::initDisplay(){
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //make window
    window = glfwCreateWindow(WIDTH*20, HEIGHT*20, "Test", NULL, NULL);
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
    glViewport(0, 0, WIDTH*20, HEIGHT*20);
    glfwSetFramebufferSizeCallback(window, Chip8::framebuffer_size_callback);

    //set clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    //create shader object
    shader = Shader("shaders/shader.vs", "shaders/shader.fs");
    shader.use();

    float vertices[] = {
        // positions            // texture coords
        1.0f,  1.0f, 0.0f,     1.0f, 1.0f,   // top right
        1.0f, -1.0f, 0.0f,     1.0f, 0.0f,   // bottom right
        -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,   // bottom left
        -1.0f,  1.0f, 0.0f,     0.0f, 1.0f    // top left 
    };

    unsigned int indices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    }; 
    
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenVertexArrays(1, &VAO);
   
    //initialize texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, screen);

    //initialize VAO
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    return 0;
}

int Chip8::start(){
    while (!glfwWindowShouldClose(window)){
        processInput(window);

        glClear(GL_COLOR_BUFFER_BIT);

        //draw object
        shader.use();

        glBindTexture(GL_TEXTURE_2D, texture);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

//get 2 byte instruction at PC location and increment by 2
unsigned short Chip8::fetch(){
    if (PC > MAX_MEM-2){
        std::cout << "PC at end of memory; failed to fetch" << std::endl;
        return 0;
    }
    unsigned short mask = 0xFFFF;
    mask &= ((memory[PC] << 8) + (memory[PC + 1]));
    PC += 2;
    return mask;
}

int Chip8::decode(unsigned short instruction){
    return 0;    
}