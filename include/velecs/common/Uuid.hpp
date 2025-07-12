/// @file    Uuid.hpp
/// @author  Matthew Green
/// @date    2025-06-23 16:33:27
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <uuid.h> // `#include <stduuid/include/uuid.h>` does not work unfortunately.

#include <string>
#include <iostream>
#include <optional>
#include <functional>

namespace velecs::common {

/// @class Uuid
/// @brief Brief description.
///
/// Rest of description.
class Uuid {
public:
    // Enums

    // Public Fields

    /// @brief Static constant representing an invalid/null UUID (all zeros)
    static const Uuid INVALID;

    // Constructors and Destructors

    /// @brief Default constructor is deleted - use factory methods instead
    /// @note Forces explicit creation through GenerateRandom() or FromString()
    Uuid() = delete;

    /// @brief Copy constructor
    /// @param other The UUID to copy from
    inline Uuid(const Uuid& other) : _uuid(other._uuid) {}

    /// @brief Move constructor
    /// @param other The UUID to move from
    inline Uuid(Uuid&& other) noexcept : _uuid(std::move(other._uuid)) {}

    /// @brief Default destructor
    ~Uuid() = default;

    // Public Methods

    /// @brief Generate a new random UUID (version 4)
    /// @return A new random UUID following RFC 4122 standards
    /// @note This method is thread-safe and uses proper entropy seeding
    static Uuid GenerateRandom();

    /// @brief Generate sequential UUIDs for testing/debugging purposes
    /// @return A UUID with an incrementing counter in the least significant bits
    /// @note Thread-safe using atomic counter. Format: 00000000-0000-0000-XXXX-XXXXXXXXXXXX
    /// @warning Not cryptographically secure - only use for testing/debugging!
    static Uuid GenerateSequential();

    /// @brief Generate a deterministic UUID from a numeric seed
    /// @param seed The numeric seed for deterministic generation (32-bit)
    /// @return A UUID that's always the same for the same seed value
    /// @note Uses Mersenne Twister engine seeded with the provided value
    static Uuid GenerateFromSeed(uint32_t seed);

    /// @brief Generate a deterministic UUID from a string seed (name-based UUID v5)
    /// @param seed The string seed (any length, e.g., "MyGameWorld123")
    /// @return A UUID that's always the same for the same seed string
    /// @note Uses SHA-1 hashing with velecs namespace. Same input always produces same output
    static Uuid GenerateFromString(const std::string& seed);

    /// @brief Generate a deterministic UUID by hashing string to numeric seed
    /// @param seed The string seed to hash into a numeric value
    /// @return A UUID generated from the hashed numeric seed
    /// @note Hashes string to size_t, then truncates to uint32_t for GenerateFromSeed()
    /// @warning Different from GenerateFromString() - may have hash collisions
    static Uuid GenerateFromStringHash(const std::string& seed);

    /// @brief Parse a UUID from a string representation
    /// @param uuid The UUID string in canonical format (e.g., "550e8400-e29b-41d4-a716-446655440000")
    /// @return A new UUID object, or INVALID if parsing failed
    /// @note Accepts both uppercase and lowercase hex digits, with or without hyphens
    static std::optional<Uuid> FromString(const std::string& uuid);

    /// @brief Copy assignment operator
    /// @param other The UUID to copy from
    /// @return Reference to this UUID
    Uuid& operator=(const Uuid& other);

    /// @brief Move assignment operator
    /// @param other The UUID to move from
    /// @return Reference to this UUID
    Uuid& operator=(Uuid&& other) noexcept;

    /// @brief Equality comparison operator
    /// @param other The UUID to compare against
    /// @return true if both UUIDs are identical
    inline bool operator==(const Uuid& other) const { return _uuid == other._uuid; }

    /// @brief Inequality comparison operator
    /// @param other The UUID to compare against
    /// @return true if UUIDs are different
    inline bool operator!=(const Uuid& other) const { return _uuid != other._uuid; }

    /// @brief Check if this UUID is valid (not the INVALID constant)
    /// @return true if this UUID is not the all-zeros invalid UUID
    /// @note A "valid" UUID here means it's not the INVALID constant, not format validation
    inline bool IsValid() const { return *this != INVALID; }

    /// @brief Convert UUID to its canonical string representation
    /// @return UUID string in lowercase with hyphens (e.g., "550e8400-e29b-41d4-a716-446655440000")
    inline std::string ToString() const { return uuids::to_string(_uuid); }

    /// @brief Get hash value for use in unordered containers
    /// @return Hash value suitable for std::unordered_map, std::unordered_set, etc.
    /// @note This enables using Uuid as a key in hash-based containers
    inline size_t GetHashCode() const { return std::hash<uuids::uuid>{}(_uuid); }

    /// @brief Stream output operator for easy printing
    /// @param os The output stream to write to
    /// @param uuid The UUID to output
    /// @return Reference to the output stream for chaining
    /// @note Outputs the UUID in canonical string format
    friend std::ostream& operator<<(std::ostream& os, const Uuid& uuid)
    {
        return os << uuid.ToString();
    }

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    /// @brief The underlying UUID implementation
    uuids::uuid _uuid;

    // Private Methods

    /// @brief Private constructor for internal use
    /// @param uuid The underlying UUID object to wrap
    /// @note Only accessible to factory methods and internal implementation
    inline explicit Uuid(const uuids::uuid& uuid) : _uuid(uuid) {}
};

} // namespace velecs::common

/// @brief Specialization of std::hash for Uuid to enable use in unordered containers
/// 
/// This specialization allows Uuid objects to be used as keys in std::unordered_map,
/// std::unordered_set, and other hash-based standard library containers.
///
/// Example usage:
/// @code
/// std::unordered_map<velecs::common::Uuid, std::string> idToName;
/// std::unordered_set<velecs::common::Uuid> uniqueIds;
/// @endcode
namespace std {
    template<>
    struct hash<velecs::common::Uuid> {
        /// @brief Hash function operator
        /// @param uuid The UUID to hash
        /// @return Hash value for the UUID
        /// @note noexcept specification ensures this won't throw exceptions
        size_t operator()(const velecs::common::Uuid& uuid) const noexcept
        {
            return uuid.GetHashCode();
        }
    };
}
