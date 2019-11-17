#pragma once

#include <functional>

#ifdef GLFW_TRUE

using window_ptr = std::unique_ptr<GLFWwindow, std::function<void(GLFWwindow*)>>;
template<typename ...Ts>
window_ptr makeWindow_glfw(Ts... args) {
   return window_ptr(glfwCreateWindow(std::forward<Ts>(args)...), glfwDestroyWindow);
}

#endif

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
