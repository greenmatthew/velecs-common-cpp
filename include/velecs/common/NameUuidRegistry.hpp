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
#include <memory>

namespace velecs::common {

/// @class NameUuidRegistry
/// @brief A dual-key registry that stores unique_ptr items accessible by both string name and UUID.
///
/// Provides efficient lookups by name while maintaining persistent UUID identifiers for serialization.
/// Specifically designed for std::unique_ptr<T> ownership semantics.
/// 
/// @tparam T Type of items to store in unique_ptr wrappers in the registry
/// @code
/// NameUuidRegistry<ActionProfile> profiles;
/// 
/// // Add items
/// auto uuid = profiles.Add("PlayerProfile", std::make_unique<ActionProfile>(...));
/// auto [item, uuid2] = profiles.Emplace("AIProfile", constructorArg1, constructorArg2);
/// 
/// // Retrieve by name or UUID
/// ActionProfile* profile = nullptr;
/// if (profiles.TryGetRef("PlayerProfile", profile)) { /* use profile */ }
/// if (profiles.TryGetRef(uuid, profile)) { /* use profile */ }
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

    /// @brief Default destructor.
    ~NameUuidRegistry() = default;

    // Public Methods

    /// @brief Adds a unique_ptr item to the registry with the given name
    /// @param name Unique name for the item
    /// @param item unique_ptr to store (will be moved)
    /// @return UUID assigned to the item for future lookups
    /// @throws std::runtime_error if name already exists
    Uuid Add(const std::string& name, std::unique_ptr<T> item)
    {
        auto uuid = Uuid::GenerateRandom();
        
        auto [nameIt, nameInserted] = _nameToUuid.try_emplace(name, uuid);
        if (!nameInserted)
        {
            throw std::runtime_error("Name '" + name + "' already exists.");
        }
        
        try
        {
            _items[uuid] = std::move(item);
            return uuid;
        }
        catch (...)
        {
            _nameToUuid.erase(nameIt);
            throw;
        }
    }

