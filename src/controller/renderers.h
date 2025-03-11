#ifndef RENDERERS_H
#define RENDERERS_H
#include "../core/globals.h"

extern void renderer_lighting(gui::context* ui, renderers::lighting* base);
extern void renderer_sslr(gui::context* ui, renderers::local_reflections* base);
extern void renderer_ssli(gui::context* ui, renderers::local_illumination* base);
extern void renderer_ssla(gui::context* ui, renderers::local_ambient* base);
extern void renderer_motion_blur(gui::context* ui, renderers::motion_blur* base);
extern void renderer_bloom(gui::context* ui, renderers::bloom* base);
extern void renderer_dof(gui::context* ui, renderers::depth_of_field* base);
extern void renderer_tone(gui::context* ui, renderers::tone* base);
extern void renderer_glitch(gui::context* ui, renderers::glitch* base);
#endif