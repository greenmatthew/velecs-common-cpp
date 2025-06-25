/// @file    NameUuidRegistry.hpp
/// @author  Matthew Green
/// @date    2025-06-24 14:06:16
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include "velecs/common/Uuid.hpp"

#include <string>
#include <unordered_map>
#include <stdexcept>
#include <utility>

namespace velecs::common {

/// @class NameUuidRegistry
/// @brief A dual-key registry that stores items accessible by both string name and UUID.
///
/// Provides efficient lookups by name while maintaining persistent UUID identifiers for serialization.
/// 
/// @tparam T Type of items to store in the registry
/// @code
/// NameUuidRegistry<std::shared_ptr<ActionProfile>> profiles;
/// 
/// // Add items
/// auto uuid = profiles.Add("PlayerProfile", profile);
/// auto [item, uuid2] = profiles.Emplace("AIProfile", "AI", constructorKey);
/// 
/// // Retrieve by name or UUID
/// ActionProfile profile;
/// if (profiles.TryGet("PlayerProfile", profile)) { /* use profile */ }
/// if (profiles.TryGet(uuid, profile)) { /* use profile */ }
/// @endcode
template<typename T>
class NameUuidRegistry {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    NameUuidRegistry() = default;

    // Explicitly delete copy operations to prevent unique_ptr copy attempts
    NameUuidRegistry(const NameUuidRegistry&) = delete;
    NameUuidRegistry& operator=(const NameUuidRegistry&) = delete;

    // Allow move operations 
    NameUuidRegistry(NameUuidRegistry&&) = default;
    NameUuidRegistry& operator=(NameUuidRegistry&&) = default;

    /// @brief Default deconstructor.
    ~NameUuidRegistry() = default;

    // Public Methods

    /// @brief Adds an item to the registry with the given name
    /// @param name Unique name for the item
    /// @param value Item to store (will be moved)
    /// @return UUID assigned to the item for future lookups
    /// @throws std::runtime_error if name already exists
    Uuid Add(const std::string& name, T value)
    {
        auto uuid = Uuid::GenerateRandom();
        
        auto [nameIt, nameInserted] = _nameToUuid.try_emplace(name, uuid);
        if (!nameInserted)
        {
            throw std::runtime_error("Name '" + name + "' already exists.");
        }
        
        try
        {
            _items[uuid] = std::move(value);
            return uuid;
        }
        catch (...)
        {
            _nameToUuid.erase(nameIt);
            throw;
        }
    }

    /// @brief Constructs an item in-place in the registry with the given name
    /// @tparam Args Constructor argument types for T
    /// @param name Unique name for the item
    /// @param args Arguments to forward to T's constructor
    /// @return Pair containing the constructed item and its UUID
    /// @throws std::runtime_error if name already exists
    template<typename... Args>
    std::pair<T, Uuid> Emplace(const std::string& name, Args&&... args) {
        auto uuid = Uuid::GenerateRandom();
        
        auto [nameIt, nameInserted] = _nameToUuid.try_emplace(name, uuid);
        if (!nameInserted)
        {
            throw std::runtime_error("Name '" + name + "' already exists.");
        }
        
        try
        {
            auto [itemIt, itemInserted] = _items.try_emplace(uuid, std::forward<Args>(args)...);
            return { itemIt->second, uuid };
        }
        catch (...)
        {
            _nameToUuid.erase(nameIt);
            throw;
        }
    }

    /// @brief Attempts to retrieve an item by UUID
    /// @param uuid UUID of the item to retrieve
    /// @param outItem Reference to store the item if found
    /// @return true if item was found, false otherwise
    bool TryGet(const Uuid& uuid, T& outItem)
    {
        auto it = _items.find(uuid);
        if (it != _items.end())
        {
            outItem = it->second;
            return true;
        }
        
        return false;
    }

    /// @brief Attempts to retrieve an item and its name by UUID
    /// @param uuid UUID of the item to retrieve
    /// @param outItem Reference to store the item if found
    /// @param outName Reference to store the name if found
    /// @return true if item was found, false otherwise
    bool TryGet(const Uuid& uuid, T& outItem, std::string& outName)
    {
        auto itemIt = _items.find(uuid);
        if (itemIt != _items.end())
        {
            // Need to find the name by searching for the UUID value
            for (const auto& [name, nameUuid] : _nameToUuid)
            {
                if (nameUuid == uuid)
                {
                    outItem = itemIt->second;
                    outName = name;
                    return true;
                }
            }
        }
        return false;
    }

    /// @brief Attempts to retrieve an item by name
    /// @param name Name of the item to retrieve
    /// @param outItem Reference to store the item if found
    /// @return true if item was found, false otherwise
    bool TryGet(const std::string& name, T& outItem)
    {
        auto it = _nameToUuid.find(name);
        if (it != _nameToUuid.end()) {
            auto uuid = it->second;
            return TryGet(uuid, outItem);
        }
        return false;
    }

    /// @brief Attempts to retrieve an item and its UUID by name
    /// @param name Name of the item to retrieve
    /// @param outItem Reference to store the item if found
    /// @param outUuid Reference to store the UUID if found
    /// @return true if item was found, false otherwise
    bool TryGet(const std::string& name, T& outItem, Uuid& outUuid)
    {
        auto it = _nameToUuid.find(name);
        if (it != _nameToUuid.end())
        {
            outUuid = it->second;
            return TryGet(outUuid, outItem);
        }
        return false;
    }

    /// @brief Attempts to retrieve the UUID for a given name
    /// @param name Name to look up
    /// @param outUuid Reference to store the UUID if found
    /// @return true if name was found, false otherwise
    bool TryGetUuid(const std::string& name, Uuid& outUuid)
    {
        auto it = _nameToUuid.find(name);
        if (it != _nameToUuid.end())
        {
            outUuid = it->second;
            return true;
        }
        return false;
    }

    /// @brief Attempts to retrieve the name for a given UUID
    /// @param uuid UUID to look up
    /// @param outName Reference to store the name if found
    /// @return true if UUID was found, false otherwise
    /// @note This operation is O(n) as it requires searching through name mappings
    bool TryGetName(const Uuid& uuid, std::string& outName)
    {
        for (const auto& [name, nameUuid] : _nameToUuid)
        {
            if (nameUuid == uuid)
            {
                outName = name;
                return true;
            }
        }
        return false;
    }

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    /// @brief Storage for items indexed by UUID
    std::unordered_map<Uuid, T> _items;
    
    /// @brief Mapping from string names to their corresponding UUIDs
    std::unordered_map<std::string, Uuid> _nameToUuid;

    // Private Methods
};

} // namespace velecs::common
