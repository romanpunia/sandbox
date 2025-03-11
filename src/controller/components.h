#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "../core/globals.h"

extern void component_model(gui::context* ui, components::model* base, bool changed);
extern void component_skin(gui::context* ui, components::skin* base, bool changed);
extern void component_emitter(gui::context* ui, components::emitter* base, bool changed);
extern void component_decal(gui::context* ui, components::decal* base, bool changed);
extern void component_soft_body(gui::context* ui, components::soft_body* base, bool changed);
extern void component_skin_animator(gui::context* ui, components::skin_animator* base, bool changed);
extern void component_key_animator(gui::context* ui, components::key_animator* base, bool changed);
extern void component_emitter_animator(gui::context* ui, components::emitter_animator* base, bool changed);
extern void component_rigid_body(gui::context* ui, components::rigid_body* base, bool changed);
extern void component_acceleration(gui::context* ui, components::acceleration* base, bool changed);
extern void component_slider_constraint(gui::context* ui, components::slider_constraint* base, bool changed);
extern void component_free_look(gui::context* ui, components::free_look* base, bool changed);
extern void component_fly(gui::context* ui, components::fly* base, bool changed);
extern void component_audio_source(gui::context* ui, components::audio_source* base, bool changed);
extern void component_audio_listener(gui::context* ui, components::audio_listener* base, bool changed);
extern void component_point_light(gui::context* ui, components::point_light* base, bool changed);
extern void component_spot_light(gui::context* ui, components::spot_light* base, bool changed);
extern void component_line_light(gui::context* ui, components::line_light* base, bool changed);
extern void component_surface_light(gui::context* ui, components::surface_light* base, bool changed);
extern void component_illuminator(gui::context* ui, components::illuminator* base, bool changed);
extern void component_camera(gui::context* ui, components::camera* base, bool changed);
extern void component_scriptable(gui::context* ui, components::scriptable* base, bool changed);
#endif