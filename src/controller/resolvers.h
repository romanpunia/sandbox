#ifndef RESOLVERS_H
#define RESOLVERS_H
#include "../core/globals.h"

extern void resolve_resource(gui::ielement& target, const string& name, const std::function<void(const string&)>& callback, bool changed);
extern void resolve_entity(gui::ielement& target, const string& name, const std::function<void(entity*)>& callback, bool changed);
extern void resolve_texture_2d(gui::context* ui, const string& id, bool assigned, const std::function<void(texture_2d*)>& callback, bool changed);
extern void resolve_key_code(gui::context* ui, const string& id, key_map* output, bool changed);
extern bool resolve_color4(gui::context* ui, const string& id, vitex::trigonometry::vector4* output);
extern bool resolve_color3(gui::context* ui, const string& id, vitex::trigonometry::vector3* output);
extern void resolve_model(gui::context* ui, const string& id, components::model* output, bool changed);
extern void resolve_skin(gui::context* ui, const string& id, components::skin* output, bool changed);
extern void resolve_soft_body(gui::context* ui, const string& id, components::soft_body* output, bool changed);
extern void resolve_rigid_body(gui::context* ui, const string& id, components::rigid_body* output, bool changed);
extern void resolve_slider_constraint(gui::context* ui, const string& id, components::slider_constraint* output, bool ghost, bool linear, bool changed);
extern void resolve_skin_animator(gui::context* ui, const string& id, components::skin_animator* output, bool changed);
extern void resolve_key_animator(gui::context* ui, const string& id, components::key_animator* output, bool changed);
extern void resolve_audio_source(gui::context* ui, const string& id, components::audio_source* output, bool changed);
extern void resolve_scriptable(gui::context* ui, const string& id, components::scriptable* output, bool changed);
#endif