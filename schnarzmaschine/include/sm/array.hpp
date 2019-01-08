#pragma once
#include <malloc.h>
#include <type_traits>
#include <cassert>
#include "api.h"

//up to 64kb is allocated on the stack, above is allocated on the heap
#define MAX_STACK_ALLOC_SIZE 1024*64

#ifdef _DEBUG

#ifndef _CRTDBG_MAP_ALLOC
#define sm_stack_alloc(size)                                                           \
            __pragma(warning(suppress: 6255 6386))                                       \
            (_MallocaComputeSize(size) != 0                                              \
                ? _MarkAllocaS(malloc(_MallocaComputeSize(size)), _ALLOCA_S_HEAP_MARKER) \
                : NULL)
#endif

#else

#define sm_stack_alloc(size)                                                                 \
        __pragma(warning(suppress: 6255 6386))                                             \
        (_MallocaComputeSize(size) != 0                                                    \
            ? (((_MallocaComputeSize(size) <= MAX_STACK_ALLOC_SIZE)                         \
                ? _MarkAllocaS(_alloca(_MallocaComputeSize(size)), _ALLOCA_S_STACK_MARKER) \
                : _MarkAllocaS(malloc(_MallocaComputeSize(size)), _ALLOCA_S_HEAP_MARKER))) \
            : NULL)

#endif



namespace sm {
template<class T, size_t ALIGNMENT= 16, class Enable = void>
class array final {

