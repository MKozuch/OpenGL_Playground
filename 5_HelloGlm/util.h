#pragma once

#include <functional>

#define _USE_MATH_DEFINES
#include <math.h>

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

template <typename T>
inline float toRadians(const T deg) {
   return float(deg) * M_PI / 180;
}