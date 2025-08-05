/// @file    Paths.cpp
/// @author  Matthew Green
/// @date    2025-08-05 13:59:29
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/common/Paths.hpp"

#include "SDL3/SDL_filesystem.h"

namespace velecs::common {

// Public Fields

// Constructors and Destructors

// Public Methods

void Paths::Init(const std::string& company, const std::string& appTitle)
{
    if (_initialized)
        throw std::runtime_error("Paths::Init() called multiple times. Init should only be called once.");

    _projectDir = std::string{SDL_GetBasePath()};
    _assetsDir = _projectDir / "assets";

    _persistentDataDir = CreateAndGetPersistentDirPath(company, appTitle);

    _initialized = true;
}

bool Paths::IsInitialized()
{
    return _initialized;
}

std::optional<std::string> Paths::GetEnvironmentVariable(const std::string& name)
{
#ifdef _WIN32
    // Use Microsoft's secure version
    char* value = nullptr;
    size_t len = 0;
    
    if (_dupenv_s(&value, &len, name.c_str()) == 0 && value != nullptr)
    {
        std::string result(value);
        free(value);  // Must free the allocated memory
        return result;
    }
    
    return std::nullopt;  // Not found or error
#else
    // Use standard C function for other platforms
    const char* value = std::getenv(name.c_str());
    return value ? std::string(value) : std::nullopt;
#endif
}

// ----------------- Application directory -----------------

const std::filesystem::path& Paths::ProjectDir()
{
    CheckIfInitialized();
    return _projectDir;
}

const std::filesystem::path& Paths::AssetsDir()
{
    CheckIfInitialized();
    return _assetsDir;
}

// ----------------- Persistent Application directory -----------------

const std::filesystem::path& Paths::PersistentDataDir()
{
    CheckIfInitialized();
    return _persistentDataDir;
}

// Protected Fields

// Protected Methods

// Private Fields

bool Paths::_initialized{false};

std::filesystem::path Paths::_projectDir;
std::filesystem::path Paths::_assetsDir;

std::filesystem::path Paths::_persistentDataDir;

// Private Methods

void Paths::CheckIfInitialized()
{
    if (!IsInitialized())
        throw std::runtime_error("Paths not initialized. Call Paths::Init() first.");
}

std::filesystem::path Paths::CreateAndGetPersistentDirPath(const std::string& company, const std::string& appTitle)
{
    std::filesystem::path persistentDataDir;
#ifdef _WIN32
    auto userProfile = GetEnvironmentVariable("USERPROFILE");
    if (!userProfile.has_value())
    {
        throw std::runtime_error("Unable to get USERPROFILE environment variable");
    }
    
    persistentDataDir = std::filesystem::path(userProfile.value()) / "AppData" / "LocalLow" / company / appTitle;
#else
    auto home = GetEnvironmentVariable("HOME");
    if (!home.has_value())
    {
        throw std::runtime_error("Unable to get HOME environment variable");  
    }
    
    persistentDataDir = std::filesystem::path(home.value()) / ".config" / company / appTitle;
#endif

    // Create directory if it doesn't exist
    std::filesystem::create_directories(persistentDataDir);
    
    return persistentDataDir;
}

} // namespace velecs::common
