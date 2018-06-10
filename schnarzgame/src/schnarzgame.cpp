// schnarzgame.cpp : Defines the entry point for the console application.
//

#include "targetver.h"
#include <iostream>


#include "sm/gfx/app.h"
#include "sm/scoped_timer.hpp"
#include "sm/command_queue.hpp"
#include "sm/thread_context.hpp"





//member setters
//TODO: Some sort of dirty mask and copy mechanism for members
//Group similar together
//Uses sizes that are cache aligned

class TestClass final {
public:
   TestClass() = default;
   ~TestClass() = default;

   DECLARE_PRIVATE_GETTER()
   DECLARE_PRIVATE_SETTER()
   #define PRIVATES(MEMBER) \
        MEMBER(count,int,1), \
        MEMBER(use_stuff,bool,true), \
        MEMBER(name,std::string,"bla")
   DECLARE_PRIVATE_MEMBERS(PRIVATES)
};


using MyClass = std::tuple<int, std::string, double>;


int main(int argc, char** argv) {

   TestClass t;
   t.set<TestClass::name>("huhu");
   std::cout << t.get<TestClass::name>() << std::endl;



   MyClass tup{ 1, "bla", 1.34 };

   auto[a, b, c] = tup;

   auto print_elem = sm::fn::overload_unref{
      [](int i) { std::cout << "int: " << i << '\n'; },
      [](std::string& s) { std::cout << "string: " << s << '\n'; s = "helo"; },
      [](double d) { std::cout << "double: " << d << '\n'; }
   };

   for (const auto & elem : sm::fn::to_range(tup)) {
      std::visit(print_elem, elem);
   }

   std::visit(print_elem, sm::fn::to_range(tup)[1]);


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
