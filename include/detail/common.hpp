#pragma once

#include <type_traits>

namespace detail::common {

template <template <typename...> class C, class I>
struct is_instance : std::false_type {};

template <template <typename...> class C, class I>
struct is_instance<C, C<I>> : std::false_type {};

template <template <typename...> class C, class I>
inline constexpr bool is_instance_v{is_instance<C, C<I>>::value};

template <class T>
concept is_vector_like = requires(T vec) {
  typename T::value_type;
  typename T::iterator;

  vec.size();

  vec.begin();
  vec.end();

  vec.push_back(std::declval<typename T::value_type>());
  vec.emplace_back(std::declval<typename T::value_type>());

  vec.pop_back();
};

} // namespace detail::common
