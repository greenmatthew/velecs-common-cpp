/// @file    Exceptions.hpp
/// @author  Matthew Green
/// @date    2025-07-23 12:43:01
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <stdexcept>
#include <string>
#include <sstream>

namespace velecs::common {

class NotImplementedException : public std::logic_error {
public:
    // Constructor with default message
    NotImplementedException()
        : std::logic_error("Functionality not implemented") {}
    
    // Constructor with custom message
    explicit NotImplementedException(const std::string& what_arg)
        : std::logic_error(what_arg) {}
};

} // namespace velecs::common
