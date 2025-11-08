#pragma once

namespace Razix {
    namespace Debug {

#ifndef RAZIX_GOLD_MASTER
    #define RZ_DEBUG_NAME_TAG_S_ARG        const RZString& bufferName
    #define RZ_DEBUG_NAME_TAG_E_ARG        , const RZString& bufferName
    #define RZ_DEBUG_NAME_TAG_F_ARG        const RZString &&bufferName,
    #define RZ_DEBUG_NAME_TAG_M_ARG        , const RZString &&bufferName,
    #define RZ_DEBUG_F_ARG_NAME            bufferName,
    #define RZ_DEBUG_S_ARG_NAME            bufferName
    #define RZ_DEBUG_E_ARG_NAME            , bufferName
    #define RZ_DEBUG_NAME_TAG_STR_E_ARG(x) , x
    #define RZ_DEBUG_NAME_TAG_STR_F_ARG(x) x,
    #define RZ_DEBUG_NAME_TAG_STR_S_ARG(x) x
    #define RZ_DEBUG_NAME_TAG_STR_M_ARG(x) , x,
#else
    #define RZ_DEBUG_NAME_TAG_S_ARG
    #define RZ_DEBUG_NAME_TAG_E_ARG
    #define RZ_DEBUG_NAME_TAG_F_ARG
    #define RZ_DEBUG_NAME_TAG_M_ARG
    #define RZ_DEBUG_F_ARG_NAME
    #define RZ_DEBUG_S_ARG_NAME
    #define RZ_DEBUG_E_ARG_NAME
    #define RZ_DEBUG_NAME_TAG_STR_E_ARG(x)
    #define RZ_DEBUG_NAME_TAG_STR_F_ARG(x)
    #define RZ_DEBUG_NAME_TAG_STR_S_ARG(x)
    #define RZ_DEBUG_NAME_TAG_STR_M_ARG(x)
#endif

    }    // namespace Debug
}    // namespace Razix
