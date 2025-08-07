/// @file    Context.hpp
/// @author  Matthew Green
/// @date    2025-08-07 12:57:17
/// 
/// @section LICENSE
/// 
/// Copyright (c) 2025 Matthew Green - All rights reserved
/// Unauthorized copying of this file, via any medium is strictly prohibited
/// Proprietary and confidential

#pragma once

namespace velecs::common {

/// @brief Safely casts a void* context to a specific context type.
/// @tparam ContextType The target context type to cast to.
/// @param context The void* context pointer passed from processing methods.
/// @return A pointer to the context cast to the specified type.
/// @details This helper function provides type-safe access to engine-specific context data
///          within scene and system processing methods. The context type is determined by the engine
///          and can vary between different processing phases (e.g., PhysicsContext for
///          ProcessPhysics, RenderContext for ProcessGUI, MaterialManager for OnEnter).
/// @note It is the caller's responsibility to ensure the void* context actually points
///       to an object of ContextType. Incorrect casting will result in undefined behavior.
/// 
/// @code
/// // In Scene::OnEnter
/// void OnEnter(void* context) override {
///     auto* sceneCtx = Context<SceneContext>(context);
///     MaterialManager* matManager = sceneCtx->GetMaterialManager();
///     matManager->CreateMaterial(...);
///     // ... use scene-specific context data
/// }
/// @endcode
template<typename ContextType>
inline ContextType* Context(void* context)
{
    return static_cast<ContextType*>(context);
}

/// @brief Safely casts a const void* context to a specific const context type.
/// @tparam ContextType The target context type to cast to.
/// @param context The const void* context pointer passed from processing methods.
/// @return A const pointer to the context cast to the specified type.
/// @details This is the const version of the Context function for use in const methods
///          or when you need read-only access to the context data.
/// @note It is the caller's responsibility to ensure the void* context actually points
///       to an object of ContextType. Incorrect casting will result in undefined behavior.
/// 
/// @code
/// // In System::ProcessPhysics
/// void ProcessPhysics(void* context) override {
///     auto* physicsCtx = Context<PhysicsContext>(context);
///     const float deltaTime = physicsCtx->GetDeltaTime();
///     // ... use physics-specific context data
/// }
/// @endcode
template<typename ContextType>
inline const ContextType* Context(const void* context)
{
    return static_cast<const ContextType*>(context);
}

} // namespace velecs::common
