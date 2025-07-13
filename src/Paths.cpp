/// @file    Paths.cpp
/// @author  Matthew Green
/// @date    2025-07-13 15:19:19
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#include "velecs/common/Paths.hpp"

namespace velecs::common {

// Private Fields
std::filesystem::path Paths::_exePath;
std::filesystem::path Paths::_projectDir;
std::filesystem::path Paths::_assetsDir;

// Public Methods

void Paths::Initialize(const std::string& argv0)
{
    if (!_exePath.empty())
    {
        throw std::runtime_error("Paths::Initialize() called multiple times. Initialize should only be called once.");
    }

    _exePath = std::filesystem::absolute(argv0);
    _projectDir = _exePath.parent_path();
    _assetsDir = _projectDir / "assets";
}

const std::filesystem::path& Paths::ExecutablePath()
{
    CheckIfInitialized();
    return _exePath;
}

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

bool Paths::IsInitialized()
{
    return !_exePath.empty();
}

// Private Methods

void Paths::CheckIfInitialized()
{
    if (_exePath.empty())
    {
        throw std::runtime_error("Paths not initialized. Call Paths::Initialize() first.");
    }
}

} // namespace velecs::common
