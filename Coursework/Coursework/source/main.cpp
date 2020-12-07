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
#include "texture.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void mouseKey(GLFWwindow* window, int button, int action, int mode);
void collision(std::vector<Objects>& scene_objects, std::vector<Rockets>& rockets);
Texture2D loadTextureFromFile(const char* file, bool alpha);

const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(0.0f, 10.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;


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
        // координаты         
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

    Shader ringShader("shaders/ring/ring.vs", "shaders/ring/ring.fs");
    Model cubeModel("models/test/test.obj");

    Shader planeShader("shaders/ring/ring.vs", "shaders/ring/ring.fs");
    Model planeModel("models/plane/plane.obj");


    Shader helShader("shaders/ring/ring.vs", "shaders/ring/ring.fs");
    Model helModel("models/helicopter/hel.obj");

    Shader rocketShader("shaders/ring/ring.vs", "shaders/ring/ring.fs");
    Model rocketModel("models/rocket/rocket.obj");

    Objects plane(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(100.0f, 0.0f, 100.0f),
        glm::vec3(-100.0f, 0.0f, -100.0f),
        planeModel, planeShader, false
    );

    std::vector<glm::vec3> target_positions{
        glm::vec3(0.0f, 5.0f, 0.0f),
        glm::vec3(1.0f, 5.0f, 1.0f),
        glm::vec3(2.0f, 5.0f, 2.0f),
        glm::vec3(3.0f, 5.0f, 3.0f),
        glm::vec3(4.0f, 5.0f, 4.0f),
        glm::vec3(5.0f, 5.0f, 5.0f)
    };

    Objects cube(
        glm::vec3(10.0f, 5.0f, 10.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f),
        cubeModel, ringShader, true
    );

    Player player(
        glm::vec3(0.5f, 0.5f, 0.5f),
        glm::vec3(-0.5f, -0.5f, -0.5f),
        helModel, helShader
    );

    std::vector<Objects> scene_objects;
    scene_objects.push_back(plane);
    for (const auto& pos : target_positions) {
        Objects cube(
            pos,
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            cubeModel, ringShader, true
        );

        scene_objects.push_back(cube);
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
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);


        for (const auto& object : scene_objects) {
            object.Draw(view, projection);
        }
        
        for (auto& rocket : rockets) {
            rocket.Draw(view, projection, rocketModel, rocketShader, deltaTime);
        }

        camera.ProcessKeyboard(FORWARD, deltaTime);
        player.Draw(view, projection, camera);

        collision(scene_objects, rockets);
        player_collision(scene_objects, player);
        if (player.lives < 0) {
            glfwSetWindowShouldClose(window, true);
            std::cout << "Game Over!" << std::endl;
        }

        if (scene_objects.size() == 1) {
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

   /* if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);*/
    /*if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);*/
}

// glfw: вс€кий раз, когда измен€ютс€ размеры окна (пользователем или опер. системой), вызываетс€ данна€ функци€
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // убеждаемс€, что вьюпорт соответствует новым размерам окна; обратите внимание,
    // что ширина и высота будут значительно больше, чем указано на retina -диспле€х.
    glViewport(0, 0, width, height);
}

// glfw: вс€кий раз, когда перемещаетс€ мышь, вызываетс€ данна€ callback-функци€
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // перевернуто, так как Y-координаты идут снизу вверх

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: вс€кий раз, когда прокручиваетс€ колесико мыши, вызываетс€ данна€ callback-функци€
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void mouseKey(GLFWwindow* window, int button, int action, int mode)
{
    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
        glm::vec3 rocket_pos = camera.Position + (camera.Front * glm::vec3(2.0f, 2.0f, 2.0f));
        Rockets rocket(
            rocket_pos,
            glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3(-1.0f, -1.0f, -1.0f),
            camera.Front,
            camera.Yaw,
            camera.Pitch
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

Texture2D loadTextureFromFile(const char* file, bool alpha)
{
    // создаем объект текстуры
    Texture2D texture;
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }
    // загружаем изображение
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);
    // теперь генерируем текстуру
    texture.Generate(width, height, data);
    // и в конце освобождаем ресурсы
    stbi_image_free(data);
    return texture;
}