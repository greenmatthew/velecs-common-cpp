/// @file    Event.hpp
/// @author  Matthew Green
/// @date    2025-05-26 16:10:43
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <vector>
#include <functional>
#include <algorithm>

namespace velecs::common {

/// @class Event
/// @brief A lightweight event system that allows multiple callbacks to be registered and invoked together.
///
/// This template class provides a C#-like event system for C++, allowing multiple function callbacks
/// to be registered and triggered simultaneously. Uses a handle-based system for reliable callback removal.
/// 
/// @tparam Args Parameter types that will be passed to all registered callbacks
/// 
/// @code
/// // Example usage:
/// Event<> buttonClicked;                    // Event with no parameters
/// Event<int, float> valueChanged;          // Event with int and float parameters
/// 
/// // Register callbacks - store handles for removal
/// auto handle1 = buttonClicked += []() { std::cout << "Button clicked!" << std::endl; };
/// auto handle2 = valueChanged += [](int id, float value) { std::cout << "Value " << id << ": " << value << std::endl; };
/// 
/// // Remove specific callbacks
/// buttonClicked -= handle1;
/// valueChanged -= handle2;
/// 
/// // Trigger events
/// buttonClicked();           // Calls all registered callbacks
/// valueChanged(42, 3.14f);   // Calls all callbacks with parameters
/// @endcode
template<typename... Args>
class Event {
public:
    /// @brief Type alias for callback functions that can be registered with this event
    using Callback = std::function<void(Args...)>;
    
    /// @brief Handle type returned when registering callbacks, used for removal
    using Handle = size_t;

private:
    /// @brief Internal structure to store callback with its handle
    struct CallbackEntry {
        Handle handle;
        Callback callback;
    };

public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor. Creates an empty event with no registered callbacks.
    Event() = default;

    /// @brief Default destructor. Automatically clears all registered callbacks.
    ~Event() = default;

    // Public Methods

    /// @brief Adds a callback function to this event
    /// @param callback The function to be called when this event is invoked
    /// @return Handle that can be used to remove this specific callback later
    /// @note The callback will be stored and called in the order it was added
    Handle Add(const Callback& callback)
    {
        Handle handle = _nextHandle++;
        _callbacks.push_back({handle, callback});
        return handle;
    }

    /// @brief Adds a callback function to this event using operator overloading
    /// @param callback The function to be called when this event is invoked
    /// @return Handle that can be used to remove this specific callback later
    /// @note Equivalent to Add(callback). Provides C#-like syntax: auto handle = event += callback
    Handle operator+=(const Callback& callback)
    {
        return Add(callback);
    }

    /// @brief Removes a specific callback function from this event using its handle
    /// @param handle The handle returned when the callback was originally added
    /// @return Reference to this Event for method chaining
    /// @note If the handle is not found, this method has no effect
    Event& Remove(Handle handle)
    {
        _callbacks.erase(
            std::remove_if(_callbacks.begin(), _callbacks.end(),
                [handle](const CallbackEntry& entry) { 
                    return entry.handle == handle; 
                }),
            _callbacks.end()
        );
        return *this;
    }

    /// @brief Removes a specific callback function from this event using operator overloading
    /// @param handle The handle returned when the callback was originally added
    /// @return Reference to this Event for method chaining
    /// @note Equivalent to Remove(handle). Provides C#-like syntax: event -= handle
    Event& operator-=(Handle handle)
    {
        return Remove(handle);
    }
    
    /// @brief Removes all registered callback functions from this event
    /// @note After calling this method, invoking the event will have no effect until new callbacks are added
    void Clear()
    {
        _callbacks.clear();
        _nextHandle = 1; // Reset handle counter
    }
    
    /// @brief Invokes all registered callback functions with the provided arguments
    /// @param args Arguments to pass to each registered callback function
    /// @note Callbacks are called in the order they were registered. If a callback throws an exception,
    ///       subsequent callbacks will not be executed.
    void Invoke(Args... args) const
    {
        for (const auto& entry : _callbacks)
        {
            entry.callback(args...);
        }
    }
    
    /// @brief Invokes all registered callback functions using function call operator
    /// @param args Arguments to pass to each registered callback function
    /// @note Equivalent to Invoke(args...). Allows calling the event like a function: event(args...)
    void operator()(Args... args) const { Invoke(args...); }

    /// @brief Checks if this event has no registered callbacks
    /// @return true if no callbacks are registered, false otherwise
    bool Empty() const { return _callbacks.empty(); }

    /// @brief Gets the number of registered callbacks
    /// @return The number of callback functions currently registered with this event
    size_t Size() const { return _callbacks.size(); }

private:
    // Private Fields

    /// @brief Container storing all registered callback functions with their handles
    std::vector<CallbackEntry> _callbacks;
    
    /// @brief Counter for generating unique handles
    Handle _nextHandle = 1;

    // Private Methods
};

} // namespace velecs::common
