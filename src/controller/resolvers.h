#ifndef RESOLVERS_H
#define RESOLVERS_H
#include "../core/globals.h"

extern void ResolveResource(GUI::IElement& Target, const std::string& Name, const std::function<void(const std::string&)>& Callback, bool Changed);
extern void ResolveEntity(GUI::IElement& Target, const std::string& Name, const std::function<void(Entity*)>& Callback, bool Changed);
extern void ResolveTexture2D(GUI::Context* UI, const std::string& Id, bool Assigned, const std::function<void(Texture2D*)>& Callback, bool Changed);
extern void ResolveKeyCode(GUI::Context* UI, const std::string& Id, KeyMap* Output, bool Changed);
extern bool ResolveColor4(GUI::Context* UI, const std::string& Id, Vector4* Output);
extern bool ResolveColor3(GUI::Context* UI, const std::string& Id, Vector3* Output);
extern void ResolveModel(GUI::Context* UI, const std::string& Id, Components::Model* Output, bool Changed);
extern void ResolveSkin(GUI::Context* UI, const std::string& Id, Components::Skin* Output, bool Changed);
extern void ResolveSoftBody(GUI::Context* UI, const std::string& Id, Components::SoftBody* Output, bool Changed);
extern void ResolveRigidBody(GUI::Context* UI, const std::string& Id, Components::RigidBody* Output, bool Changed);
extern void ResolveSliderConstraint(GUI::Context* UI, const std::string& Id, Components::SliderConstraint* Output, bool Ghost, bool Linear, bool Changed);
extern void ResolveSkinAnimator(GUI::Context* UI, const std::string& Id, Components::SkinAnimator* Output, bool Changed);
extern void ResolveKeyAnimator(GUI::Context* UI, const std::string& Id, Components::KeyAnimator* Output, bool Changed);
extern void ResolveAudioSource(GUI::Context* UI, const std::string& Id, Components::AudioSource* Output, bool Changed);
extern void ResolveScriptable(GUI::Context* UI, const std::string& Id, Components::Scriptable* Output, bool Changed);
#endif