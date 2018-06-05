#pragma once
#include <any>
#include <optional>
#include <functional>
#include <deque>
#include <future>
#include <atomic>
#include <thread>
#include <algorithm>
#include <type_traits>

#include "api.h"
#include "functional.hpp"

namespace sm::thread {

template<typename Rt=std::any, typename Arg=void, typename... Args>
class CommandQueue final {
public:
   struct QueueTask {
      using type= std::packaged_task<Rt(Arg, Args...)>;
   };

   struct CommandBuffer {
      using type = std::vector<typename QueueTask::type>;
   };

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
      auto f = task.get_future();
      while (m_busy.test_and_set(std::memory_order_acquire));
      m_commands_push->push_back(std::move(task));
      m_waiting_for_work.clear(std::memory_order_release);
      m_busy.clear(std::memory_order_release);
      return std::move(f);
   }

   //---------------------------------------------------------------------------------------
   //this is a sink function
   template<typename Container>
   auto submit(Container &&tasks)->std::vector<std::future<Rt>> {
      std::vector<std::future<Rt>> fs;
      for (auto& item : tasks) {
         fs.emplace_back(std::move(item.get_future()));
      }
      while (m_busy.test_and_set(std::memory_order_acquire));
      m_commands_push->insert(m_commands_push->end(), std::make_move_iterator(std::begin(tasks)), std::make_move_iterator(std::end(tasks)));
      m_waiting_for_work.clear(std::memory_order_release);
      m_busy.clear(std::memory_order_release);
      return std::move(fs);
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
   void process_all_commands(Args&&... args) {
      swap();
      while (!m_commands_pop->empty()) {
         auto &m = m_commands_pop->front();
         m(std::forward<Args>(args)...);
         m_commands_pop->pop_front();
      }
   }

   //---------------------------------------------------------------------------------------
   bool is_waiting_for_work() {
      return m_waiting_for_work.test_and_set(std::memory_order_acquire);
   }


   std::deque<typename QueueTask::type> m_commands0;
   std::deque<typename QueueTask::type> m_commands1;
   std::deque<typename QueueTask::type> *m_commands_push;
   std::deque<typename QueueTask::type> *m_commands_pop;
   std::atomic_flag m_busy = ATOMIC_FLAG_INIT;
   std::atomic_flag m_waiting_for_work = ATOMIC_FLAG_INIT;
};


}
