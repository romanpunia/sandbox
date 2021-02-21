#ifndef RESOLVERS_H
#define RESOLVERS_H
#include "../app.h"

extern void ResolveResource(GUI::IElement& Target, const std::string& Name, const std::function<void(const std::string&)>& Callback);
extern void ResolveEntity(GUI::IElement& Target, const std::string& Name, const std::function<void(Entity*)>& Callback);
extern void ResolveTexture2D(GUI::Context* UI, const std::string& Id, bool Assigned, const std::function<void(Texture2D*)>& Callback);
extern void ResolveKeyCode(GUI::Context* UI, const std::string& Id, KeyMap* Output);
extern bool ResolveColor4(GUI::Context* UI, const std::string& Id, Vector4* Output);
extern bool ResolveColor3(GUI::Context* UI, const std::string& Id, Vector3* Output);
extern void ResolveModel(GUI::Context* UI, const std::string& Id, Components::Model* Output);
extern void ResolveSkin(GUI::Context* UI, const std::string& Id, Components::Skin* Output);
extern void ResolveSoftBody(GUI::Context* UI, const std::string& Id, Components::SoftBody* Output);
extern void ResolveRigidBody(GUI::Context* UI, const std::string& Id, Components::RigidBody* Output);
extern void ResolveSliderConstraint(GUI::Context* UI, const std::string& Id, Components::SliderConstraint* Output, bool Ghost, bool Linear);
extern void ResolveSkinAnimator(GUI::Context* UI, const std::string& Id, Components::SkinAnimator* Output);
extern void ResolveKeyAnimator(GUI::Context* UI, const std::string& Id, Components::KeyAnimator* Output);
extern void ResolveAudioSource(GUI::Context* UI, const std::string& Id, Components::AudioSource* Output);
extern void ResolveScriptable(GUI::Context* UI, const std::string& Id, Components::Scriptable* Output);
#endif