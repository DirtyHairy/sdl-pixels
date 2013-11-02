#include <iostream>
#include <sstream>

#include <SDL_gfxPrimitives.h>

#include "viewport.h"
#include "global.h"
#include "setup.h"

using namespace std;

Viewport::Viewport (GlobalData* global) : resx(global->setup->resx),
    resy(global->setup->resy), verbose(global->setup->verbose),
    shadow(global->setup->shadow), global(global)
{
    stringstream ss;

    Uint32 flags = SDL_HWSURFACE | SDL_DOUBLEBUF |
        (global->setup->fullscreen ? SDL_FULLSCREEN : 0);
    bpp = SDL_VideoModeOK (resx, resy, global->setup->bpp, flags);
    if (!bpp) throw EFatal ("requested resolution not available");
    real_surface = SDL_SetVideoMode (resx, resy, bpp, flags);
    if (!real_surface) throw EFatal ("unable to setup video");

    if (shadow) {
        SDL_PixelFormat* f = real_surface->format;
        surface = SDL_CreateRGBSurface (SDL_HWSURFACE, resx, resy, bpp,
            f->Rmask, f->Gmask, f->Bmask, f->Amask);
        if (!surface) throw EFatal ("unable to create shadow surface");
    }
    else surface = real_surface;
    bytesPerPixel = surface->format->BytesPerPixel;
    pitch = surface->pitch;
    clear ();

    fullscreen = real_surface->flags && SDL_FULLSCREEN;
    doublebuffered = real_surface->flags && SDL_DOUBLEBUF;
    hardware = real_surface->flags && SDL_HWSURFACE;

    if (verbose) {
        ss.str("");
        ss << "created a " << resx << "x" << resy << "@" << bpp << " surface";
        global->logger->Log(ss.str());

        if (hardware) global->logger->Log("managed to obtain a hardware surface");
        if (doublebuffered) global->logger->Log("surface is doublebuffered");
        if (shadow) global->logger->Log("allocated a shadow buffer");
    }

    gfxPrimitivesSetFont (0, 0, 0);
    SDL_ShowCursor (SDL_DISABLE);
    SDL_EnableKeyRepeat (0, 0);
}

void Viewport::flip () {
    if (shadow) SDL_BlitSurface (surface, 0, real_surface, 0);
    SDL_Flip (real_surface);
}

Viewport::~Viewport () {
    if (shadow) SDL_FreeSurface (surface);
    SDL_Quit ();
}

void Viewport::clear () {
    lock ();
    SDL_FillRect (surface, 0, mapColor (0, 0, 0));
    unlock ();
}

Uint32 Viewport::mapColor (Uint8 r, Uint8 g, Uint8 b) const {
    return SDL_MapRGB (surface->format, r, g, b);
}


