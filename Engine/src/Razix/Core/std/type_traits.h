#ifndef _RZ_TYPE_TRAITS_H_
#define _RZ_TYPE_TRAITS_H_

namespace Razix {
    
    // this remote_refence is to convert a Type value from either value/ref/&& to a &&, used for custom move semantics
    // It removes & or && from a type so you can get the underlying base type
    template <typename T> struct rz_remove_reference {using type  = T;};
    template <typename T> struct rz_remove_reference<T&> {using type  = T;};
    template <typename T> struct rz_remove_reference<T&&> {using type  = T;};
}

#endif // _RZ_TYPE_TRAITS_H_
