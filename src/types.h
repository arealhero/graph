#pragma once

#include <concepts>
#include <optional>
#include <type_traits>

struct TNothing {
  explicit constexpr TNothing(int) noexcept {}
};

constexpr TNothing Nothing{0};

template <typename Type> class TMaybe {
public:
  static_assert(!std::is_same_v<std::remove_cv_t<Type>, TNothing>,
                "TMaybe<TNothing> is forbidden");
  static_assert(!std::is_reference_v<Type>,
                "TMaybe with reference type is forbidden");
  static_assert(std::is_destructible_v<Type>,
                "TMaybe's type should be destructible");

  constexpr TMaybe(TNothing) : Optional(std::nullopt) {}

  template <std::convertible_to<Type> T>
  constexpr TMaybe(const T &value) : Optional(value) {}

  template <std::convertible_to<Type> T>
  constexpr TMaybe(Type &&value) : Optional(std::move(value)) {}

  constexpr bool Defined() const noexcept { return Optional.has_value(); }

  constexpr const Type &Value() const & { return Optional.value(); }
  constexpr Type &Value() & { return Optional.value(); }
  constexpr const Type &&Value() const && { return Optional.value(); }
  constexpr Type &&Value() && { return Optional.value(); }

  constexpr operator Type() { return Optional.value(); }

private:
  std::optional<Type> Optional = std::nullopt;
};

template <typename Type>
constexpr bool operator==(const TMaybe<Type> &maybe, const TNothing &) {
  return !maybe.Defined();
}
template <typename Type>
constexpr bool operator==(const TNothing &nothing, const TMaybe<Type> &maybe) {
  return maybe == nothing;
}
template <typename Type>
constexpr bool operator!=(const TMaybe<Type> &maybe, const TNothing &) {
  return maybe.Defined();
}
template <typename Type>
constexpr bool operator!=(const TNothing &nothing, const TMaybe<Type> &maybe) {
  return maybe != nothing;
}
