#ifndef _RZ_STD_UTILITIY_H_
#define _RZ_STD_UTILITIY_H_

#include "Razix/Core/std/type_traits.h"

namespace Razix {

    // custom razix move semantics, get the underlying base type and force cast to r-value ref
    template<typename T>
    constexpr typename rz_remove_reference<T>::type&& rz_move(T&& arg) noexcept
    {
        return static_cast<typename rz_remove_reference<T>::type&&>(arg);
    }
}    // namespace Razix

#endif    // _RZ_STD_UTILITIY_H_
