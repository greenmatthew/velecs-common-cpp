/// @file    Paths.hpp
/// @author  Matthew Green
/// @date    2025-07-13 15:17:01
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

#include <filesystem>
#include <stdexcept>

namespace velecs::common {

/// @class Paths
/// @brief Centralized path management system for the Velecs engine.
///
/// Provides access to important project directories after initialization.
/// Must be initialized once with the executable path before use.
class Paths {
public:
    // Enums

    // Public Fields

    // Constructors and Destructors

    /// @brief Default constructor.
    Paths() = default;

    /// @brief Default deconstructor.
    ~Paths() = default;

    // Public Methods

    /// @brief Initializes the path system with the executable path
    /// @param argv0 Path to the application executable (typically argv[0])
    /// @throws std::runtime_error if already initialized
    static void Initialize(const std::string& argv0);

    /// @brief Gets the absolute path to the binary executable
    /// @return Absolute path to the executable file
    /// @throws std::runtime_error if not initialized
    static const std::filesystem::path& ExecutablePath();

    /// @brief Gets the project root directory (directory containing the executable)
    /// @return Absolute path to the project directory
    /// @throws std::runtime_error if not initialized
    static const std::filesystem::path& ProjectDir();

    /// @brief Gets the assets directory path
    /// @return Absolute path to the assets directory (ProjectDir/assets)
    /// @throws std::runtime_error if not initialized
    static const std::filesystem::path& AssetsDir();

    /// @brief Checks if the path system has been initialized
    /// @return True if initialized, false otherwise
    static bool IsInitialized();

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    static std::filesystem::path _exePath;
    static std::filesystem::path _projectDir;
    static std::filesystem::path _assetsDir;

    // Private Methods

    /// @brief Throws exception if not initialized
    /// @throws std::runtime_error if not initialized
    static void CheckIfInitialized();
};

} // namespace velecs::common
