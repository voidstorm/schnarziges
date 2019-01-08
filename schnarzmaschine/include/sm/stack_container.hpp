#pragma once
#include <malloc.h>
#include "api.h"

namespace sm
{
    template<class T>
    class array final
    {
    public:
        INLINE array(const size_t size)
            :
            m_size(size)
        {
            T* m_data = (T*)_malloca(sizeof(T) * m_size + 1);	//alloca will create chunk on the stack
        }

        INLINE array(std::initializer_list<T> l)
            :
            m_size(l.size())
        {
            T* m_data = (T*)_malloca(sizeof(T) * m_size + 1);
            std::copy(l.begin(), l.end(), this->begin());
        }

        INLINE array(const array<T> &other)
            :
            m_size(other.m_size)
        {
            T* m_data = (T*)_malloca(sizeof(T) * m_size + 1);
            memcpy(m_data, other.m_data, sizeof(T) * m_size);
        }

        INLINE array(const array<T> &&other)
        {
            m_size = other.m_size;
            m_data = other.m_data;
            other.m_size = 0;
            other.m_data = nullptr;
        }

        INLINE array& operator=(const array<T> &other)
        {
            m_size = other.m_size;
            T* m_data = (T*)_malloca(sizeof(T) * m_size + 1);
            memcpy(m_data, other.m_data, sizeof(T) * m_size);
        }

        INLINE array& operator=(std::initializer_list<T> other)
        {
            m_size = other.m_size;
            T* m_data = (T*)_malloca(sizeof(T) * m_size + 1);
            std::copy(other.begin(), other.end(), begin());
        }

        INLINE ~array()
        {
            //nothing to do here, stack based
        }

        INLINE T* const data() const
        {
            return m_data;
        }

        INLINE T* data()
        {
            return m_data;
        }

        INLINE T& operator[](const size_t idx) const
        {
            return m_data[idx];
        }

        INLINE T& operator[](const size_t idx)
        {
            return m_data[idx];
        }

        INLINE size_t size() const
        {
            return m_size;
        }

        INLINE T* begin()
        {
            return m_data;
        }

        INLINE T* end()
        {
            return &(m_data[m_size]);
        }

        INLINE T const * cbegin() const
        {
            return m_data;
        }

        INLINE T const * cend() const
        {
            return &(m_data[m_size]);
        }


    private:
        T *m_data;
        const size_t m_size;
    };
}


namespace std
{
    template<class T>
    T* begin(const sm::array<typename T> &a)
    {
        return a.begin();
    }

    template<class T>
    T* end(const sm::array<typename T> &a)
    {
        return a.end();
    }
}
