/// @file    Uuid.cpp
/// @author  Matthew Green
/// @date    2025-06-23 16:53:39
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/common/Uuid.hpp"

namespace velecs::common {

// Public Fields

const Uuid Uuid::INVALID = Uuid(uuids::uuid{});

// Constructors and Destructors

// Public Methods

Uuid Uuid::GenerateRandom()
{
    // Thread-local static generator for thread safety and performance
    static thread_local auto generator = []() {
        // Properly seed the generator with high-quality entropy
        std::random_device rd;
        auto seed_data = std::array<int, std::mt19937::state_size>{};
        std::generate(std::begin(seed_data), std::end(seed_data), std::ref(rd));
        std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
        std::mt19937 engine(seq);
        
        return uuids::uuid_random_generator{engine};
    }();
    
    // Generate and wrap the UUID
    return Uuid{generator()};
}

Uuid Uuid::GenerateSequential()
{
    // Thread-safe atomic counter for sequential IDs
    static std::atomic<uint32_t> counter{1};
    
    // Get next sequential number
    uint32_t id = counter.fetch_add(1);
    
    // Create UUID with sequential number in the least significant bits
    // Format: 00000000-0000-0000-XXXX-XXXXXXXXXXXX where X is the counter
    std::array<uint8_t, 16> bytes{};
    
    // Put the counter in the last 4 bytes (big-endian) since it's now 32-bit
    for (int i = 0; i < 4; ++i) {
        bytes[15 - i] = static_cast<uint8_t>((id >> (i * 8)) & 0xFF);
    }
    
    // Create UUID from byte array
    uuids::uuid uuid{bytes.begin(), bytes.end()};
    return Uuid{uuid};
}

Uuid Uuid::GenerateFromSeed(uint32_t seed)
{
    // Create a seeded Mersenne Twister engine
    std::mt19937 engine(seed);
    
    // Create UUID generator with seeded engine
    uuids::uuid_random_generator generator{engine};
    
    // Generate and wrap the UUID
    return Uuid{generator()};
}

Uuid Uuid::GenerateFromString(const std::string& seed)
{
    static auto generator = []() {
        // Use a fixed namespace UUID for your engine
        // This ensures your UUIDs don't collide with other systems using name-based UUIDs
        auto velecs_namespace = uuids::uuid::from_string("76656c65-6373-4000-8000-000000000000").value();
        //                                                ^^^^^^^^ ^^^^ ^    ^
        //                                                "vele"   "cs" |    |
        //                                                          "version"|
        //                                                               "variant"
        return uuids::uuid_name_generator{velecs_namespace};
    }();
    
    return Uuid{generator(seed)};
}

Uuid Uuid::GenerateFromStringHash(const std::string& seed)
{
    // Hash the string to get a numeric seed
    std::hash<std::string> hasher;
    size_t hashValue = hasher(seed);
    
    // Convert to uint32_t - explicitly acknowledge potential truncation on 64-bit systems
    uint32_t numericSeed = static_cast<uint32_t>(hashValue);
    
    // Use existing GenerateFromSeed method
    return GenerateFromSeed(numericSeed);
}

std::optional<Uuid> Uuid::FromString(const std::string& uuid)
{
    auto opt = uuids::uuid::from_string(uuid);
    if (opt.has_value())
    {
        return Uuid{opt.value()};  // Create Uuid from the unwrapped value
    }
    return std::nullopt;  // Return empty optional
}

Uuid& Uuid::operator=(const Uuid& other)
{
    // If not self, assign from internal uuid
    if (this != &other) _uuid = other._uuid;
    return *this;
}

Uuid& Uuid::operator=(Uuid&& other) noexcept
{
    // If not self, move internal uuid
    if (this != &other) _uuid = std::move(other._uuid);
    return *this;
}

// Protected Fields

// Protected Methods

// Private Fields

// Private Methods

} // namespace velecs::common
