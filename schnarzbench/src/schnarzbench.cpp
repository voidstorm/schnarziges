//Testbench for lots of new stuff



#include <iostream>
#include <string>
#include "gsl/gsl"
#include "rxcpp/rx.hpp"

#include "sm/shared_string_view.hpp"
#include "sm/command_queue.hpp"
#include "sm/thread_context.hpp"


using sm::make_shared_string;
using sm::shared_string_view;
using namespace sm::thread;

int main() {
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

   auto results2 = q.submit<static_tasks.size()>(std::move(static_tasks));


   sm::fn::for_each(results, [](auto& f) {std::cout << std::any_cast<std::string>(f.get()) << std::endl; });
   sm::fn::for_each(results2, [](auto& f) {std::cout << std::any_cast<std::string>(f.get()) << std::endl; });

   tc.request_exit().get();
   

}

