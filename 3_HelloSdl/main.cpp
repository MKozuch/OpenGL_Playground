#include <stdio.h>
#include <GL/glew.h>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>
#include <string>

#include <SDL.h>
#include <SDL_main.h>

#include <atomic>
#include <random>

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

struct Color {
   float R = 0;
   float G = 0;
   float B = 0;
   float alpha = 1;
};

const Color lawnGreen{  124.0/255, 252.0/255,   0.0/255, 1 };
const Color chartReuse{ 127.0/255, 255.0/255,   0.0/255, 1 };
const Color maroon{     128.0/255,   0.0/255,   0.0/255, 1 };
std::atomic<Color> currentColor = lawnGreen;

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

bool compileShaders(){
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

void sdlDie(const char* msg)
{
   printf("%s: %s\n", msg, SDL_GetError());
   SDL_Quit();
   exit(1);
}

void checkSDLError(int line = -1)
{
#ifndef NDEBUG 
   const char* error = SDL_GetError();
   if (*error != '\0')   {
      printf("SDL Error: %s\n", error);
      if (line != -1)
         printf(" + line: %i\n", line);
      SDL_ClearError();
   }
#endif
}

int main(int argc, char* argv[]) {
   SDL_Window* mainWindow;
   SDL_GLContext glContext;

   if (SDL_Init(SDL_INIT_VIDEO) < 0) 
      sdlDie("Unable to initialize SDL");

   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

   //create timer
   srand(2137);
   unsigned int delay = 100;
   auto callbackFun = [](unsigned int interval, void* param)->unsigned int {
      printf("Kek xDD\n");
      static int cnt = 0;
      cnt++;

      auto col = currentColor.load();
      float r = 0;
      r = (float(rand()) / RAND_MAX) * 0.01;
      col.R = abs(fmod(col.R + r, 1));
      col.G = abs(fmod(col.G - r, 1));
      col.B = abs(fmod(col.B + r, 1));
      currentColor.store(col);
      //if (cnt % 2)
      //   currentColor.store(maroon);
      //else
      //   currentColor.store(chartReuse);

      return *static_cast<int*>(param);
   };
   auto timerId = SDL_AddTimer(delay, callbackFun, &delay);

   mainWindow = SDL_CreateWindow(
      "SDL2/OpenGL Demo",
      SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480,
      SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

   if (!mainWindow)
      sdlDie("Unable to create window");

   checkSDLError(__LINE__);

   glContext = SDL_GL_CreateContext(mainWindow);
   checkSDLError(__LINE__);

   SDL_GL_SetSwapInterval(1);

   //init glew
   glewExperimental = GL_TRUE;
   if (const auto ret = glewInit(); ret != GLEW_OK)
      return ret;

   createTriangle();
   compileShaders();

   bool shouldQuit = false;
   SDL_Event e;
   while (!shouldQuit) {
      while (SDL_PollEvent(&e)) {
         if (e.type == SDL_QUIT)
            shouldQuit = true;
      }

      const auto col = currentColor.load();

      glClearColor(col.R, col.G, col.B, col.alpha);
      glUseProgram(shaderId);
         glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(createSquare());
            glDrawArrays(GL_LINES, 0, 8);
         glBindVertexArray(0);
      glUseProgram(0);

      SDL_GL_SwapWindow(mainWindow);
      glClear(GL_COLOR_BUFFER_BIT);
   }

   SDL_GL_DeleteContext(glContext);
   SDL_DestroyWindow(mainWindow);
   SDL_Quit();

   return 0;
}