#pragma once

#define EXPAND(X) X 
#define __NARG__(...)  EXPAND(__NARG_I_(__VA_ARGS__,__RSEQ_N()))
#define __NARG_I_(...) EXPAND(__ARG_N(__VA_ARGS__))
#define __ARG_N( \
      _1, _2, _3, _4, _5, _6, _7, _8, _9,_10, \
     _11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
     _21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
     _31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
     _41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
     _51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
     _61,_62,_63,N,...) N
#define __RSEQ_N() \
     63,62,61,60,                   \
     59,58,57,56,55,54,53,52,51,50, \
     49,48,47,46,45,44,43,42,41,40, \
     39,38,37,36,35,34,33,32,31,30, \
     29,28,27,26,25,24,23,22,21,20, \
     19,18,17,16,15,14,13,12,11,10, \
     9,8,7,6,5,4,3,2,1,0

// general definition for any function name
#define _VFUNC_(name, n) name##n
#define _VFUNC(name, n) _VFUNC_(name, n)
#define VFUNC(func, ...) EXPAND(_VFUNC(func, EXPAND( __NARG__(__VA_ARGS__))) (__VA_ARGS__))


#define MEMBER_LIST(...) EXPAND(VFUNC(MEMBER_LIST, __VA_ARGS__))

#define SM_GET_NAME(tYpE,nAmE,vAlUe) nAmE
#define SM_GET_TYPE(tYpE,nAmE,vAlUe) tYpE
#define SM_GET_TYPE_WITHFLAG(tYpE,nAmE,vAlUe) std::tuple<bool, tYpE>
#define SM_GET_VALUE(tYpE,nAmE,vAlUe) vAlUe
#define SM_GET_VALUE_WITHFLAG(tYpE,nAmE,vAlUe) std::tuple(true, vAlUe)

#define SM_DECLARE_ENUM(pRiVates) \
    enum PrivateIdx { \
        pRiVates(SM_GET_NAME) \
    };

#define SM_DECLARE_PRIVATE_GETTER() \
template<int M> \
auto get() const { \
   return std::get<M>(m_private_members); \
} \

#define SM_DECLARE_PRIVATE_SETTER() \
template<int M, typename ARG> \
void set(const ARG &value) { \
   std::get<M>(m_private_members)=value; \
} \

#define SM_DECLARE_PRIVATE_TUPLE(pRiVates) std::tuple<pRiVates(SM_GET_TYPE)> m_private_members{pRiVates(SM_GET_VALUE)};
#define SM_DECLARE_PRIVATE_TUPLE_WITHFLAG(pRiVates) std::tuple<pRiVates(SM_GET_TYPE_WITHFLAG)> m_private_members{pRiVates(SM_GET_VALUE_WITHFLAG)};

#define SM_DECLARE_PRIVATE_MEMBERS(pRiVates) \
    public: \
        SM_DECLARE_ENUM(pRiVates) \
    private: \
        SM_DECLARE_PRIVATE_TUPLE(pRiVates)

#define SM_DECLARE_PRIVATE_MEMBERS_WITHFLAG(pRiVates) \
    public: \
        SM_DECLARE_ENUM(pRiVates) \
    private: \
        SM_DECLARE_PRIVATE_TUPLE_WITHFLAG(pRiVates)


#define SM_DECLARE_PRIVATE_MEMBER_ITER() \
   auto begin() { \
      return sm::fn::tuple_iterator{ m_private_members, 0 }; \
   } \
   auto end() { \
      return sm::fn::tuple_iterator{ m_private_members, std::tuple_size_v<decltype(m_private_members)> }; \
   } \
\
   auto operator[](std::size_t i) { \
      return sm::fn::runtime_get(m_private_members, i); \
   } \

