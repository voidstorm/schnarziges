//Testbench for lots of new stuff

#define _HAS_EXCEPTIONS 0

#include <iostream>
#include <string>
#include <string_view>
#include <memory>
#include <vector>

#include "gsl/gsl"
#include "rxcpp/rx.hpp"

#define CACHE_LINE 128  
#define CACHE_ALIGN __declspec(align(CACHE_LINE))

using shared_string = std::shared_ptr<std::string>;

class shared_string_view final
{
public:
    shared_string_view(const shared_string &in)
        :
        m_s(in),
        m_v(*m_s)
    {
    }

    shared_string_view(const shared_string &in, size_t end)
        :
        m_s(in),
        m_v(m_s->data(), end)
    {

    }

    shared_string_view(const shared_string &in, size_t start, size_t end)
        :
        m_s(in),
        m_v(m_s->data() + start, end)
    {

    }

    shared_string_view(const shared_string_view &other)
        :
        m_s(other.m_s),
        m_v(other.m_v)
    {
    }

    shared_string_view(shared_string_view &&other)
        :
        m_s(std::move(other.m_s)),
        m_v(std::move(other.m_v))
    {
#ifdef _DEBUG
        other.m_invalid = true;
#endif
    }

    shared_string_view& operator=(const shared_string_view &other)
    {
        m_s = other.m_s;
        m_v = other.m_v;
        return *this;
    }

    ~shared_string_view()
    {

    }

    std::string_view view()
    {
        return m_v;
    }

private:
    shared_string m_s;
    std::string_view m_v;
#ifdef _DEBUG
    bool m_invalid = false;
#endif
};



shared_string make_shared_string(std::string &&s)
{
    return std::make_shared<std::string>(std::forward<std::string>(s));
}


void add_vec(const std::vector<float> &a, const std::vector<float> &b, std::vector<float> &out)
{
    for (int i = 0; i < 8; ++i)
    {
        out[i] = a[i] + b[i];
    }
}

void add_vec_raw(const float* __restrict a, const float* __restrict b, float* __restrict out)
{
    CACHE_ALIGN const float * __restrict x = a;
    CACHE_ALIGN const float * __restrict y = b;
    CACHE_ALIGN float * __restrict z = out;

    for (size_t i = 0; i < 8; ++i)
    {
        z[i] = x[i] + y[i];
    }
}


int main()
{
    std::vector<float> a{1,2,3,4,5,6,7,8};
    std::vector<float> b{1,2,3,4,5,6,7,8};
    std::vector<float> result(8);

    CACHE_ALIGN float x[]{1,2,3,4,5,6,7,8};
    CACHE_ALIGN float y[]{1,2,3,4,5,6,7,8};
    CACHE_ALIGN float r[8];

    //add_vec_raw(a.data(), b.data(), r);
    
    add_vec_raw(x, y, r);

    for (size_t i = 0; i < 8; ++i)
    {
        std::cout << r[i] << std::endl;
    }

    ////raw
    //std::string s{"hello"};
    //std::string_view v{s};

    ////ref counted

    //auto sp = make_shared_string("hello");

    //auto ssv1 = shared_string_view{sp};
    //auto ssv2 = shared_string_view{sp, 2};
    //auto ssv3 = shared_string_view{sp, 1, 4};
    //auto ssv4 = shared_string_view{ssv3};



    //std::cout << ssv1.view() << std::endl;
    //std::cout << ssv2.view() << std::endl;
    //std::cout << ssv3.view() << std::endl;
    //std::cout << ssv4.view() << std::endl;



}

