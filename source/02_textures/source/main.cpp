/*
 main

 Copyright 2012 Thomas Dalling - http://tomdalling.com/

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "platform.hpp"

// third-party libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// include GLM (OpenGL Mathematics)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// standard C++ libraries
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>

// tdogl classes
#include "tdogl/Program.h"
#include "tdogl/Texture.h"

// constants
const glm::vec2 SCREEN_SIZE(800, 600);

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Movement control
void do_movement();

// globals
GLFWwindow* gWindow = NULL;
tdogl::Texture* gTexture = NULL;
tdogl::Program* gLightingShader = NULL;
tdogl::Program* gLambShader = NULL;
GLuint gVAO = 0;
GLuint gVBO = 0;

// define lighting position
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// move transform matrix to global since it should define in load function and replay in render iteration
glm::mat4 gModel;
glm::mat4 gView;
glm::mat4 gProjection;

// define global position and direction for camera sorrounding
glm::vec3 gCameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 gCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 gCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
bool keys[1024];

// loads the vertex shader and fragment shader, and links them to make the global gProgram
static void LoadShaders() {
    std::vector<tdogl::Shader> lightShaders;
    std::vector<tdogl::Shader> lampShader;
    lightShaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("lighting.vs"), GL_VERTEX_SHADER));
    lightShaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("lighting.frag"), GL_FRAGMENT_SHADER));
    gLightingShader = new tdogl::Program(lightShaders);
    lampShader.push_back(tdogl::Shader::shaderFromFile(ResourcePath("lighting.vs"), GL_VERTEX_SHADER));
    lampShader.push_back(tdogl::Shader::shaderFromFile(ResourcePath("lighting.frag"), GL_FRAGMENT_SHADER));
    gLambShader = new tdogl::Program(lampShader);
}


// loads a triangle into the VAO global
static void LoadRectangle() {   
    
    GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,
        0.5f,  -0.5f, -0.5f,
        0.5f,   0.5f, -0.5f,
        0.5f,   0.5f, -0.5f,
       -0.5f,   0.5f, -0.5f,
       -0.5f,  -0.5f, -0.5f,
        
       -0.5f,  -0.5f,  0.5f,
        0.5f,  -0.5f,  0.5f,
        0.5f,   0.5f,  0.5f,
        0.5f,   0.5f,  0.5f,
       -0.5f,   0.5f,  0.5f,
       -0.5f,  -0.5f,  0.5f,
        
       -0.5f,   0.5f,  0.5f,
       -0.5f,   0.5f, -0.5f,
       -0.5f,  -0.5f, -0.5f,
       -0.5f,  -0.5f, -0.5f,
       -0.5f,  -0.5f,  0.5f,
       -0.5f,   0.5f,  0.5f,
        
        0.5f,   0.5f,  0.5f,
        0.5f,   0.5f, -0.5f,
        0.5f,  -0.5f, -0.5f,
        0.5f,  -0.5f, -0.5f,
        0.5f,  -0.5f,  0.5f,
        0.5f,   0.5f,  0.5f,
        
       -0.5f,  -0.5f, -0.5f,
        0.5f,  -0.5f, -0.5f,
        0.5f,  -0.5f,  0.5f,
        0.5f,  -0.5f,  0.5f,
       -0.5f,  -0.5f,  0.5f,
       -0.5f,  -0.5f, -0.5f,
        
       -0.5f,  0.5f, -0.5f,
        0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f,
       -0.5f,  0.5f,  0.5f,
       -0.5f,  0.5f, -0.5f,
    };
    
    // define model matrix to transform all object's vertices to global world space
    gModel = glm::rotate(gModel, -10.0f, glm::vec3(1.0f, 0.0f, 0.0f));
    // move slightly backward in the scene so that object's visible
    gView = glm::translate(gView, glm::vec3(0.0f, 0.0f, -3.0f));
    // define the projection matrix
    gProjection = glm::perspective(45.0f, (GLfloat)SCREEN_SIZE.x / (GLfloat)SCREEN_SIZE.y, 0.1f, 100.0f);
    
    // make and bind the VAO
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);
    
    // make and bind the VBO
    glGenBuffers(1, &gVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Link vertex attribute for position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    // unbind the VAO
    glBindVertexArray(0);
}


// loads the file "hazard.png" into gTexture
static void LoadTexture(std::string image) {
    tdogl::Bitmap bmp = tdogl::Bitmap::bitmapFromFile(ResourcePath(image));
    //bmp.flipVertically();
    gTexture = new tdogl::Texture(bmp);
}


// draws a single frame
static void Render() {
    // clear everything
    glClearColor(0.1f, 0.1f, 0.2f, 2.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // bind the program (the shaders)
    gLightingShader->use();
    gLightingShader->setUniform("objectColor", 1.0f, 0.5f, 0.31f);
    gLightingShader->setUniform("lightColor", 1.0f, 0.5f, 1.0f);
    
    //suspect view not display
    gView = glm::lookAt(gCameraPos, gCameraPos + gCameraFront, gCameraUp);
    
    // send the transform matrix to the vertex sharder
    gLightingShader->setUniform("model", gModel, GL_FALSE);
    gLightingShader->setUniform("view", gView, GL_FALSE);
    gLightingShader->setUniform("projection", gProjection, GL_FALSE);
    
    // draw container
    glBindVertexArray(gVAO);
    glm::mat4 model;
    gLightingShader->setUniform("model", model, GL_FALSE);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    
    // unbind the VAO, the program and the texture
    gLightingShader->stopUsing();
    
    // create model for the second lamb cube
    model = glm::mat4();
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.5f));
    
    // using lambd shader
    gLambShader->use();
    gLambShader->setUniform("model", model, GL_FALSE);
    gLambShader->setUniform("view", gView, GL_FALSE);
    gLambShader ->setUniform("projection", gProjection, GL_FALSE);
    glBindVertexArray(gVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    
    gLambShader->stopUsing();
    
    // swap the display buffers (displays what was just drawn)
    glfwSwapBuffers(gWindow);
}

void OnError(int errorCode, const char* msg) {
    throw std::runtime_error(msg);
}

// the program starts here
void AppMain() {
    // initialise GLFW
    glfwSetErrorCallback(OnError);
    if(!glfwInit())
        throw std::runtime_error("glfwInit failed");
    
    // open a window with GLFW
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    gWindow = glfwCreateWindow((int)SCREEN_SIZE.x, (int)SCREEN_SIZE.y, "OpenGL Tutorial", NULL, NULL);
    if(!gWindow)
        throw std::runtime_error("glfwCreateWindow failed. Can your hardware handle OpenGL 3.2?");

    // GLFW settings
    glfwMakeContextCurrent(gWindow);
    
    // Register callback function for keyboard
    glfwSetKeyCallback(gWindow, key_callback);
    
    // initialise GLEW
    glewExperimental = GL_TRUE; //stops glew crashing on OSX :-/
    if(glewInit() != GLEW_OK)
        throw std::runtime_error("glewInit failed");

    // print out some info about the graphics drivers
    std::cout << "OpenGL version:   " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version:     " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor:           " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer:         " << glGetString(GL_RENDERER) << std::endl;

    // make sure OpenGL version 3.2 API is available
    if(!GLEW_VERSION_3_2)
        throw std::runtime_error("OpenGL 3.2 API is not available.");

    // OpenGL settings
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // load vertex and fragment shaders into opengl
    LoadShaders();

    // load the texture
    LoadTexture("container.jpg");

    // create buffer and fill it with the points of the triangle
    LoadRectangle();

    // run while the window is open
    while(!glfwWindowShouldClose(gWindow)){
        // process pending events
        glfwPollEvents();
        
        do_movement();
        Render();
    }

    // clean up and exit
    glfwTerminate();
}

int main(int argc, char *argv[]) {
    try {
        AppMain();

        
    } catch (const std::exception& e){
        std::cerr << "ERROR: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }
}

void do_movement() {

    // Camera controls
    GLfloat cameraSpeed = 0.05f;
    if (keys[GLFW_KEY_W])
        gCameraPos += cameraSpeed * gCameraFront;
    if (keys[GLFW_KEY_S])
        gCameraPos -= cameraSpeed * gCameraFront;
    if (keys[GLFW_KEY_A])
        gCameraPos -= glm::normalize(glm::cross(gCameraFront, gCameraUp)) * cameraSpeed;
    if (keys[GLFW_KEY_D])
        gCameraPos += glm::normalize(glm::cross(gCameraFront, gCameraUp)) * cameraSpeed;

    
}
