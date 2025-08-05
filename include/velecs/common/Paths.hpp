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
#include <optional>

namespace velecs::common {

/// @class Paths
/// @brief Centralized path management system for the Velecs engine.
///
/// Provides access to important project directories after initialization.
/// Must be initialized once before use. Uses SDL to reliably determine executable location.
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

    /// @brief Initializes the path system using SDL to determine executable location
    /// @param company Company or developer name for persistent data organization
    /// @param appTitle Application title for persistent data organization
    /// @throws std::runtime_error if already initialized or SDL fails to determine paths
    static void Init(const std::string& company, const std::string& appTitle);

    /// @brief Checks if the path system has been initialized
    /// @return True if initialized, false otherwise
    static bool IsInitialized();

    /// @brief Cross-platform environment variable retrieval
    /// @param name Name of the environment variable to retrieve
    /// @return Optional string containing the variable value, or nullopt if not found
    /// @details Uses secure _dupenv_s on Windows, standard getenv on other platforms
    static std::optional<std::string> GetEnvironmentVariable(const std::string& name);

    // ----------------- Application directory -----------------

    /// @brief Gets the project root directory (directory containing the executable)
    /// @return Absolute path to the project directory
    /// @throws std::runtime_error if not initialized
    static const std::filesystem::path& ProjectDir();

    /// @brief Gets the assets directory path
    /// @return Absolute path to the assets directory (ProjectDir/assets)
    /// @throws std::runtime_error if not initialized
    static const std::filesystem::path& AssetsDir();

    // ----------------- Persistent Application directory -----------------

    /// @brief Gets the persistent data directory for application data storage
    /// @return Absolute path to persistent data directory
    /// @throws std::runtime_error if not initialized
    /// @details Windows: %USERPROFILE%/AppData/LocalLow/{COMPANY NAME}/{APP TITLE}
    ///          Unix-like: ~/.config/{COMPANY NAME}/{APP TITLE}
    static const std::filesystem::path& PersistentDataDir();

protected:
    // Protected Fields

    // Protected Methods

private:
    // Private Fields

    static bool _initialized;

    static std::filesystem::path _projectDir;
    static std::filesystem::path _assetsDir;

    static std::filesystem::path _persistentDataDir;

    // Private Methods

    /// @brief Throws exception if not initialized
    /// @throws std::runtime_error if not initialized
    static void CheckIfInitialized();

    /// @brief Creates and returns the platform-specific persistent data directory path
    /// @param company Company name for directory organization
    /// @param appTitle Application title for directory organization
    /// @return Absolute path to the created persistent data directory
    /// @throws std::runtime_error if unable to determine user directories
    static std::filesystem::path CreateAndGetPersistentDirPath(const std::string& company, const std::string& appTitle);
};

} // namespace velecs::common
