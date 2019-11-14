#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <string>

const GLint WIDTH = 600;
const GLint HEIGHT = 600;

using window_ptr = std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>>;
template<typename ...Ts>
window_ptr makeWindow(Ts... args) {
   return window_ptr(glfwCreateWindow(std::forward<Ts>(args)...), glfwDestroyWindow);
}

struct [[nodiscard]] ContextGuard{
   ContextGuard(
      std::function<int(void)> initContext
      , std::function<void(void)> terminateContext)
      : mInitContextFun{ initContext }
      , mTerminateContextFun{ terminateContext } {
      mInitStatus = mInitContextFun();
   }

   ~ContextGuard() {
      mTerminateContextFun();
   }

   int getInitStatus() {
      return mInitStatus;
   }

private: 
   int mInitStatus;
   std::function<int(void)> mInitContextFun;
   std::function<void(void)> mTerminateContextFun;
};


GLuint VAO, VBO, shaderId;

void createTriangle() {
   GLfloat vertices[] = {
      -1.0, -1.0, 0.0,
       1.0, -1.0, 0.0,
       0.0,  1.0, 0.0
   };

   glGenVertexArrays(1, &VAO);
   glBindVertexArray(VAO);

      glGenBuffers(1, &VBO);
      glBindBuffer(GL_ARRAY_BUFFER, VBO); 
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
      glEnableVertexAttribArray(0);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
}


GLuint createSquare() {
   GLuint vao, vbo;

   GLfloat vertices[] = {
      -1.0, -1.0, 0.0,
      -1.0,  1.0, 0.0,

      -1.0,  1.0, 0.0,
       1.0,  1.0, 0.0,

       1.0,  1.0, 0.0,
       1.0, -1.0, 0.0,

       1.0, -1.0, 0.0,
      -1.0, -1.0, 0.0
   };

   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

   glGenBuffers(1, &vbo);
   glBindBuffer(GL_ARRAY_BUFFER, vbo);
   glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   glVertexAttribPointer(0, 3, GL_FLOAT, GL_TRUE, 0, 0);
   glEnableVertexAttribArray(0);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);

   return vao;
}

//vertex shader
static const char* vShader =
R"(

#version 330
layout (location = 0) in vec3 pos;

void main(){
    gl_Position = vec4(0.5*pos.x, 0.5*pos.y, 0.5*pos.z, 1.0);	
}

)";

//fragment shader
static const char* fShader =
R"(

#version 330
out vec4 color;

void main(){
   color = vec4(1.0, 0.0, 0.0, 0.5);
}

)";

bool addShader(GLuint program, const char* shaderCode, GLenum shaderType) {
   GLuint shader = glCreateShader(shaderType);

   const GLchar* code[1];
   code[0] = shaderCode;

   GLint codeLength[1];
   codeLength[0] = strlen(shaderCode);

   glShaderSource(shader, 1, code, codeLength);
   glCompileShader(shader);

   GLint result = 0;
   GLchar log[1024] = "";

   glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
   if (!result) {
      glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
      printf("Error compiling shader: '%s'\n", log);
      return false;
   }

   glAttachShader(program, shader);
   return true;
}

bool createShaders(){
   shaderId = glCreateProgram();
   if (!shaderId) {
      printf("Error creating shader program");
      return false;
   }

   auto ret = true;
   ret &= addShader(shaderId, vShader, GL_VERTEX_SHADER);
   ret &= addShader(shaderId, fShader, GL_FRAGMENT_SHADER);

   GLint result = 0;
   GLchar log[1024] = "";

   glLinkProgram(shaderId);
   glGetProgramiv(shaderId, GL_LINK_STATUS, &result);
   if (!result) {
      glGetProgramInfoLog(shaderId, sizeof(log), nullptr, log);
      printf("Error linking program: '%s'\n", log);
      ret = false;
   }

   glValidateProgram(shaderId);
   glLinkProgram(shaderId);
   glGetProgramiv(shaderId, GL_VALIDATE_STATUS, &result);
   if (!result) {
      glGetProgramInfoLog(shaderId, sizeof(log), nullptr, log);
      printf("Error validating program: '%s'\n", log);
      ret = false;
   }

   return ret;
}


int main() {
   ContextGuard glfwContext(glfwInit, glfwTerminate);
   const auto ret = glfwContext.getInitStatus();
   if (ret != GLFW_TRUE)
      return ret;

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

   auto mainWindow = makeWindow(WIDTH, HEIGHT, "MainWindow", nullptr, nullptr);

   glfwMakeContextCurrent(mainWindow.get());
   glewExperimental = GL_TRUE;
   if (const auto ret = glewInit(); ret != GLEW_OK)
      return ret;

   int bufferWidth, bufferHeight;
   glfwGetFramebufferSize(mainWindow.get(), &bufferWidth, &bufferHeight);
   glViewport(0, 0, bufferHeight, bufferWidth);

   createTriangle();
   createShaders();

   for (unsigned long i = 0; !glfwWindowShouldClose(mainWindow.get()); ++i) {
      if(i & 0x80)
         glClearColor(0.0, 1.0, 0.0, 1.0);
      else
         glClearColor(0.0, 0.0, 1.0, 1.0);

      glUseProgram(shaderId);
         glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
         glBindVertexArray(createSquare());
            glDrawArrays(GL_LINES, 0, 8);
         glBindVertexArray(0);

      glUseProgram(0);

      glfwPollEvents();
      glfwSwapBuffers(mainWindow.get());
      glClear(GL_COLOR_BUFFER_BIT);
   }

   return 0;
}