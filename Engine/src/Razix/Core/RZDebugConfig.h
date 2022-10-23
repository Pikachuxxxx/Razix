#pragma once

namespace Razix {
    namespace Debug {

#ifdef RAZIX_DEBUG
    #define NAME_TAG_F      const std::string& bufferName
    #define NAME_ARG_NAME_F bufferName
    #define NAME_TAG      , const std::string& bufferName
    #define NAME_ARG_NAME , bufferName
    #define NAME_TAG_STR(x) x
#else
    #define NAME_TAG
    #define NAME_ARG_NAME
    #define NAME_TAG_F
    #define NAME_ARG_NAME_F
    #define NAME_TAG_STR(x)
#endif

    }    // namespace Debug
}    // namespace Razix
