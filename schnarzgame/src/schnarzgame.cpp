// schnarzgame.cpp : Defines the entry point for the console application.
//

#include "targetver.h"
#include <iostream>


#include "sm/gfx/app.h"
#include "sm/game.h"
#include "sm/scoped_timer.hpp"
#include "sm/command_queue.hpp"
#include "sm/thread_context.hpp"

#include "sm/schnarzpad.h"

int main(int argc, char** argv) {
   auto onKey = [=](int key, int scancode, int action, int mods)->void {
      std::cout << "bla" << std::endl;
   };

   //window main loop
   auto mainloop = [=](const std::chrono::high_resolution_clock::duration &delta)->void {
      std::cout << "delta: " << std::chrono::duration_cast<sm::time::fmilliseconds> (delta).count() << std::endl;
   };

   //create and run app
   auto run = make_game(GameCreateInfo{})({ "schnarzmaschine", 640, 480, false }, { onKey }, { mainloop })(sm::gfx::rc::RenderContextCreateInfo{});
   return run();
}
