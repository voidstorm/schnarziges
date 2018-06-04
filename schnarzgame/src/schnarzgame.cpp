// schnarzgame.cpp : Defines the entry point for the console application.
//

#include "targetver.h"
#include <iostream>

#include "sm/gfx/app.h"
#include "sm/scoped_timer.hpp"
#include "sm/command_queue.hpp"


int main(int argc, char** argv) {

   std::vector<sm::thread::QueueTask> tasks;
   sm::thread::CommandQueue q;


   tasks.emplace_back([argc]()->std::any {return (argc); });
   tasks.emplace_back([argc]()->std::any {return (argc); });
   tasks.emplace_back([argc]()->std::any {return (argc); });

   q.submit(tasks);

   auto onKey = [=](int key, int scancode, int action, int mods)->void {
      std::cout << "bla" << std::endl;
   };
   //window main loop
   auto mainloop = [=](const std::chrono::high_resolution_clock::duration &delta)->void {
      std::cout << "delta: " << std::chrono::duration_cast<sm::time::fmilliseconds> (delta).count() << std::endl;
   };
   //create and run app
   auto app = sm::app::make_app({ "schnarzmaschine", 640, 480, false }, { onKey }, { mainloop });
   return app();
}
