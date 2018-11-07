//Testbench for lots of new stuff



#include <iostream>
#include <string>
#include <string_view>
#include <memory>

#include "gsl/gsl"
#include "rxcpp/rx.hpp"

using shared_string = std::shared_ptr<std::string>;

class shared_string_view
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
};


shared_string make_shared_string(std::string &&s)
{
    return std::make_shared<std::string>(std::forward<std::string>(s));
}


int main()
{
    //raw
    std::string s{"hello"};
    std::string_view v{s};

    //ref counted

    auto sp = make_shared_string("hello");

    auto ssv1 = shared_string_view{sp};
    auto ssv2 = shared_string_view{sp, 2};
    auto ssv3 = shared_string_view{sp, 1, 4};



    std::cout << ssv1.view() << std::endl;
    std::cout << ssv2.view() << std::endl;
    std::cout << ssv3.view() << std::endl;

}

