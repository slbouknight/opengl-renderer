 #include <glad/glad.h>
 #include <GLFW/glfw3.h>
 #include <stb_image/stb_image.h>

 #include <glm/glm.hpp>
 #include <glm/gtc/matrix_transform.hpp>
 #include <glm/gtc/type_ptr.hpp>
 #include "../include/Camera.h"
 #include "../include/Shader.h"
 #include "../include/LightingManager.h"

 #include <iostream>
 #include <random>

 // Prototypes
 // ---------------------------------------------------------
 void processInput(GLFWwindow* window);
 void framebuffer_size_callback(GLFWwindow* window, int width, int height);
 void mouse_callback(GLFWwindow* window, double xpos, double ypos);
 void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
 void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
 unsigned int loadTexture(const char* path);

 // Configuration Constants & Globals
 // ----------------------------------------------------------
 const unsigned int SCR_WIDTH = 800;
 const unsigned int SCR_HEIGHT = 600;

 // Camera
 Camera camera (glm::vec3(0.0f, 0.0f, 3.0f));
 float lastX = SCR_WIDTH / 2.0f;
 float lastY = SCR_HEIGHT / 2.0f;
 bool firstMouse = true;
 bool rightMouseHeld = false;
 bool middleMouseHeld = false;
 bool resetMousePosition = false;

 // Timing
 float deltaTime = 0.0f;
 float lastFrame = 0.0f;

 int main()
 {
     // GLFW & GL Context Setup
     // -----------------------------
     glfwInit();
     glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
     glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
     glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

     GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Renderer", NULL, NULL);
     if (window == NULL)
     {
         std::cout << "Failed to create GLFW  window" << std::endl;
         glfwTerminate();
         return -1;
     }

     // Callbacks
     glfwMakeContextCurrent(window);
     glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
     glfwSetCursorPosCallback(window, mouse_callback);
     glfwSetMouseButtonCallback(window, mouse_button_callback);
     glfwSetScrollCallback(window, scroll_callback);

     // GLAD
     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
     {
         std::cout << "Failed to initialize GLAD" << std::endl;
         return -1;
     }

     // Global OpenGL state
     glEnable(GL_DEPTH_TEST);

     // Create Lighting Manager and add lights
     // --------------------------------------
     LightingManager lighting;

     // Directional light
     DirectionalLightDesc dirDesc;
     dirDesc.direction = glm::vec3(-0.2f, -1.0f, -0.3f);
     // dirDesc.ambient/diffuse/specular are the defaults from the struct
     lighting.addDirectional(dirDesc);

     // Four point lights at fixed positions:
     std::vector<glm::vec3> pointPositions = {
         { 0.7f,  0.2f,  2.0f},
         { 2.3f, -3.3f, -4.0f},
         {-4.0f,  2.0f, -12.0f},
         { 0.0f,  0.0f,  -3.0f}
     };
     for (auto& pos : pointPositions) {
         PointLightDesc pld;
         pld.position = pos;
         // ambient/diffuse/specular/attenuation default
         lighting.addPoint(pld);
     }

     // (Optional) Spot‐light following the camera:
     SpotLightDesc sld;
     sld.position = camera.Position;
     sld.direction = camera.Front;
     // other fields left at defaults
     lighting.addSpot(sld);

     // Shaders & Textures
     // --------------------------------
     Shader lightingShader("shaders/lit_geometry.vs", "shaders/lit_geometry.fs");
     Shader lightingCubeShader("shaders/light_cube.vs", "shaders/light_cube.fs");
     lightingShader.use();
     lightingShader.setInt("material.diffuse", 0);
     lightingShader.setInt("material.specular", 1);
     unsigned int diffuseMap = loadTexture("resources/textures/container2.png");
     unsigned int specularMap = loadTexture("resources/textures/container2_specular.png");

     // Set up vertex data, buffers, and configure vertex attributes
     // ------------------------------------------------------------------
     float vertices[] = {
         // positions          // normals           // texture coords
         -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
          0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
          0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
          0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
         -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

         -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
          0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
         -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

         -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

          0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
          0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
          0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
          0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
          0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
          0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
          0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
          0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
         -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
          0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
          0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
         -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
     };
    
     // world space positions of our cubes
     glm::vec3 cubePositions[] = {
         glm::vec3(0.0f,  0.0f,  0.0f),
         glm::vec3(2.0f,  5.0f, -15.0f),
         glm::vec3(-1.5f, -2.2f, -2.5f),
         glm::vec3(-3.8f, -2.0f, -12.3f),
         glm::vec3(2.4f, -0.4f, -3.5f),
         glm::vec3(-1.7f,  3.0f, -7.5f),
         glm::vec3(1.3f, -2.0f, -2.5f),
         glm::vec3(1.5f,  2.0f, -2.5f),
         glm::vec3(1.5f,  0.2f, -1.5f),
         glm::vec3(-1.3f,  1.0f, -1.5f)
     };

     // Configure cube objects
     // -------------------------
     unsigned int VBO, cubeVAO;
     glGenVertexArrays(1, &cubeVAO);
     glGenBuffers(1, &VBO);
     // Bind vertex array object first, then bind and set its vertex and element buffer objects
     glBindVertexArray(cubeVAO);
     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
     // Position attribute
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
     glEnableVertexAttribArray(0);
     // Normal attribute
     glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
     glEnableVertexAttribArray(1);
     // Texture coord attribute
     glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
     glEnableVertexAttribArray(2);

     // Configure light objects
     // -------------------------
     unsigned int lightCubeVAO;
     glGenVertexArrays(1, &lightCubeVAO);
     glBindVertexArray(lightCubeVAO);

     glBindBuffer(GL_ARRAY_BUFFER, VBO);
     // note that we update the lamp's position attribute's stride to reflect the updated buffer data
     glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
     glEnableVertexAttribArray(0);

     // Render loop
     // ----------------------------------------------------
     while (!glfwWindowShouldClose(window))
     {
         // Timing & Input
         float currentFrame = (float)glfwGetTime();
         deltaTime = currentFrame - lastFrame;
         lastFrame = currentFrame;
         processInput(window);

         // Clear buffers
         glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
         glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

         // Draw scene geometry with lighting shader
         lightingShader.use();
         lightingShader.setVec3("viewPos", camera.Position);
         lightingShader.setFloat("material.shininess", 32.0f);

         // Spotlight follows camera each frame
         //SpotLightDesc sld;
         //sld.position = camera.Position;
         //sld.direction = camera.Front;
         //// override defaults only; no need to re-add the old one
         //lighting.updateSpot(0, sld);

         // Upload all lights in one shot
         lighting.uploadToShader(lightingShader);

         // Set matrices
         glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
         glm::mat4 view = camera.GetViewMatrix();
         lightingShader.setMat4("projection", projection);
         lightingShader.setMat4("view", view);

         // Bind textures
         glActiveTexture(GL_TEXTURE0);
         glBindTexture(GL_TEXTURE_2D, diffuseMap);
         glActiveTexture(GL_TEXTURE1);
         glBindTexture(GL_TEXTURE_2D, specularMap);

         // Draw containers
         glBindVertexArray(cubeVAO);
         int cubeCount = 0;
         for (auto& pos : cubePositions)
         {
             // calculate the model matrix for each object and pass it to shader before drawing
             glm::mat4 model = glm::mat4(1.0f);
             model = glm::translate(model, pos);
             float angle = 20.0 * cubeCount;
             model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
             lightingShader.setMat4("model", model);
             glDrawArrays(GL_TRIANGLES, 0, 36);
             cubeCount++;
         }

         // Draw light shapes
          lightingCubeShader.use();
          lightingCubeShader.setMat4("projection", projection);
          lightingCubeShader.setMat4("view", view);
          lighting.drawShapes(lightingCubeShader);

         // Swap & Poll
         glfwSwapBuffers(window);
         glfwPollEvents();
     }

     // Cleanup
     // ------------------------------------
     glDeleteVertexArrays(1, &cubeVAO);
     glDeleteVertexArrays(1, &lightCubeVAO);
     glDeleteBuffers(1, &VBO);
     glfwTerminate();
     return 0;
 }

 // Process all input
 // ------------------
 void processInput(GLFWwindow* window)
 {
     if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
         glfwSetWindowShouldClose(window, true);

     // Camera only moves when RMB is held
     if (rightMouseHeld)
     {
         if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
             camera.ProcessKeyboard(FORWARD, deltaTime);
         if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
             camera.ProcessKeyboard(BACKWARD, deltaTime);
         if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
             camera.ProcessKeyboard(LEFT, deltaTime);
         if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
             camera.ProcessKeyboard(RIGHT, deltaTime);
         if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
             camera.ProcessKeyboard(UP, deltaTime);
         if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
             camera.ProcessKeyboard(DOWN, deltaTime);
     }
 }

 // GLFW: Callback executed whenever window is resized
 // --------------------------------------------------
 void framebuffer_size_callback(GLFWwindow* window, int width, int height)
 {
     // Make sure viewport matches new dimensions
     glViewport(0, 0, width, height);
 }

 // GLFW: Whenever the mouse moves, this callback is called
 // -------------------------------------------------------
 void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
 {
     float xpos = static_cast<float>(xposIn);
     float ypos = static_cast<float>(yposIn);

     if (resetMousePosition)
     {
         lastX = xpos;
         lastY = ypos;
         resetMousePosition = false;
         return;  // prevent offset jump
     }

     if (rightMouseHeld || middleMouseHeld)
     {
         if (firstMouse)
         {
             lastX = xpos;
             lastY = ypos;
             firstMouse = false;
             return;
         }

         float xoffset = xpos - lastX;
         float yoffset = lastY - ypos;

         lastX = xpos;
         lastY = ypos;

         if (rightMouseHeld)
             camera.ProcessMouseMovement(xoffset, yoffset);

         if (middleMouseHeld)
         {
             float angleX = xoffset * 0.3f;
             float angleY = yoffset * 0.3f;

             glm::vec3 focusPoint = glm::vec3(0.0f);
             glm::vec3 direction = camera.Position - focusPoint;

             glm::mat4 yawMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-angleX), camera.WorldUp);
             glm::vec3 right = glm::normalize(glm::cross(camera.Front, camera.WorldUp));
             glm::mat4 pitchMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(-angleY), right);

             glm::vec3 rotated = glm::vec3(pitchMatrix * yawMatrix * glm::vec4(direction, 1.0f));
             camera.Position = focusPoint + rotated;
             camera.Front = glm::normalize(focusPoint - camera.Position);
             camera.Yaw = glm::degrees(atan2(camera.Front.z, camera.Front.x)) - 90.0f;
             camera.Pitch = glm::degrees(asin(camera.Front.y));
         }
     }

 }

 // GLFW: Whenever mouse buttons are is held this callback is called
 // --------------------------------------------------
 void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
 {
     // Right Mouse Button Check
     if (button == GLFW_MOUSE_BUTTON_RIGHT  && action == GLFW_PRESS)
     {
         rightMouseHeld = true;
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
         resetMousePosition = true;

         // Reset mouse tracking cleanly
         double xpos, ypos;
         glfwGetCursorPos(window, &xpos, &ypos);
         lastX = static_cast<float>(xpos);
         lastY = static_cast<float>(ypos);
     }
     if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
     {
         rightMouseHeld = false;
         resetMousePosition = true;
         glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
     }

     // Middle Mouse Button Check
     if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
     {
         middleMouseHeld = true;

         // Reset mouse tracking cleanly
         double xpos, ypos;
         glfwGetCursorPos(window, &xpos, &ypos);
         lastX = static_cast<float>(xpos);
         lastY = static_cast<float>(ypos);
     }
     if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE)
     {
         middleMouseHeld = false;
     }
 }

 // GLFW: Whenever the mouse scroll wheel scrolls, this callback is called
 // ----------------------------------------------------------------------
 void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
 {
     if (rightMouseHeld)
     {
         // Adjust movement speed instead of zoom
         camera.MovementSpeed += static_cast<float>(yoffset);
         if (camera.MovementSpeed < 0.1f)
             camera.MovementSpeed = 0.1f;
         if (camera.MovementSpeed > 100.0f)
             camera.MovementSpeed = 100.0f;

         std::cout << "Camera speed: " << camera.MovementSpeed << '\n';
     }
     else
     {
         camera.ProcessMouseScroll(static_cast<float>(yoffset));
     }
 }

 // Utility function for loading a 2D texture from file
 // ---------------------------------------------------
 unsigned int loadTexture(char const* path)
 {
     unsigned int textureID;
     glGenTextures(1, &textureID);

     int width, height, nrComponents;
     unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
     if (data)
     {
         GLenum format = GL_RGB;
         if (nrComponents == 1)
             format = GL_RED;
         else if (nrComponents == 3)
             format = GL_RGB;
         else if (nrComponents == 4)
             format = GL_RGBA;

         glBindTexture(GL_TEXTURE_2D, textureID);
         glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
         glGenerateMipmap(GL_TEXTURE_2D);

         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

         stbi_image_free(data);
     }
     else
     {
         std::cout << "Texture failed to load at path: " << path << std::endl;
         stbi_image_free(data);
     }

     return textureID;
 }