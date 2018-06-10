#pragma once
#include "command_queue.hpp"
#include "scoped_timer.hpp"
#include <Windows.h>

//SAMPLE
//sm::thread::ThreadContext<> tc({});
//sm::thread::CommandQueue<>::CommandBuffer::type tasks;
//
//auto &q = tc.get_command_queue();
//
//tasks.emplace_back([argc]()->std::any { std::cout << "1" << std::endl; return (4); });
//tasks.emplace_back([argc]()->std::any { std::cout << "2" << std::endl; return (5); });
//tasks.emplace_back([argc]()->std::any { std::cout << "3" << std::endl; return (6); });
//
//auto results = q.submit(std::move(tasks));
//
//sm::fn::for_each(results, [](auto& f) {std::cout << std::any_cast<int>(f.get()) << std::endl; });


namespace sm::thread {

struct ThreadMapping_t {
   ThreadMapping_t() {
      //core i7
      if (true) {
         //TM_WINDOW_LOOP = 0;
         //TM_GAME_LOOP = 1;
         //TM_RENDER_LOOP = 2;
      } else {
         //ryzen

      }
   }

   ~ThreadMapping_t() = default;

   unsigned long long TM_WINDOW_LOOP = 0;
   unsigned long long  TM_GAME_LOOP = 1;
   unsigned long long  TM_RENDER_LOOP = 2;

};

inline ThreadMapping_t ThreadMapping;

inline void SetThreadMapping(const unsigned long long mapping) {
   SetThreadIdealProcessor(GetCurrentThread(), (DWORD)mapping);
   //SetThreadAffinityMask(GetCurrentThread(), mapping);
}

struct ContextCreationFlags {
   const bool current_context = false;
   const bool use_mapping = false;
   const unsigned long long mapping = 0;
   const bool wait_for_work = true;
};


//---------------------------------------------------------------------------------------
template<typename Rt = std::any, typename Arg = void, typename... Args>
class ThreadContext final {
public:
   ThreadContext(const ContextCreationFlags &flags) {
      auto thread_task = [this, &flags]()->void {
         if (flags.use_mapping) {
            sm::thread::SetThreadMapping(flags.mapping);
         }
         while (m_running.load(std::memory_order::memory_order_acquire)) {
            {
#ifdef SM_TIMING
               sm::time::ScopedTimer<std::chrono::nanoseconds> sc([this](const std::chrono::high_resolution_clock::duration &d)->void {
                  m_last_duration.store(d, std::memory_order::memory_order_release);
               });
#endif
               //can take a context param, e.g. the opengl or thread context.
               while (m_work_items.is_waiting_for_work() && flags.wait_for_work) {
                  std::this_thread::yield();
               }
               m_work_items.process_all_commands();
            }
         }
      };
      if (flags.current_context) {
         thread_task();
      } else {
         m_work_thread = std::make_unique<std::thread>(thread_task);
      }
   }

   ThreadContext(ThreadContext&&) = default;
   ThreadContext(const ThreadContext&) = delete;
   ThreadContext& operator=(ThreadContext&&) = default;
   ThreadContext& operator=(const ThreadContext&) = delete;

   ~ThreadContext() {
      request_exit();
   }

   void request_exit() {
      m_running.store(false, std::memory_order::memory_order_release);
      if (m_work_thread->joinable()) {
         m_work_thread->join();
      }
   }

   sm::thread::CommandQueue<Rt, Arg, Args...>& get_command_queue() {
      return m_work_items;
   }

   std::chrono::high_resolution_clock::duration get_duration() const {
      return m_last_duration.load(std::memory_order::memory_order_acquire);
   }

private:
   sm::thread::CommandQueue<Rt, Arg, Args...> m_work_items;
   std::unique_ptr<std::thread> m_work_thread;
   std::atomic_bool m_running = true;
   std::atomic<std::chrono::high_resolution_clock::duration> m_last_duration;
};
}
