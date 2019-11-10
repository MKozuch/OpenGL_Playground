#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>
#include <memory>
#include <functional>

const GLint WIDTH = 800;
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

   for (unsigned long i = 0; !glfwWindowShouldClose(mainWindow.get()); ++i) {
      if(i & 0x80)
         glClearColor(0.0, 1.0, 0.0, 1.0);
      else
         glClearColor(0.0, 0.0, 1.0, 1.0);

      glfwPollEvents();
      glfwSwapBuffers(mainWindow.get());
      glClear(GL_COLOR_BUFFER_BIT);
   }

   return 0;
}