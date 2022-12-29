#pragma once

#include <string_view>

namespace Razix {
    namespace Graphics {
        namespace FrameGraph {

            // https://www.bfilipek.com/2016/02/notes-on-c-sfinae.html
            // https://en.cppreference.com/w/cpp/types/enable_if
            // https://levelup.gitconnected.com/c-detection-idiom-explained-5cc7207a0067

            template<typename T, typename = void>
            struct has_CreateDesc : std::false_type
            {
            };
            template<typename T>
            struct has_CreateDesc<T, std::void_t<typename T::Desc>> : std::true_type
            {
            };
            template<typename T>
            inline constexpr bool has_CreateDesc_v = has_CreateDesc<T>::value;

            template<typename T>
            struct has_create
            {
                template<typename U>
                static constexpr std::false_type test(...)
                {
                    return {};
                }
                template<typename U>
                static constexpr auto test(U *u) ->
                    typename std::is_same<void,
                        decltype(u->create(typename T::Desc{},
                            std::declval<void *>()))>::type
                {
                    return {};
                }

                static constexpr bool value{test<T>(nullptr)};
            };
            template<typename T>
            inline constexpr bool has_create_v = has_create<T>::value;

            template<typename T>
            struct has_destroy
            {
                template<typename U>
                static constexpr std::false_type test(...)
                {
                    return {};
                }
                template<typename U>
                static constexpr auto test(U *u) ->
                    typename std::is_same<void,
                        decltype(u->destroy(typename T::Desc{},
                            std::declval<void *>()))>::type
                {
                    return {};
                }

                static constexpr bool value{test<T>(nullptr)};
            };
            template<typename T>
            inline constexpr bool has_destroy_v = has_destroy<T>::value;

            template<typename T>
            constexpr bool is_resource()
            {
                return std::is_default_constructible_v<T> &&
                       std::is_move_constructible_v<T> && has_CreateDesc_v<T> && has_create_v<T> &&
                       has_destroy_v<T>;
            }

#define ENFORCE_CONCEPT \
    template<typename T, std::enable_if_t<is_resource<T>(), bool> = true>
#define ENFORCE_CONCEPT_IMPL \
    template<typename T, std::enable_if_t<is_resource<T>(), bool>>

            template<typename T, typename = void>
            struct has_toString : std::false_type
            {
            };
            template<class T>
            struct has_toString<T, std::void_t<decltype(T::toString)>>
                : std::is_convertible<decltype(T::toString(typename T::Desc{})),
                      std::string_view>
            {
            };
        }    // namespace FrameGraph
    }        // namespace Graphics
}    // namespace Razix