#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "../app.h"

extern void ComponentModel(GUI::Context* UI, Components::Model* Base);
extern void ComponentSkin(GUI::Context* UI, Components::Skin* Base);
extern void ComponentEmitter(GUI::Context* UI, Components::Emitter* Base);
extern void ComponentDecal(GUI::Context* UI, Components::Decal* Base);
extern void ComponentSoftBody(GUI::Context* UI, Components::SoftBody* Base);
extern void ComponentSkinAnimator(GUI::Context* UI, Components::SkinAnimator* Base);
extern void ComponentKeyAnimator(GUI::Context* UI, Components::KeyAnimator* Base);
extern void ComponentEmitterAnimator(GUI::Context* UI, Components::EmitterAnimator* Base);
extern void ComponentRigidBody(GUI::Context* UI, Components::RigidBody* Base);
extern void ComponentAcceleration(GUI::Context* UI, Components::Acceleration* Base);
extern void ComponentSliderConstraint(GUI::Context* UI, Components::SliderConstraint* Base);
extern void ComponentFreeLook(GUI::Context* UI, Components::FreeLook* Base);
extern void ComponentFly(GUI::Context* UI, Components::Fly* Base);
extern void ComponentAudioSource(GUI::Context* UI, Components::AudioSource* Base);
extern void ComponentAudioListener(GUI::Context* UI, Components::AudioListener* Base);
extern void ComponentPointLight(GUI::Context* UI, Components::PointLight* Base);
extern void ComponentSpotLight(GUI::Context* UI, Components::SpotLight* Base);
extern void ComponentLineLight(GUI::Context* UI, Components::LineLight* Base);
extern void ComponentSurfaceLight(GUI::Context* UI, Components::SurfaceLight* Base);
extern void ComponentCamera(GUI::Context* UI, Components::Camera* Base);
extern void ComponentScriptable(GUI::Context* UI, Components::Scriptable* Base);
#endif