   //prevent heap alloc
   static void *operator new     (size_t) = delete;
   static void *operator new[](size_t) = delete;
   static void  operator delete  (void*) = delete;
   static void  operator delete[](void*) = delete;

public:
   INLINE array(const size_t size)
      :
      m_size(size) {
      m_data = (T*)sm_stack_alloc(sizeof(T) * m_size + ALIGNMENT + 1);	//alloca will create chunk on the stack
      m_aligned_data = (T*)(((UINT_PTR)m_data + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
      m_aligned_data = new(m_data) T();
   }

   INLINE array(std::initializer_list<T> l)
      :
      m_size(l.size()) {
      m_data = (T*)sm_stack_alloc(sizeof(T) * m_size + ALIGNMENT + 1);	//alloca will create chunk on the stack
      m_aligned_data = (T*)(((UINT_PTR)m_data + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
      m_aligned_data = new(m_data) T();
      std::copy(l.begin(), l.end(), this->begin());
   }

   INLINE array(const array<typename T> &other)
      :
      m_size(other.m_size) {
      m_data = (T*)sm_stack_alloc(sizeof(T) * m_size + ALIGNMENT + 1);	//alloca will create chunk on the stack
      m_aligned_data = (T*)(((UINT_PTR)m_data + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
      m_aligned_data = new(m_data) T();
      memcpy(m_aligned_data, other.m_aligned_data, sizeof(T) * m_size);
   }

   INLINE array& operator=(const array<typename T> &other) {
      m_size = other.m_size;
      m_data = (T*)sm_stack_alloc(sizeof(T) * m_size + ALIGNMENT + 1);	//alloca will create chunk on the stack
      m_aligned_data = (T*)(((UINT_PTR)m_data + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
      m_aligned_data = new(m_data) T();
      memcpy(m_aligned_data, other.m_aligned_data, sizeof(T) * m_size);
   }

   INLINE array& operator=(std::initializer_list<T> other) {
      m_size = other.m_size;
      m_data = (T*)sm_stack_alloc(sizeof(T) * m_size + ALIGNMENT + 1);	//alloca will create chunk on the stack
      m_aligned_data = (T*)(((UINT_PTR)m_data + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
      m_aligned_data = new(m_data) T();
      std::copy(other.begin(), other.end(), begin());
   }

   INLINE array(const array<typename T> &&other) {
      m_size = other.m_size;
      m_data = other.m_data;
      m_aligned_data = other.m_aligned_data;
      other.m_size = 0;
      other.m_aligned_data = nullptr;
      other.m_data = nullptr;
   }

   INLINE ~array() {
      m_aligned_data->~T();
      _freea(m_data);
   }

   INLINE T* const data() const {
      return m_aligned_data;
   }

   INLINE T* data() {
      return m_aligned_data;
   }

   INLINE T& operator[](const size_t idx) const {
      assert(idx < m_size);
      return m_aligned_data[idx];
   }

   INLINE T& operator[](const size_t idx) {
      assert(idx < m_size);
      return m_aligned_data[idx];
   }

   INLINE size_t size() const {
      return m_size;
   }

   INLINE T* begin() {
      return &(m_aligned_data[0]);
   }

   INLINE T* end() {
      return &(m_aligned_data[m_size]);
   }

   INLINE T const * cbegin() const {
      return m_aligned_data;
   }

   INLINE T const * cend() const {
      return &(m_aligned_data[m_size]);
   }


private:
   const size_t m_size;
   T *m_aligned_data;
   T *m_data;
};

template<class T, size_t ALIGNMENT>
class array<T, ALIGNMENT, typename std::enable_if<std::is_fundamental<T>::value>::type> final {

   //prevent heap alloc
   static void *operator new     (size_t) = delete;
   static void *operator new[](size_t) = delete;
   static void  operator delete  (void*) = delete;
   static void  operator delete[](void*) = delete;

public:
   INLINE array(const size_t size)
      :
      m_size(size) {
      m_data = (T*)sm_stack_alloc(sizeof(T) * m_size + ALIGNMENT + 1);	//alloca will create chunk on the stack
      m_aligned_data = (T*)(((UINT_PTR)m_data + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));

   }

   INLINE array(const size_t size, const T init_value)
      :
      m_size(size) {
      m_data = (T*)sm_stack_alloc(sizeof(T) * m_size + ALIGNMENT + 1);	//alloca will create chunk on the stack
      m_aligned_data = (T*)(((UINT_PTR)m_data + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
      std::fill(begin(), end(), init_value);
   }

   INLINE array(std::initializer_list<T> l)
      :
      m_size(l.size()) {
      m_data = (T*)sm_stack_alloc(sizeof(T) * m_size + ALIGNMENT + 1);	//alloca will create chunk on the stack
      m_aligned_data = (T*)(((UINT_PTR)m_data + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
      std::copy(l.begin(), l.end(), this->begin());
   }

   INLINE array(const array<typename T> &other)
      :
      m_size(other.m_size) {
      m_data = (T*)sm_stack_alloc(sizeof(T) * m_size + ALIGNMENT + 1);	//alloca will create chunk on the stack
      m_aligned_data = (T*)(((UINT_PTR)m_data + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
      memcpy(m_aligned_data, other.m_aligned_data, sizeof(T) * m_size);
   }

   INLINE array& operator=(const array<typename T> &other) {
      m_size = other.m_size;
      m_data = (T*)sm_stack_alloc(sizeof(T) * m_size + ALIGNMENT + 1);	//alloca will create chunk on the stack
      m_aligned_data = (T*)(((UINT_PTR)m_data + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
      memcpy(m_aligned_data, other.m_aligned_data, sizeof(T) * m_size);
   }

   INLINE array& operator=(std::initializer_list<T> other) {
      m_size = other.m_size;
      m_data = (T*)sm_stack_alloc(sizeof(T) * m_size + ALIGNMENT + 1);	//alloca will create chunk on the stack
      m_aligned_data = (T*)(((UINT_PTR)m_data + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1));
      std::copy(other.begin(), other.end(), begin());
   }

   INLINE array(const array<typename T> &&other) {
      m_size = other.m_size;
      m_aligned_data = other.m_aligned_data;
      m_data = other.m_data;
      other.m_size = 0;
      other.m_aligned_data = nullptr;
      other.m_data = nullptr;
   }

   INLINE ~array() {
      _freea(m_data);
   }

   INLINE T* const data() const {
      return m_aligned_data;
   }

   INLINE T* data() {
      return m_aligned_data;
   }

   INLINE T& operator[](const size_t idx) const {
      assert(idx < m_size);
      return m_aligned_data[idx];
   }

   INLINE T& operator[](const size_t idx) {
      assert(idx < m_size);
      return m_aligned_data[idx];
   }

   INLINE size_t size() const {
      return m_size;
   }

   INLINE T* begin() {
      return &(m_aligned_data[0]);
   }

   INLINE T* end() {
      return &(m_aligned_data[m_size]);
   }

   INLINE T const * cbegin() const {
      return m_aligned_data;
   }

   INLINE T const * cend() const {
      return &(m_aligned_data[m_size]);
   }


private:
   const size_t m_size;
   T *m_aligned_data;
   T *m_data;
};

}


namespace std {
template<class T>
T* begin(const sm::array<typename T> &a) {
   return a.begin();
}

template<class T>
T* end(const sm::array<typename T> &a) {
   return a.end();
}
}
