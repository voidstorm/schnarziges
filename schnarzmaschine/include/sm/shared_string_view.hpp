#pragma once
#include <memory>
#include <string>
#include <string_view>

namespace sm {
using shared_string = std::shared_ptr<std::string>;

class shared_string_view {
public:
   shared_string_view(const shared_string &in)
      :
      m_s(in),
      m_v(*m_s) {
   }

   shared_string_view(const shared_string &in, size_t end)
      :
      m_s(in),
      m_v(m_s->data(), end) {

   }

   shared_string_view(const shared_string &in, size_t start, size_t end)
      :
      m_s(in),
      m_v(m_s->data() + start, end) {

   }

   shared_string_view(const shared_string_view &other)
      :
      m_s(other.m_s),
      m_v(other.m_v) {
   }

   shared_string_view(shared_string_view &&other)
      :
      m_s(std::move(other.m_s)),
      m_v(std::move(other.m_v)) {
#ifdef _DEBUG
      other.m_invalid = true;
#endif
   }

   shared_string_view& operator=(const shared_string_view &other) {
      m_s = other.m_s;
      m_v = other.m_v;
      return *this;
   }

   ~shared_string_view() {

   }

   std::string_view view() {
      return m_v;
   }

private:
   shared_string m_s;
   std::string_view m_v;
#ifdef _DEBUG
   bool m_invalid = false;
#endif
};


shared_string make_shared_string(std::string &&s) {
   return std::make_shared<std::string>(std::forward<std::string>(s));
}
}

