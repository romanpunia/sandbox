#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "../core/globals.h"

extern void ComponentModel(GUI::Context* UI, Components::Model* Base, bool Changed);
extern void ComponentSkin(GUI::Context* UI, Components::Skin* Base, bool Changed);
extern void ComponentEmitter(GUI::Context* UI, Components::Emitter* Base, bool Changed);
extern void ComponentDecal(GUI::Context* UI, Components::Decal* Base, bool Changed);
extern void ComponentSoftBody(GUI::Context* UI, Components::SoftBody* Base, bool Changed);
extern void ComponentSkinAnimator(GUI::Context* UI, Components::SkinAnimator* Base, bool Changed);
extern void ComponentKeyAnimator(GUI::Context* UI, Components::KeyAnimator* Base, bool Changed);
extern void ComponentEmitterAnimator(GUI::Context* UI, Components::EmitterAnimator* Base, bool Changed);
extern void ComponentRigidBody(GUI::Context* UI, Components::RigidBody* Base, bool Changed);
extern void ComponentAcceleration(GUI::Context* UI, Components::Acceleration* Base, bool Changed);
extern void ComponentSliderConstraint(GUI::Context* UI, Components::SliderConstraint* Base, bool Changed);
extern void ComponentFreeLook(GUI::Context* UI, Components::FreeLook* Base, bool Changed);
extern void ComponentFly(GUI::Context* UI, Components::Fly* Base, bool Changed);
extern void ComponentAudioSource(GUI::Context* UI, Components::AudioSource* Base, bool Changed);
extern void ComponentAudioListener(GUI::Context* UI, Components::AudioListener* Base, bool Changed);
extern void ComponentPointLight(GUI::Context* UI, Components::PointLight* Base, bool Changed);
extern void ComponentSpotLight(GUI::Context* UI, Components::SpotLight* Base, bool Changed);
extern void ComponentLineLight(GUI::Context* UI, Components::LineLight* Base, bool Changed);
extern void ComponentSurfaceLight(GUI::Context* UI, Components::SurfaceLight* Base, bool Changed);
extern void ComponentIlluminator(GUI::Context* UI, Components::Illuminator* Base, bool Changed);
extern void ComponentCamera(GUI::Context* UI, Components::Camera* Base, bool Changed);
extern void ComponentScriptable(GUI::Context* UI, Components::Scriptable* Base, bool Changed);
#endif