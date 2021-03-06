//Testbench for lots of new stuff


//Testbench for lots of new stuff

//#define _HAS_EXCEPTIONS 0


#include <iostream>
#include <string>
#include <tuple>
#include <memory>
#include "gsl/gsl"

#include "sm/shared_string_view.hpp"
#include "sm/command_queue.hpp"
#include "sm/thread_context.hpp"
#include "sm/functional.hpp"
#include "sm/array.hpp"


#define CACHE_LINE_SIZE 64  
#define CACHE_ALIGN __declspec(align(CACHE_LINE_SIZE))



using sm::make_shared_string;
using sm::shared_string_view;
using namespace sm::thread;
using namespace sm;


enum class Enum: unsigned
{
    _1= 1,
    _2,
    _3
};

using IntPair = std::tuple<int, int>;

// every object of type sse_t will be aligned to 16-byte boundary
struct alignas(16) sse_t {
   float sse_data[4];
};

// the array "cacheline" will be aligned to 64-byte boundary
alignas(CACHE_LINE_SIZE) char cacheline[CACHE_LINE_SIZE];


void add_vec_raw(const float* __restrict a, const float* __restrict b, float* __restrict out) {
   for (size_t i = 0; i < 8; ++i) {
      out[i] = a[i] + b[i];
   }
}


NO_INLINE void add_vec(const std::vector<float> &a, const std::vector<float> &b, std::vector<float> &out) {
   const float * __restrict x = a.data();
   const float * __restrict y = b.data();
   float * __restrict z = out.data();

   for (int i = 0; i < 8; ++i) {
      z[i] = x[i] + y[i];
   }
}


int main() {

    std::cout << "enum: " << sm::enum_cast(Enum::_1) << std::endl;

   //vec
   std::vector<float> a{ 1,2,3,4,5,6,7,8 };
   std::vector<float> b{ 1,2,3,4,5,6,7,8 };
   std::vector<float> result(8);

   //ref counted
   CACHE_ALIGN float x[]{ 1,2,3,4,5,6,7,8 };
   CACHE_ALIGN float y[]{ 1,2,3,4,5,6,7,8 };
   CACHE_ALIGN float r[8];

   //add_vec_raw(a.data(), b.data(), r);

   bool cond{ true };
   int cnt = 0;

   while_true(cond, [&cond, cnt]()mutable->void {
      cnt++;
      if (cnt > 10) {
         cond = false;
         std::cout << "hello once: " << cnt << std::endl;
      }
   });


   //add_vec_raw(x, y, r);
   add_vec(a, b, result);


   //dyn array
   sm::array<int> da{1,2,3,4,5};
   sm::array<int> da23(1023, 7); 
   sm::array<int> da2{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

   //sm::array<IntPair> pair_array{ {1,2}, {3,4} };

   //stack array, aligned
   sm::array<IntPair, CACHE_LINE_SIZE> pair_array{ {1,2}, {3,4} };


   for (const auto &i : da)
   {
       std::cout << "dyn: " << i << std::endl;
   }

   for (int i = 0; i < 10; ++i)
   {
       std::cout << "for dyn: " << da23[i] << std::endl;
   }

   for (const auto &i : da2) {
      std::cout << "for dyn2: " << i << std::endl;
   }

   //raw
   std::string s{ "hello" };
   std::string_view v{ s };

   //ref counted

   auto sp = make_shared_string("hello");

   auto ssv1 = shared_string_view{ sp };
   auto ssv2 = shared_string_view{ sp, 2 };
   auto ssv3 = shared_string_view{ sp, 1, 4 };
   auto ssv4 = shared_string_view{ ssv3 };

   std::cout << ssv1.view() << std::endl;
   std::cout << ssv2.view() << std::endl;
   std::cout << ssv3.view() << std::endl;
   std::cout << ssv4.view() << std::endl;


   //command queue
   //sm::thread::CommandQueue<> q;
   //sm::thread::CommandQueue<>::CommandBuffer::type tasks;

   //thread context
   ThreadContext<> tc({});
   CommandQueue<>::DynamicCommandBuffer::type tasks;


   tasks.emplace_back([s]()->std::any {std::cout << "task 1: " << s << std::endl; return (s); });
   tasks.emplace_back([s]()->std::any {std::cout << "task 2: " << s << std::endl; return (s); });
   tasks.emplace_back([s]()->std::any {std::cout << "task 3: " << s << std::endl; return (s); });

   CommandQueue<>::StaticCommandBuffer::type<3> static_tasks{
      CommandQueue<>::QueueTask::type([s]()->std::any {std::cout << "task 4: " << s << std::endl; return (s); }),
      CommandQueue<>::QueueTask::type([s]()->std::any {std::cout << "task 5: " << s << std::endl; return (s); }),
      CommandQueue<>::QueueTask::type([s]()->std::any {std::cout << "task 6: " << s << std::endl; return (s); })
   };




   auto &q = tc.get_command_queue();
   auto results = q.submit(std::move(tasks));

   auto results2 = q.submit<3>(std::move(static_tasks));


   sm::for_each(results, [](auto& f) {std::cout << std::any_cast<std::string>(f.get()) << std::endl; });
   sm::for_each(results2, [](auto& f) {std::cout << std::any_cast<std::string>(f.get()) << std::endl; });

   tc.request_exit().get();


}

