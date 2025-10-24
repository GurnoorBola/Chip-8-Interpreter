#include <chip8/chip8.h>
#include <cstring>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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


/*-----------------[Special Member Functions]-----------------*/
Chip8::Chip8(){
    //copy fonts to memory (0x050 - 0x09F)
    memcpy(&memory[0x50], fonts, sizeof(fonts));

    //int length = WIDTH*HEIGHT;
    //for (int i=0; i < length; i++){
        //screen[i] = 255;
    //}

};


/*-----------------[Stack]-----------------*/
void Chip8::push(unsigned short x){
    if (top == MAX_STACK-1){
        std::cerr << "Cannot push; Stack full" << std::endl;
        return;
    }
    stack[++top] = x;
}

unsigned short Chip8::pop(){
    if (top == -1) {
        std::cerr << "Cannot pop; Stack empty" << std::endl;
        return 0;
    }
    return stack[top--];
}

unsigned short Chip8::peek(){
    if (top == -1) {
        std::cerr << "Cannot peek; Stack empty" << std::endl;
        return 0;
    }
    return stack[top];
}



/*-----------------[Graphics]-----------------*/
void Chip8::framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, WIDTH*20, HEIGHT*20);
}

void Chip8::processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

void Chip8::initDisplay(){
    glfwInit();
    glfwWindowHint(GLFW_RESIZABLE, 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //make window
    window = glfwCreateWindow(WIDTH*20, HEIGHT*20, "Test", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        return;
    }
    glfwMakeContextCurrent(window);


    //intialize GLAD for OpenGl functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
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

    return;
}

void Chip8::start(){
    //small test display 1A
    Chip8::set_index(0x50 + (1)*5);
    Chip8::display(1, 1, 5);
    Chip8::set_index(0x50 + (0xA)*5);
    Chip8::display(7, 1, 5);

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
    return;
}


/*-----------------[Main Functionality]-----------------*/

//get 2 byte instruction at PC location and increment by 2
unsigned short Chip8::fetch(){
    if (PC > MAX_MEM-2){
        std::cerr << "PC at end of memory; failed to fetch" << std::endl;
        return 0;
    }
    unsigned short mask = 0xFFFF;
    mask &= ((memory[PC] << 8) + (memory[PC + 1]));
    PC += 2;
    return mask;
}

//determine what to do based on instruction
void Chip8::decode(unsigned short instruction){
    switch (instruction >> 12){
        case 0x0:
            switch (instruction & 0xFFF){
                case 0x0E0:
                    Chip8::clear();
                    break;

                case 0x0EE:
                    Chip8::return_subroutine();
                    break;

                default:
                    std::cerr << "Invalid opcode" << std::endl;
            }
            break;

        case 0x1:
            break;

        case 0x2:
            break;
            
        case 0x3:
            break;

        case 0x4:
            break;

        case 0x5:
            break;

        case 0x6:
            
            break;

        case 0x7:
            break;

        case 0x8:
            break;

        case 0x9:
            break;
            
        case 0xA:
            break;

        case 0xB:
            break;

        case 0xC:
            break;
            
        case 0xD:
            break;

        case 0xE:
            break;

        case 0xF:
            break;
    }
}


/*-----------------[Opcodes]-----------------*/

void Chip8::clear(){
    std::memset(screen, 0, WIDTH*HEIGHT*sizeof(unsigned char));
}

void Chip8::jump(unsigned short addr){
    PC = addr;
}

void Chip8::return_subroutine(){
    PC = Chip8::pop();
}

void Chip8::start_subroutine(unsigned short addr){
    Chip8::push(PC);
    PC = addr; 
}

void Chip8::set(unsigned char reg, unsigned char val){
    registers[reg] = val;
}

void Chip8::add(unsigned char reg, unsigned char val) {
    registers[reg] += val;
}

void Chip8::set_index(unsigned short index){
    I = index;
}


//get sprite from memory location pointed at by I and display height bytes of it
void Chip8::display(unsigned char x, unsigned char y, unsigned char height){
    //initialize VF register to be 0 and make sure x and y are on screen
    registers[0xF] = 0;
    unsigned char sprite_index = I;
    x %= WIDTH;
    y %= HEIGHT;

    //loop through rows and cols of the screen and update individual screen pixels
    for (int row = y; row < (y + height); row++){
        //stop if off of screen
        if (row >= HEIGHT) { break; }
        
        //read from left to right so we start with 7th pixel 0-indexed
        //we also get the current row of the character
        unsigned char sprite_row = memory[sprite_index];
        int pixel = 7;

        for (int col = x; col < (x + 8); col++){
            //stop if off of screen
            if (col >= WIDTH){ break; }

            unsigned char bit = -((sprite_row >> pixel) & 1);
            pixel--;

            int screen_index = (row*WIDTH) + col;
            if (screen[screen_index] & bit) { registers[0xF] = 1; }
            screen[screen_index] ^= bit;
        }
        sprite_index++;
    }

    //update texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, WIDTH, HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, screen);
}