    /// @brief Constructs a subclass item in-place in the registry with the given name
    /// @tparam U The specific subclass type to construct (must inherit from T)
    /// @tparam Args Constructor argument types for U
    /// @param name Unique name for the item
    /// @param args Arguments to forward to U's constructor
    /// @return Pair containing reference to the constructed item (as U&) and its UUID
    /// @throws std::runtime_error if name already exists
    template<typename U, typename... Args>
    std::pair<U&, Uuid> EmplaceAs(const std::string& name, Args&&... args) {
        static_assert(std::is_base_of_v<T, U>, "Type U must be type T or inherit from type T.");
        
        auto uuid = Uuid::GenerateRandom();
        
        auto [nameIt, nameInserted] = _nameToUuid.try_emplace(name, uuid);
        if (!nameInserted)
        {
            throw std::runtime_error("Name '" + name + "' already exists.");
        }
        
        try
        {
            auto itemPtr = std::make_unique<U>(std::forward<Args>(args)...);
            U& itemRef = *itemPtr; // Get reference before moving (keep as U&)
            
            auto [itemIt, itemInserted] = _items.try_emplace(uuid, std::move(itemPtr));
            return { itemRef, uuid };
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
    /// @return Pair containing reference to the constructed item and its UUID
    /// @throws std::runtime_error if name already exists
    template<typename... Args>
    std::pair<T&, Uuid> Emplace(const std::string& name, Args&&... args)
    {
        return EmplaceAs<T>(name, std::forward<Args>(args)...);
    }

    /// @brief Attempts to retrieve a raw pointer by UUID
    /// @param uuid UUID of the item to retrieve
    /// @param outItem Reference to store raw pointer if found
    /// @return true if item was found, false otherwise
    bool TryGetRef(const Uuid& uuid, T*& outItem) const
    {
        auto it = _items.find(uuid);
        if (it != _items.end())
        {
            outItem = it->second.get();
            return true;
        }
        return false;
    }

    /// @brief Attempts to retrieve a raw pointer and name by UUID
    /// @param uuid UUID of the item to retrieve
    /// @param outItem Reference to store raw pointer if found
    /// @param outName Reference to store the name if found
    /// @return true if item was found, false otherwise
    /// @note This operation is O(n) as it requires searching through name mappings
    bool TryGetRef(const Uuid& uuid, T*& outItem, std::string& outName) const
    {
        auto itemIt = _items.find(uuid);
        if (itemIt != _items.end())
        {
            // Need to find the name by searching for the UUID value
            for (const auto& [name, nameUuid] : _nameToUuid)
            {
                if (nameUuid == uuid)
                {
                    outItem = itemIt->second.get();
                    outName = name;
                    return true;
                }
            }
        }
        return false;
    }

    /// @brief Attempts to retrieve a raw pointer by name
    /// @param name Name of the item to retrieve
    /// @param outItem Reference to store raw pointer if found
    /// @return true if item was found, false otherwise
    bool TryGetRef(const std::string& name, T*& outItem) const
    {
        auto it = _nameToUuid.find(name);
        if (it != _nameToUuid.end()) {
            auto uuid = it->second;
            return TryGetRef(uuid, outItem);
        }
        return false;
    }

    /// @brief Attempts to retrieve a raw pointer and UUID by name
    /// @param name Name of the item to retrieve
    /// @param outItem Reference to store raw pointer if found
    /// @param outUuid Reference to store the UUID if found
    /// @return true if item was found, false otherwise
    bool TryGetRef(const std::string& name, T*& outItem, Uuid& outUuid) const
    {
        auto it = _nameToUuid.find(name);
        if (it != _nameToUuid.end())
        {
            outUuid = it->second;
            return TryGetRef(outUuid, outItem);
        }
        return false;
    }

    /// @brief Attempts to retrieve the UUID for a given name
    /// @param name Name to look up
    /// @param outUuid Reference to store the UUID if found
    /// @return true if name was found, false otherwise
    bool TryGetUuid(const std::string& name, Uuid& outUuid) const
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
    bool TryGetName(const Uuid& uuid, std::string& outName) const
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

    /// @brief Removes an item from the registry by UUID
    /// @param uuid UUID of the item to remove
    /// @return true if item was found and removed, false otherwise
    bool Remove(const Uuid& uuid)
    {
        auto itemIt = _items.find(uuid);
        if (itemIt != _items.end())
        {
            // Find and remove the name mapping
            for (auto nameIt = _nameToUuid.begin(); nameIt != _nameToUuid.end(); ++nameIt)
            {
                if (nameIt->second == uuid)
                {
                    _nameToUuid.erase(nameIt);
                    break;
                }
            }
            
            // Remove the item
            _items.erase(itemIt);
            return true;
        }
        return false;
    }

    /// @brief Removes an item from the registry by name
    /// @param name Name of the item to remove
    /// @return true if item was found and removed, false otherwise
    bool Remove(const std::string& name)
    {
        auto nameIt = _nameToUuid.find(name);
        if (nameIt != _nameToUuid.end())
        {
            auto uuid = nameIt->second;
            _nameToUuid.erase(nameIt);
            _items.erase(uuid);
            return true;
        }
        return false;
    }

    /// @brief Clears all items from the registry
    void Clear()
    {
        _items.clear();
        _nameToUuid.clear();
    }

    /// @brief Gets the number of items in the registry
    /// @return Number of items currently stored
    size_t Size() const { return _items.size(); }

    /// @brief Checks if the registry is empty
    /// @return true if no items are stored, false otherwise
    bool Empty() const { return _items.empty(); }

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    /// @brief Storage for items indexed by UUID
    std::unordered_map<Uuid, std::unique_ptr<T>> _items;
    
    /// @brief Mapping from string names to their corresponding UUIDs
    std::unordered_map<std::string, Uuid> _nameToUuid;

    // Private Methods
};

} // namespace velecs::common
