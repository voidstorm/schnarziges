#pragma once
#include <any>
#include <optional>
#include <functional>
#include <queue>
#include <future>
#include <atomic>
#include <thread>

#include "api.h"
#include "functional.hpp"

namespace sm::thread {

using QueueTask = std::packaged_task<std::any(void)>;

class CommandQueue final {
public:
   //---------------------------------------------------------------------------------------
   CommandQueue() {
      m_waiting_for_work.test_and_set(std::memory_order_acquire);
      m_commands_push = &m_commands0;
      m_commands_pop = &m_commands1;
   }

   //---------------------------------------------------------------------------------------
   ~CommandQueue() = default;
   CommandQueue(CommandQueue&&) = default;
   CommandQueue(const CommandQueue&) = delete;
   CommandQueue& operator=(CommandQueue&&) = default;
   CommandQueue& operator=(const CommandQueue&) = delete;


   //---------------------------------------------------------------------------------------
   std::future<std::any> submit(QueueTask &&task) {
      auto local_task{ std::move(task) };
      auto f = local_task.get_future();
      while (m_busy.test_and_set(std::memory_order_acquire));
      m_commands_push->push(std::move(local_task));
      m_waiting_for_work.clear(std::memory_order_release);
      m_busy.clear(std::memory_order_release);
      return std::move(f);
   }

   //---------------------------------------------------------------------------------------
   size_t get_size() {
      return m_commands_push->size();
   }

private:
   //---------------------------------------------------------------------------------------
   void swap() {
      while (m_busy.test_and_set(std::memory_order_acquire));
      std::swap(m_commands_push, m_commands_pop);
      m_busy.clear(std::memory_order_release);
   }

   //---------------------------------------------------------------------------------------
   void process_all_commands() {
      swap();
      while (!m_commands_pop->empty()) {
         auto &m = m_commands_pop->front();
         m();
         m_commands_pop->pop();
      }
   }

   //---------------------------------------------------------------------------------------
   bool is_waiting_for_work() {
      return m_waiting_for_work.test_and_set(std::memory_order_acquire);
   }


   std::queue<QueueTask> m_commands0;
   std::queue<QueueTask> m_commands1;
   std::queue<QueueTask> *m_commands_push;
   std::queue<QueueTask> *m_commands_pop;
   std::atomic_flag m_busy = ATOMIC_FLAG_INIT;
   std::atomic_flag m_waiting_for_work = ATOMIC_FLAG_INIT;
   bool m_false = false;
   friend class ThreadContext;
};


}
