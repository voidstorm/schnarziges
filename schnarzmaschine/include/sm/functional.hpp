#pragma once
#include <functional>
#include <algorithm>
#include <tuple>
#include <type_traits>
#include <any>
#include <optional>
#include <variant>
#include <array>
#include "api.h"
#include "macro_util.hpp"
//functional style stuff

//curry/partial function application

namespace sm::fn {

template <typename C>
struct get_template_type;

template <template <typename > class C, typename T>
struct get_template_type<C<T>> {
   using type = T;
};

template <typename E>
constexpr auto enum_cast(E e) noexcept
{
    return static_cast<std::underlying_type_t<E>>(e);
}
   
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

// a really helpful tuple iterator, credits go to: https://foonathan.net/blog/2017/03/01/tuple-iterator.html and
// https://github.com/arnemertz/blog-code/blob/master/2017-03-tuple-iterator/tuple_iterator.h

//class TestClass final {
//public:
//   TestClass() = default;
//   ~TestClass() = default;
//
//   SM_DECLARE_PRIVATE_GETTER()
//      SM_DECLARE_PRIVATE_SETTER()
//      SM_DECLARE_PRIVATE_MEMBER_ITER()
//#define PRIVATES(MEMBER) \
//        MEMBER(int,m_count,1), \
//        MEMBER(const bool,m_use,true), \
//        MEMBER(std::string,m_name,"bla")
//      SM_DECLARE_PRIVATE_MEMBERS_WITHFLAG(PRIVATES)
//};
//
//
//using MyClass = std::tuple<std::tuple<bool, int>, std::tuple<bool, std::string>, std::tuple<bool, double>>;
//TestClass t;
//t.set<TestClass::m_name>("huhu");

//std::cout << t.get<TestClass::m_name>() << std::endl;

//auto[a, b] = t.get<TestClass::m_name>();
//
//MyClass tup{ std::make_tuple(false,1), std::make_tuple(false,"bla"), std::make_tuple(false,1.34) };
//
//
//auto print_elem = sm::fn::overload_unref{
//   [](int i) { std::cout << "int: " << i << '\n'; },
//   [](std::string& s) { std::cout << "string: " << s << '\n'; s = "helo"; },
//   [](double d) { std::cout << "double: " << d << '\n'; },
//   [](bool b) { std::cout << "bool: " << b << '\n'; }
//};
//
//auto range = sm::fn::to_range(tup);



//for (const auto &  [dirty, elem] : t) {
//   std::visit(print_elem, elem);
//}

//for (const auto [dirty, elem] : sm::fn::to_range(tup)) {
//   std::visit(print_elem, elem);
//}

//std::visit(print_elem, sm::fn::to_range(tup)[1]);


template <typename Tup, typename R, typename F, std::size_t... Idxs>
struct tuple_runtime_access_table {
   using tuple_type = Tup;
   using return_type = R;
   using converter_fun = F;

   template <std::size_t N>
   static return_type access_tuple(tuple_type& t, converter_fun& f) {
      return f(std::get<N>(t));
   }

   using accessor_fun_ptr = return_type(*)(tuple_type&, converter_fun&);
   const static auto table_size = sizeof...(Idxs);

   constexpr static std::array<accessor_fun_ptr, table_size> lookup_table = {
      { &access_tuple<Idxs>... }
   };
};


template <typename R, typename Tup, typename F, std::size_t... Idxs>
auto call_access_function(Tup& t, std::size_t i, F f, std::index_sequence<Idxs...>) {
   auto& table = tuple_runtime_access_table<Tup, R, F, Idxs...>::lookup_table;
   auto* access_function = table[i];
   return access_function(t, f);
}



template <typename Tup> struct common_tuple_access;

template <typename... Ts>
struct common_tuple_access<std::tuple<Ts...>> {
   using type = std::variant<std::reference_wrapper<Ts>...>;
};

template <typename T1, typename T2>
struct common_tuple_access<std::pair<T1, T2>> {
   using type = std::variant<std::reference_wrapper<T1>, std::reference_wrapper<T2>>;
};

template <typename T, auto N>
struct common_tuple_access<std::array<T, N>> {
   using type = std::variant<std::reference_wrapper<T>>;
};

template <typename Tup>
using common_tuple_access_t = typename common_tuple_access<Tup>::type;



template <typename Tup>
auto runtime_get(Tup& t, std::size_t i) {
   return call_access_function<common_tuple_access_t<Tup>>(
      t, i,
      [](auto & element) { return std::ref(element); },
      std::make_index_sequence<std::tuple_size_v<Tup>>{}
   );
}

template <typename Tup> class tuple_iterator {
   Tup& t;
   size_t i;
public:
   tuple_iterator(Tup& tup, size_t idx)
      : t{ tup }, i{ idx } {
   }

   tuple_iterator& operator++() {
      ++i; return *this;
   }
   bool operator==(tuple_iterator const& other) const {
      return std::addressof(other.t) == std::addressof(t)
         && other.i == i;
   }

   bool operator!=(tuple_iterator const& other) const {
      return !(*this == other);
   }

   auto operator*() const {
      return runtime_get(t, i);
   }
};

template <typename Tup>
class to_range {
   Tup& t;
public:
   to_range(Tup& tup) : t{ tup } {}
   auto begin() {
      return tuple_iterator{ t, 0 };
   }
   auto end() {
      return tuple_iterator{ t, std::tuple_size_v<Tup> };
   }

   auto operator[](std::size_t i) {
      return runtime_get(t, i);
   }
};

//template <class ... Fs>
//struct overload : Fs... {
//   overload(Fs&&... fs) : Fs{ fs }... {}
//   using Fs::operator()...;
//};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...)->overloaded<Ts...>;

template <class ... Fs>
struct overload_unref : overloaded<Fs...> {
   overload_unref(Fs&&... fs)
      : overloaded<Fs...>{ std::forward<Fs>(fs)... }
   {}

   using overloaded<Fs...>::operator();

   template <class T>
   auto operator()(std::reference_wrapper<T> rw) {
      return (*this)(rw.get());
   }
};




}

//TUPLE sample

//MyClass tup{ 1, "bla", 1.34 };
//
//
//auto print_elem = sm::fn::overload_unref(
//   [](int i) { std::cout << "int: " << i << '\n'; },
//   [](std::string const& s) { std::cout << "string: " << s << '\n'; },
//   [](double d) { std::cout << "double: " << d << '\n'; }
//);
//
//for (auto const& elem : sm::fn::to_range(tup)) {
//   std::visit(print_elem, elem);
//}
//
//std::visit(print_elem, sm::fn::to_range(tup)[1]);


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
