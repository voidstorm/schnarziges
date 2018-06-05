#pragma once
#include <functional>
#include <algorithm>
#include "api.h"
//functional style stuff

//curry/partial function application

namespace sm::fn {

template <typename C>
struct get_template_type;

template <template <typename > class C, typename T>
struct get_template_type<C<T>> {
   using type = T;
};

template<typename Function, typename... Arguments>
INLINE auto partial(Function &&func, Arguments&&... args) {
   return [=](auto&&... rest) {
      return func(std::forward<Arguments>(args)..., std::forward<Arguments>(rest)...);
   };
}

template <typename Collection, typename unop>
INLINE void for_each(Collection &&col, unop &&op) {
   std::for_each(std::begin(col), std::end(col), std::forward<unop>(op));
}

template <typename Collection, typename unop>
INLINE void generate(Collection &&col, unop &&op) {
   std::generate(std::begin(col), std::end(col), std::forward<unop>(op));
}

template <typename Condition, typename Function, typename... Arguments>
INLINE void while_true(Condition &&cond, Function &&f, Arguments&&... args) {
   while (cond() == true)
      f(std::forward<Arguments>(args)...);
}

template <typename Condition, typename Function, typename... Arguments>
INLINE void while_false(Condition &&cond, Function &&f, Arguments&&... args) {
   while (cond() == false)
      f(std::forward<Arguments>(args)...);
}

template <typename Counter, typename Counter_Function, typename Function, typename... Arguments>
INLINE void repeat(const Counter &&times, Counter_Function &&cf, Function &&f, Arguments&&... args) {
   for (Counter i = 0; i< times; ++i)
      f(cf(std::forward<Counter>(i)), std::forward<Arguments>(args)...);
}

template <typename Counter, typename Function, typename... Arguments>
INLINE void repeat(const Counter times, Function &&f, Arguments&&... args) {
   for (Counter i = 0; i< times; ++i)
      f(std::forward<Counter>(i), std::forward<Arguments>(args)...);
}


template <typename Collection, typename unop>
INLINE Collection map(Collection col, unop &&op) {
   std::transform(std::begin(col), std::end(col), std::begin(col), std::forward<unop>(op));
   return col;
}

template <typename Collection, typename binop>
INLINE Collection zip(Collection &&fc, Collection sc, binop &&op) {
   std::transform(std::begin(fc), std::end(fc), std::begin(sc), std::begin(sc), std::forward<binop>(op));
   return sc;
}

template <typename Collection, typename Condition>
INLINE bool exists(Collection &&col, Condition &&con) {
   auto exist = std::find_if(std::begin(col), std::end(col), std::forward<Condition>(con));
   return exist != col.end();
}


template <typename Collection, typename Predicate>
INLINE Collection filterNot(Collection col, Predicate &&predicate) {
   auto returnIterator = std::remove_if(std::begin(col), std::end(col), std::forward<Predicate>(predicate));
   col.erase(returnIterator, std::end(col));
   return col;
}

template <typename Collection, typename Predicate>
INLINE Collection filter(Collection col, Predicate &&predicate) {
   auto fnCol = filterNot(col, [predicate](typename Collection::value_type i) { return !predicate(i); });
   return fnCol;
}

}


//auto mult = [](int a, int b, int c)->int {
//   return a * b;
//};
//
//
//int m(int a, int b) {
//   return a * b;
//}
//
//auto mult2 = sm::fn::partial(mult, 1);
//
//auto mult3 = std::bind(m, 1, std::placeholders::_1);
//
////closure
//
//auto makeInc(int start) {
//   int n = start;
//
//   auto helper = [=]() mutable->auto {
//      return ++n;
//   };
//
//   return[=]() mutable->auto {
//      return helper();
//   };
//}

//sm::fn::repeat(20, [=](int c) {
//   std::cout << "hello" << c << std::endl;
//});
//
//sm::fn::repeat(20, [=](const int i)->int {return i / 2; }, [=](int c) {
//   std::cout << "hello" << c << std::endl;
//});
