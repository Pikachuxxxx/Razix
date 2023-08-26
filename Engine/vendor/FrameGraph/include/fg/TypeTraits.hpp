#pragma once

#include <string_view>

// https://www.bfilipek.com/2016/02/notes-on-c-sfinae.html

#if _HAS_CXX20
#  include <concepts>

template <typename T>
concept Virtualizable = requires(T t) {
  requires std::is_default_constructible_v<T> &&
    std::is_move_constructible_v<T>;

  typename T::Desc;
  { t.create(typename T::Desc{}, (void *)nullptr) } -> std::same_as<void>;
  { t.destroy(typename T::Desc{}, (void *)nullptr) } -> std::same_as<void>;
};

#  define _VIRTUALIZABLE_CONCEPT template <Virtualizable T>
#  define _VIRTUALIZABLE_CONCEPT_IMPL _VIRTUALIZABLE_CONCEPT

template <typename T>
concept has_preRead = requires(T t) {
  { t.preRead(typename T::Desc{}, 0u, (void *)nullptr) } -> std::same_as<void>;
};
template <typename T>
concept has_preWrite = requires(T t) {
  { t.preWrite(typename T::Desc{}, 0u, (void *)nullptr) } -> std::same_as<void>;
};

template <typename T>
concept has_toString = requires() {
  { T::toString(typename T::Desc{}) } -> std::convertible_to<std::string_view>;
};
#else
// https://en.cppreference.com/w/cpp/types/enable_if
// https://levelup.gitconnected.com/c-detection-idiom-explained-5cc7207a0067

template <typename T, typename = void> struct has_Desc : std::false_type {};
template <typename T>
struct has_Desc<T, std::void_t<typename T::Desc>> : std::true_type {};
template <typename T> inline constexpr bool has_Desc_v = has_Desc<T>::value;

template <typename T> struct has_create {
  template <typename U> static constexpr std::false_type test(...) {
    return {};
  }
  template <typename U>
  static constexpr auto test(U *u) ->
    typename std::is_same<void,
                          decltype(u->create(typename T::Desc{},
                                             std::declval<void *>()))>::type {
    return {};
  }

  static constexpr bool value{test<T>(nullptr)};
};
template <typename T> inline constexpr bool has_create_v = has_create<T>::value;

template <typename T> struct has_destroy {
  template <typename U> static constexpr std::false_type test(...) {
    return {};
  }
  template <typename U>
  static constexpr auto test(U *u) ->
    typename std::is_same<void,
                          decltype(u->destroy(typename T::Desc{},
                                              std::declval<void *>()))>::type {
    return {};
  }

  static constexpr bool value{test<T>(nullptr)};
};
template <typename T>
inline constexpr bool has_destroy_v = has_destroy<T>::value;

template <typename T> constexpr bool is_resource() {
  return std::is_default_constructible_v<T> &&
         std::is_move_constructible_v<T> && has_Desc_v<T> && has_create_v<T> &&
         has_destroy_v<T>;
}

#  define _VIRTUALIZABLE_CONCEPT                                               \
    template <typename T, std::enable_if_t<is_resource<T>(), bool> = true>
#  define _VIRTUALIZABLE_CONCEPT_IMPL                                          \
    template <typename T, std::enable_if_t<is_resource<T>(), bool>>

template <typename T> struct has_preRead {
  template <typename U> static constexpr std::false_type test(...) {
    return {};
  }
  template <typename U>
  static constexpr auto test(U *u) ->
    typename std::is_same<void,
                          decltype(u->preRead(typename T::Desc{}, 0u,
                                              std::declval<void *>()))>::type {
    return {};
  }

  static constexpr bool value{test<T>(nullptr)};
};
template <typename T> struct has_preWrite {
  template <typename U> static constexpr std::false_type test(...) {
    return {};
  }
  template <typename U>
  static constexpr auto test(U *u) ->
    typename std::is_same<void,
                          decltype(u->preWrite(typename T::Desc{}, 0u,
                                               std::declval<void *>()))>::type {
    return {};
  }

  static constexpr bool value{test<T>(nullptr)};
};

template <typename T, typename = void> struct has_toString : std::false_type {};
template <class T>
struct has_toString<T, std::void_t<decltype(T::toString)>>
    : std::is_convertible<decltype(T::toString(typename T::Desc{})),
                          std::string_view> {};

#endif
