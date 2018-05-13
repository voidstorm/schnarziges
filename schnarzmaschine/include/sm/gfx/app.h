#pragma once
#include "./sm/api.h"
#include "./sm/functional.hpp"
#include <string>
#include <future>
#include <chrono>
#include <thread>

namespace sm::gfx::app {

struct AppCreateInfo {
   const std::string title = "app";
   const int width = 640;
   const int height = 480;
   const bool fullscreen = false;
};

struct AppCallbackInfo {
   const std::function<void(int key, int scancode, int action, int mods)> onKey;
   const std::function<void(double xpos, double ypos)> onMouseMove;
   const std::function<void(int button, int action, int mods)> onMouseButton;
   const std::function<void(double xoffset, double yoffset)> onScrollWheel;
   const std::function<void(int width, int height)> onSize;
   const std::function<void()> onClose;
};

struct AppMainCallbacks {
   const std::function<void(const std::chrono::high_resolution_clock::duration &delta)> main;
   const std::function<void()> init;
   const std::function<void()> close;
};

SCHNARZ_API std::function<bool(void)> make_app(const AppCreateInfo &app_info, AppCallbackInfo callbacks, AppMainCallbacks appmain);
}