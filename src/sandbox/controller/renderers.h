#ifndef RENDERERS_H
#define RENDERERS_H
#include "../app.h"

extern void RendererDepth(GUI::Context* UI, Renderers::Depth* Base);
extern void RendererLighting(GUI::Context* UI, Renderers::Lighting* Base);
extern void RendererEnvironment(GUI::Context* UI, Renderers::Environment* Base);
extern void RendererSSR(GUI::Context* UI, Renderers::SSR* Base);
extern void RendererSSAO(GUI::Context* UI, Renderers::SSAO* Base);
extern void RendererSSDO(GUI::Context* UI, Renderers::SSDO* Base);
extern void RendererBloom(GUI::Context* UI, Renderers::Bloom* Base);
extern void RendererDoF(GUI::Context* UI, Renderers::DoF* Base);
extern void RendererTone(GUI::Context* UI, Renderers::Tone* Base);
extern void RendererGlitch(GUI::Context* UI, Renderers::Glitch* Base);
#endif