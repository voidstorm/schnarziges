#pragma once
#include <iostream>
#include <functional>
//Schnarziges testpad

//closures

auto make_closure(int init) {
   return [init](int p0) {
      return [p0, init](int p1) {
         return init + p0 + p1;
      };
   };
}


void test_closure() {

   auto c = make_closure(10);

   std::cout << c(10)(2) << std::endl;


}