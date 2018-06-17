#include "./sm/gfx/app.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include "sm/system_logger.hpp"

using namespace sm::fn;

namespace detail {
INLINE void attach_callbacks(GLFWwindow* window, sm::app::AppCallbackInfo callbacks) {
   static const sm::app::AppCallbackInfo cbi{ std::move(callbacks) };
   //set callbacks
   //keyboard

   if (cbi.onKey) {
      glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)->void {
         cbi.onKey(key, scancode, action, mods);
      });
   }

   //mouse
   if (cbi.onMouseMove) {
      glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)->void {
         cbi.onMouseMove(xpos, ypos);

      });
   }

   if (cbi.onMouseButton) {
      glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)->void {
         cbi.onMouseButton(button, action, mods);
      });
   }

   if (cbi.onScrollWheel) {
      glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)->void {
         cbi.onScrollWheel(xoffset, yoffset);
      });
   }

   //window handling
   if (cbi.onSize) {
      glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)->void {
         cbi.onSize(width, height);
      });
   }


   if (cbi.onClose) {
      glfwSetWindowCloseCallback(window, [](GLFWwindow* window)->void {
         cbi.onClose();
      });
   }
}
}

std::function<bool(void)> sm::app::make_app(const AppCreateInfo &app_info, AppCallbackInfo callbacks, AppMainCallbacks appmain) {
   static auto init = [=]()->bool {
      SYSTEM_LOG_INFO("-------------------------------------------------------");
      SYSTEM_LOG_INFO("Schnarzmaschine, version %s", SCHNARZ_VERSION);
      SYSTEM_LOG_INFO("-------------------------------------------------------");
      SYSTEM_LOG_INFO("Booting...");

      return glfwInit() == GLFW_TRUE;
   };
   //init glfw
   const static bool run_once = init();

   const AppMainCallbacks amc{ std::move(appmain) };
   //create window
   glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
   auto window = glfwCreateWindow(app_info.width, app_info.height, app_info.title.c_str(), nullptr, nullptr);
   //attach callbacks
   detail::attach_callbacks(window, callbacks);

   //returns main loop function
   auto lastTime = std::chrono::high_resolution_clock::now();
   auto now = std::chrono::high_resolution_clock::now();
   return [=]() mutable -> bool {
      //start application main loop
      if (amc.init) {
         amc.init();
      }
      while (!glfwWindowShouldClose(window)) {
         /* Poll for and process events */
         glfwPollEvents();
         amc.main(now - lastTime);
         lastTime = now;
         now = std::chrono::high_resolution_clock::now();
      }
      if (amc.close) {
         amc.close();
      }
      glfwTerminate();
      return true;
   };
}
