#pragma once

#include <vector>
#include <future>
#include <atomic>
#include <thread>

#include "api.h"
#include "functional.hpp"

//SAMPLE
//-----------------------------------------------------------------------------
//sm::thread::CommandQueue<> q;
//sm::thread::CommandQueue<>::CommandBuffer::type tasks;
//
//
//tasks.emplace_back([argc]()->std::any {return (argc); });
//tasks.emplace_back([argc]()->std::any {return (argc); });
//tasks.emplace_back([argc]()->std::any {return (argc); });
//
//q.submit(std::move(tasks));

#define SM_INIT_CMD_QUEUE_SIZE 1024

namespace sm::thread {
template<typename Rt, typename Arg, typename... Args> class ThreadContext;

template<typename Rt = std::any, typename Arg = void, typename... Args>
class CommandQueue final {

public:
   struct QueueTask {
      using type = std::packaged_task<Rt(Arg, Args...)>;
   };

   struct CommandBuffer {
      using type = std::vector<typename QueueTask::type>;
   };

   //---------------------------------------------------------------------------------------
   CommandQueue() {
      m_waiting_for_work.test_and_set(std::memory_order_acquire);
      m_commands_push = &m_commands0;
      m_commands_pop = &m_commands1;
      m_commands_push->reserve(SM_INIT_CMD_QUEUE_SIZE);
      m_commands_pop->reserve(SM_INIT_CMD_QUEUE_SIZE);
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
      for (auto &cmd : *m_commands_pop) {
         cmd(std::forward<Args>(args)...);
      }
      m_commands_pop->clear();
   }

   //---------------------------------------------------------------------------------------
   bool is_waiting_for_work() {
      return m_waiting_for_work.test_and_set(std::memory_order_acquire);
   }


   std::vector<typename QueueTask::type> m_commands0;
   std::vector<typename QueueTask::type> m_commands1;
   std::vector<typename QueueTask::type> *m_commands_push;
   std::vector<typename QueueTask::type> *m_commands_pop;
   std::atomic_flag m_busy = ATOMIC_FLAG_INIT;
   std::atomic_flag m_waiting_for_work = ATOMIC_FLAG_INIT;

   template<typename Rt, typename Arg, typename... Args> friend class sm::thread::ThreadContext;
};


}
