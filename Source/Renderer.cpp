#include "Renderer.h"

mat4 Renderer::projection;
mat4 Renderer::viewPort;
mat4 Renderer::modelView;
float Renderer::depth;


Renderer::Shader::~Shader() {}

Renderer::Renderer(SDL_Surface* screen)
{
    this->screen = screen;
    width = screen->w;
    height = screen->h;
}
