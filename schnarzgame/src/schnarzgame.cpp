// schnarzgame.cpp : Defines the entry point for the console application.
//

#include "targetver.h"
#include <iostream>

#include "sm/gfx/app.h"


int main(int argc, char** argv) {
   
   auto onKey = [=](int key, int scancode, int action, int mods)->void {
      std::cout << "bla" << std::endl;
   };

   //window main loop
   auto mainloop = [=](const std::chrono::high_resolution_clock::duration &delta)->void {
      std::cout << "delta: " << std::chrono::duration_cast<std::chrono::microseconds>(delta).count() << std::endl;
   };


   auto app = sm::gfx::app::make_app({ "schnarzmaschine", 640, 480, false }, { onKey }, { mainloop });
   return app();
}
