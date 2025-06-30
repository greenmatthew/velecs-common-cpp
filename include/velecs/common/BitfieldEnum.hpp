/// @file    BitfieldEnum.hpp
/// @author  Matthew Green
/// @date    2025-06-30 14:07:46
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <cstdint>
#include <type_traits>

namespace velecs::common {

/// @class BitfieldEnum
/// @brief CRTP base class that provides bitfield operations for scoped enums
///
/// This template automatically provides all bitwise operators (|, &, ^, ~, |=, &=, ^=)
/// and helper functions for any enum class that inherits from it. The enum must have
/// an underlying integral type to work properly.
///
/// @tparam Derived The derived enum class type (CRTP pattern)
/// @tparam Underlying The underlying integral type of the enum (default: uint32_t)
///
/// @code
/// // Define your enum values
/// enum class MyFlags : uint32_t {
///     None    = 0,
///     Flag1   = 1 << 0,
///     Flag2   = 1 << 1,
///     Flag3   = 1 << 2,
///     All     = Flag1 | Flag2 | Flag3
/// };
///
/// // Enable bitfield operations by specializing the template
/// template<> struct EnableBitfieldEnum<MyFlags> : std::true_type {};
///
/// // Now you can use all bitfield operations
/// MyFlags flags = MyFlags::Flag1 | MyFlags::Flag2;
/// flags |= MyFlags::Flag3;
/// if (HasAnyFlag(flags, MyFlags::Flag1)) { /* ... */ }
/// @endcode
template<typename T>
struct EnableBitfieldEnum : std::false_type {};

// Helper to check if type should have bitfield operations
template<typename T>
constexpr bool is_bitfield_enum_v = EnableBitfieldEnum<T>::value;

} // namespace velecs::common

/// @brief Bitwise OR operator for combining enum flags
/// @tparam T Enum type that has EnableBitfieldEnum specialized
/// @param a First enum flag
/// @param b Second enum flag  
/// @return Combined enum flags
template<typename T>
constexpr std::enable_if_t<velecs::common::is_bitfield_enum_v<T>, T>
operator|(T a, T b) noexcept
{
    using underlying = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(a) | static_cast<underlying>(b));
}

/// @brief Bitwise OR assignment operator for combining enum flags
/// @tparam T Enum type that has EnableBitfieldEnum specialized
/// @param a Enum reference to modify
/// @param b Enum flag to combine with a
/// @return Reference to the modified enum a
template<typename T>
constexpr std::enable_if_t<velecs::common::is_bitfield_enum_v<T>, T&>
operator|=(T& a, T b) noexcept
{
    a = a | b;
    return a;
}

/// @brief Bitwise AND operator for testing enum flags
/// @tparam T Enum type that has EnableBitfieldEnum specialized
/// @param a First enum flag
/// @param b Second enum flag
/// @return Intersection of enum flags
template<typename T>
constexpr std::enable_if_t<velecs::common::is_bitfield_enum_v<T>, T>
operator&(T a, T b) noexcept
{
    using underlying = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(a) & static_cast<underlying>(b));
}

/// @brief Bitwise AND assignment operator for masking enum flags
/// @tparam T Enum type that has EnableBitfieldEnum specialized
/// @param a Enum reference to modify
/// @param b Enum flag to mask with
/// @return Reference to the modified enum a
template<typename T>
constexpr std::enable_if_t<velecs::common::is_bitfield_enum_v<T>, T&>
operator&=(T& a, T b) noexcept
{
    a = a & b;
    return a;
}

/// @brief Bitwise XOR operator for toggling enum flags
/// @tparam T Enum type that has EnableBitfieldEnum specialized
/// @param a First enum flag
/// @param b Second enum flag
/// @return XOR result of enum flags
template<typename T>
constexpr std::enable_if_t<velecs::common::is_bitfield_enum_v<T>, T>
operator^(T a, T b) noexcept
{
    using underlying = std::underlying_type_t<T>;
    return static_cast<T>(static_cast<underlying>(a) ^ static_cast<underlying>(b));
}

/// @brief Bitwise XOR assignment operator for toggling enum flags
/// @tparam T Enum type that has EnableBitfieldEnum specialized
/// @param a Enum reference to modify
/// @param b Enum flag to toggle
/// @return Reference to the modified enum a
template<typename T>
constexpr std::enable_if_t<velecs::common::is_bitfield_enum_v<T>, T&>
operator^=(T& a, T b) noexcept
{
    a = a ^ b;
    return a;
}

/// @brief Bitwise NOT operator for inverting enum flags
/// @tparam T Enum type that has EnableBitfieldEnum specialized
/// @param a Enum flag to invert
/// @return Inverted enum flags
template<typename T>
constexpr std::enable_if_t<velecs::common::is_bitfield_enum_v<T>, T>
operator~(T a) noexcept
{
    using underlying = std::underlying_type_t<T>;
    return static_cast<T>(~static_cast<underlying>(a));
}

/// @brief Check if any of the specified enum flags are set
/// @tparam T Enum type that has EnableBitfieldEnum specialized
/// @param value The current enum state to check
/// @param flags The enum flags to test for
/// @return true if any of the specified flags are set in value
template<typename T>
constexpr std::enable_if_t<velecs::common::is_bitfield_enum_v<T>, bool>
HasAnyFlag(T value, T flags) noexcept
{
    using underlying = std::underlying_type_t<T>;
    return (value & flags) != static_cast<T>(underlying{0});
}

/// @brief Check if all of the specified enum flags are set
/// @tparam T Enum type that has EnableBitfieldEnum specialized
/// @param value The current enum state to check
/// @param flags The enum flags that must all be present
/// @return true if all of the specified flags are set in value
template<typename T>
constexpr std::enable_if_t<velecs::common::is_bitfield_enum_v<T>, bool>
HasAllFlags(T value, T flags) noexcept
{
    return (value & flags) == flags;
}

/// @brief Check if the enum value represents no flags set
/// @tparam T Enum type that has EnableBitfieldEnum specialized
/// @param value The enum value to check
/// @return true if no flags are set (value equals zero)
template<typename T>
constexpr std::enable_if_t<velecs::common::is_bitfield_enum_v<T>, bool>
IsEmpty(T value) noexcept
{
    using underlying = std::underlying_type_t<T>;
    return static_cast<underlying>(value) == underlying{0};
}

/// @brief Get the underlying integral value of the enum
/// @tparam T Enum type that has EnableBitfieldEnum specialized
/// @param value The enum value
/// @return The underlying integral value
template<typename T>
constexpr std::enable_if_t<velecs::common::is_bitfield_enum_v<T>, std::underlying_type_t<T>>
ToUnderlying(T value) noexcept
{
    return static_cast<std::underlying_type_t<T>>(value);
}
