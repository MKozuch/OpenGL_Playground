#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <string>
#include <cmath>
#include <optional>
#include <cstdlib>
#include <time.h>

#include "util.h"
#include "shaders.h"

const GLint WIDTH = 800;
const GLint HEIGHT = 800;

GLuint uniformXMove;
GLuint uniformYMove;
int directionX = 1;
int directionY = 1;
auto offsetX = 0.0f;
auto offsetY = 0.0f;
auto offsetMax = 0.5f;
auto offsetIncrement = 5.0e-3f;

GLuint createTriangle() {
   GLuint vao = 0;
   GLuint vbo = 0;

   GLfloat vertices[] = {
      -1.0, -1.0, 0.0,
       1.0, -1.0, 0.0,
       0.0,  1.0, 0.0
   };

   glGenVertexArrays(1, &vao);
   glBindVertexArray(vao);

      glGenBuffers(1, &vbo);
      glBindBuffer(GL_ARRAY_BUFFER, vbo); 
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
      glEnableVertexAttribArray(0);

   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);

   return vao;
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

std::optional<GLuint> compileShaders(){
   auto shaderId = glCreateProgram();
   if (!shaderId) {
      printf("Error creating shader program");
      return {};
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

   uniformXMove = glGetUniformLocation(shaderId, "xMove");
   uniformYMove = glGetUniformLocation(shaderId, "yMove");

   if (ret)
      return shaderId;
   else {
      glDeleteShader(shaderId);
      return {};
   }
}


int main() {
   std::srand(time(nullptr));
   offsetX = float(rand() % 100) / 100 - 0.5;
   offsetY = float(rand() % 100) / 100 - 0.5;

   //init glew
   ContextGuard glfwContext(glfwInit, glfwTerminate);
   const auto ret = glfwContext.getInitStatus();
   if (ret != GLFW_TRUE)
      return ret;

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

   //make glfw window
   auto mainWindow = makeWindow_glfw(WIDTH, HEIGHT, "MainWindow", nullptr, nullptr);

   // create gl context
   glfwMakeContextCurrent(mainWindow.get());

   // intit glew
   glewExperimental = GL_TRUE;
   if (const auto ret = glewInit(); ret != GLEW_OK)
      return ret;

   int bufferWidth, bufferHeight;
   glfwGetFramebufferSize(mainWindow.get(), &bufferWidth, &bufferHeight);
   glViewport(0, 0, bufferHeight, bufferWidth);

   const auto triangleVao = createTriangle();
   const auto squareVao = createSquare();   

   GLuint shaderId = 0;
   if (auto ret = compileShaders(); !ret.has_value())
      return(-1);
   else
      shaderId = ret.value();

   for (unsigned long i = 0; !glfwWindowShouldClose(mainWindow.get()); ++i) {
      if(i & 0x80)
         glClearColor(0.0, 1.0, 0.0, 1.0);
      else
         glClearColor(0.0, 0.0, 1.0, 1.0);

      offsetX += directionX * offsetIncrement;
      if(abs(offsetX) >= offsetMax)
         directionX *= -1;
      offsetY += directionY * offsetIncrement;
      if(abs(offsetY) >= offsetMax)
         directionY *= -1;

      glUseProgram(shaderId);
         glUniform1f(uniformXMove, offsetX);
         glUniform1f(uniformYMove, offsetY);
         glBindVertexArray(triangleVao);
            glDrawArrays(GL_TRIANGLES, 0, 3);
         glBindVertexArray(squareVao);
            glDrawArrays(GL_LINES, 0, 8);
         glBindVertexArray(0);

      glUseProgram(0);

      glfwPollEvents();
      glfwSwapBuffers(mainWindow.get());
      glClear(GL_COLOR_BUFFER_BIT);
   }

   return 0;
}