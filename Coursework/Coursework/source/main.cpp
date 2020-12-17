#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "functions.h"
#include "player.h"
#include "rockets.h"
#include "objects.h"
//#include "evil.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void mouseKey(GLFWwindow* window, int button, int action, int mode);
void collision(std::vector<Objects>& scene_objects, std::vector<Rockets>& rockets);

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(0.0f, 10.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float horizontal_angle = 0.0f;
float vertical_angle = 0.0f;
float player_speed = 0.0f;


std::vector<Rockets> rockets;


int main() {
    //system("echo %cd%");
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CourseWork", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouseKey);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    Shader skyboxShader("shaders/skybox/skybox.vs", "shaders/skybox/skybox.fs");
    std::vector<std::string> faces
    {
        "textures/skybox/right.jpg",
        "textures/skybox/left.jpg",
        "textures/skybox/top.jpg",
        "textures/skybox/bottom.jpg",
        "textures/skybox/front.jpg",
        "textures/skybox/back.jpg"
    };

    unsigned int cubemapTexture = loadCubemap(faces);

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    Shader evilShader("shaders/ring/ring.vs", "shaders/ring/ring.fs");
    Model evilModel("models/evil/evil.obj");

    /*Shader planeShader("shaders/ring/ring.vs", "shaders/ring/ring.fs");
    Model planeModel("models/plane/plane.obj");*/


    Shader playerShader("shaders/ring/ring.vs", "shaders/ring/ring.fs");
    Model playerModel("models/player/player.obj");

    Shader rocketShader("shaders/ring/ring.vs", "shaders/ring/ring.fs");
    Model rocketModel("models/laser/laser.obj");

    /*Objects plane(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(100.0f, 0.0f, 100.0f),
        glm::vec3(-100.0f, 0.0f, -100.0f),
        planeModel, planeShader, false
    );*/

    std::vector<glm::vec3> target_positions{
        glm::vec3(0.0f, 5.0f, 0.0f),
        glm::vec3(10.0f, 10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, -10.0f),
        glm::vec3(10.0f, 10.0f, -10.0f),
        glm::vec3(-10.0f, 10.0f, 10.0f),
        glm::vec3(-10.0f, -10.0f, 10.0f)
    };

    /*Objects cube(
        glm::vec3(10.0f, 5.0f, 10.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f),
        evilModel, evilShader, true
    );*/

    Player player(
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(-0.5f, -0.5f, -0.5f),
        playerModel, playerShader
    );

    std::vector<Objects> scene_evil;
    //scene_objects.push_back(plane);
    for (const auto& pos : target_positions) {
        Objects evil(
            pos,
            glm::vec3(0.146f, 0.146f, 0.146f),
            glm::vec3(-0.146f, -0.146f, -0.146f),
            evilModel, evilShader, true
        );
        scene_evil.push_back(evil);
    }

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 model = glm::mat4(1.0f);


        for (auto& object : scene_evil) {
            object.Draw(view, projection, deltaTime);
        }
        
        for (auto& rocket : rockets) {
            rocket.Draw(view, projection, rocketModel, rocketShader, deltaTime);
        }

        camera.ProcessKeyboard(FORWARD, deltaTime, player_speed);
        player.Draw(view, projection, camera, horizontal_angle, vertical_angle);
        horizontal_angle = 0.0f;
        vertical_angle = 0.0f;
        player_speed = 1.0f;


        collision(scene_evil, rockets);
        player_collision(scene_evil, player);
        
        if (player.lives < 0) {
            glfwSetWindowShouldClose(window, true);
            std::cout << "Game Over!" << std::endl;
        }

        if (scene_evil.size() == 0) {
            glfwSetWindowShouldClose(window, true);
            std::cout << "You Won!" << std::endl;
        }

        glDepthFunc(GL_LEQUAL);  
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVAO);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        player_speed += 2;
    }

    float xoffset = 0;
    float yoffset = 0;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        vertical_angle = 20;
        yoffset = 0.75;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        vertical_angle = -20;
        yoffset = -0.75;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        horizontal_angle = -20;
        xoffset = -0.75;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        horizontal_angle = 20;
        xoffset = 0.75;
    }
    camera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 

    //if (yoffset > 0)
    //    vertical_angle = 20;
    //else if (yoffset < 0)
    //    vertical_angle = -20;
    //else
    //    vertical_angle = 0;

    //if (xoffset > 0)
    //    horizontal_angle = 20;
    //else if (xoffset < 0)
    //    horizontal_angle = -20;
    //else
    //    horizontal_angle = 0;

    lastX = xpos;
    lastY = ypos;

    //camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void mouseKey(GLFWwindow* window, int button, int action, int mode)
{
    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
        glm::vec3 rocket_pos = camera.Position + (camera.Front * glm::vec3(4.0f, 4.0f, 4.0f));
        Rockets rocket(
            rocket_pos,
            glm::vec3(0.36f, 0.36f, 0.36f),
            glm::vec3(-0.36f, -0.36f, -0.36f),
            camera.Front,
            camera.Yaw,
            camera.Pitch,
            player_speed * 15
        );
        rockets.push_back(rocket);
    }
}

void collision(std::vector<Objects>& scene_objects, std::vector<Rockets>& rockets) {
    for (auto it_obj = scene_objects.begin(); it_obj != scene_objects.end();) {
        bool update = true;
        for (auto it_rock = rockets.begin(); it_rock != rockets.end(); ++it_rock) {
            if (it_rock->is_collision(*it_obj)) {
                if (it_obj->is_destroyed) {
                    it_obj = scene_objects.erase(it_obj);
                    update = false;
                }
                it_rock = rockets.erase(it_rock);
                break;
            }
        }
        if (update) {
            ++it_obj;
        }
    }
}