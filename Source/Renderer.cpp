#include "Renderer.h"

Renderer::Renderer(SDL_Surface* screen)
{
    this->screen = screen;
    width = screen->w;
    height = screen->h;
}

Renderer::Shader::~Shader() {}


mat4 Renderer::projection;
mat4 Renderer::viewPort;
mat4 Renderer::modelView;
float Renderer::depth;