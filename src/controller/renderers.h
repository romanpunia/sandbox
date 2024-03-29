#ifndef RENDERERS_H
#define RENDERERS_H
#include "../core/globals.h"

extern void RendererLighting(GUI::Context* UI, Renderers::Lighting* Base);
extern void RendererSSR(GUI::Context* UI, Renderers::SSR* Base);
extern void RendererSSGI(GUI::Context* UI, Renderers::SSGI* Base);
extern void RendererSSAO(GUI::Context* UI, Renderers::SSAO* Base);
extern void RendererMotionBlur(GUI::Context* UI, Renderers::MotionBlur* Base);
extern void RendererBloom(GUI::Context* UI, Renderers::Bloom* Base);
extern void RendererDoF(GUI::Context* UI, Renderers::DoF* Base);
extern void RendererTone(GUI::Context* UI, Renderers::Tone* Base);
extern void RendererGlitch(GUI::Context* UI, Renderers::Glitch* Base);
#endif