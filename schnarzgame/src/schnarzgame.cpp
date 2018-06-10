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

   SM_DECLARE_PRIVATE_GETTER()
   SM_DECLARE_PRIVATE_SETTER()
   SM_DECLARE_PRIVATE_MEMBER_ITER()
   #define PRIVATES(MEMBER) \
        MEMBER(m_count,int,1), \
        MEMBER(m_use_stuff,const bool,true), \
        MEMBER(m_name,std::string,"bla")
   SM_DECLARE_PRIVATE_MEMBERS_WITHFLAG(PRIVATES)
};


using MyClass = std::tuple<std::tuple<bool, int>, std::tuple<bool, std::string>, std::tuple<bool, double>>;


int main(int argc, char** argv) {

   TestClass t;
   //t.set<TestClass::m_name>("huhu");

   //std::cout << t.get<TestClass::m_name>() << std::endl;

   auto[a, b] = t.get<TestClass::m_name>();

   MyClass tup{ std::make_tuple(false,1), std::make_tuple(false,"bla"), std::make_tuple(false,1.34) };


   auto print_elem = sm::fn::overload_unref{
      [](int i) { std::cout << "int: " << i << '\n'; },
      [](std::string& s) { std::cout << "string: " << s << '\n'; s = "helo"; },
      [](double d) { std::cout << "double: " << d << '\n'; },
      [](bool b) { std::cout << "bool: " << b << '\n'; }
   };

   
   //for (const auto &  [dirty, elem] : t) {
   //   std::visit(print_elem, elem);
   //}

   //for (const auto [dirty, elem] : sm::fn::to_range(tup)) {
   //   std::visit(print_elem, elem);
   //}

   //std::visit(print_elem, sm::fn::to_range(tup)[1]);


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
