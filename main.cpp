#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "skybox.h"
#include "cube.h"
#include "texture_loader.h"
#include "model.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
  stbi_set_flip_vertically_on_load(true);

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // load skybox
  // -----------
  std::string facePaths[] = 
  {
    "/Users/mashiro_jin/opengl/resources/skybox/right.jpg",
    "/Users/mashiro_jin/opengl/resources/skybox/left.jpg",
    "/Users/mashiro_jin/opengl/resources/skybox/bottom.jpg",
    "/Users/mashiro_jin/opengl/resources/skybox/top.jpg",
    "/Users/mashiro_jin/opengl/resources/skybox/front.jpg",
    "/Users/mashiro_jin/opengl/resources/skybox/back.jpg"
  };
  vector<std::string> faces(facePaths, facePaths + 6);
  unsigned int cubeMapTexture = loadCubemap(faces);

  // Initialize objects
  // ------------------
  Skybox skybox(cubeMapTexture);
  Cube cube(cubeMapTexture, 0.25);
  Model nanosuitModel("/Users/mashiro_jin/opengl/resources/objects/nanosuit/nanosuit.obj");
  Model cyboryModel("/Users/mashiro_jin/opengl/resources/objects/cyborg/cyborg.obj");

  // draw in wireframe
  // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // build and compile shaders
  // -------------------------
  Shader skyboxShader("/Users/mashiro_jin/opengl/shaders/skybox.vs", "/Users/mashiro_jin/opengl/shaders/skybox.fs");
  Shader cubemapShader("/Users/mashiro_jin/opengl/shaders/cube.vs", "/Users/mashiro_jin/opengl/shaders/cube_reflect.fs");
  Shader nanosuitShader("/Users/mashiro_jin/opengl/shaders/nanosuit.vs", "/Users/mashiro_jin/opengl/shaders/nanosuit.fs");
  Shader cyborgShader("/Users/mashiro_jin/opengl/shaders/cyborg.vs", "/Users/mashiro_jin/opengl/shaders/cyborg.fs");

  // shader configuration
  skyboxShader.use();
  skyboxShader.setInt("skybox", 0);

  cubemapShader.use();
  cubemapShader.setInt("skybox", 0);

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window))
  {
    // per-frame time logic
    // --------------------
    float currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // input
    // -----
    processInput(window);

    // render
    // ------
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Specular Cube Render
    // -----------
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 1.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.5));
    model = glm::translate(model, glm::vec3(2.0f, -1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    cubemapShader.use();
    cubemapShader.setMat4("model", model);
    cubemapShader.setMat4("projection", projection);
    cubemapShader.setMat4("view", view);
    cubemapShader.setVec3("cameraPos", camera.Position);
    cube.Draw(cubemapShader);

    // Model nanosuit Render
    // ---------------------
    nanosuitShader.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.1f));     // it's a bit too big for our scene, so scale it down
    model = glm::rotate(model, currentFrame * glm::radians(5.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    nanosuitShader.setMat4("model", model);
    nanosuitShader.setMat4("projection", projection);
    nanosuitShader.setMat4("view", view);
    // direct light
    nanosuitShader.setVec3("lightDir", 0.0f, -0.5f, -1.0f);
    nanosuitShader.setVec3("dirLight.ambient",  glm::vec3(1.0f, 1.0f, 1.0f));
    nanosuitShader.setVec3("dirLight.diffuse",  glm::vec3(1.0f, 1.0f, 1.0f));
    nanosuitShader.setVec3("dirLight.specular",  glm::vec3(1.0f, 1.0f, 1.0f));
    nanosuitModel.Draw(nanosuitShader);

    // Model cyborg Render
    // -------------------
    cyborgShader.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f)); // translate it down so it's at the center of the scene
    model = glm::scale(model, glm::vec3(0.4f));
    cyborgShader.setMat4("model", model);
    cyborgShader.setMat4("projection", projection);
    cyborgShader.setMat4("view", view);
    cyborgShader.setVec3("viewPos", camera.Position);
    cyborgShader.setInt("cubemap", 0);
    cyboryModel.Draw(cyborgShader);

    // Sky box render
    glDepthFunc(GL_LEQUAL);
    skyboxShader.use();
    view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
    skyboxShader.setMat4("projection", projection);
    skyboxShader.setMat4("view", view);
    // Draw skybox
    skybox.Draw(skyboxShader);
    glDepthFunc(GL_LESS);

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
{
  float xpos = static_cast<float>(xposIn);
  float ypos = static_cast<float>(yposIn);

  if (firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
