#pragma once

#include <optional>

template<typename Type>
using Maybe = std::optional<Type>;
constexpr auto Nothing = std::nullopt;
