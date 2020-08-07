
// This file is part of the MDCII Game Engine.
// Copyright (C) 2020  Armin Schlegel
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef _GAMEWINDOW_H_
#define _GAMEWINDOW_H_

#include <iostream>

#include "SDL2/SDL.h"

#include "sdlgui/button.h"
#include "sdlgui/widget.h"

#include "menu/gamewindow.hpp"
#include "sdl2shared/sdl2shared.hpp"

using namespace sdlgui;

GameWindow::GameWindow(SDL_Renderer* renderer, SDL_Window* pwindow, const std::string& gamName, bool fullscreen)
    : Screen(pwindow, Vector2i(Scale::Instance()->GetScreenWidth(), Scale::Instance()->GetScreenHeight()), "Game")
    , renderer(renderer)
    , width(Scale::Instance()->GetScreenWidth())
    , height(Scale::Instance()->GetScreenHeight())
    , gamName(gamName)
    , fullscreen(fullscreen)
    , buildings(Buildings::Instance())
    , running(true)
    , gam(std::make_shared<GamParser>(gamName, false))
    , scale(Scale::Instance())
{
    std::cout << "Haeuser: " << buildings->GetBuildingsSize() << std::endl;
    {
        auto& button1 = wdg<Button>("Exit", [this] {
            std::cout << "Leaving game" << std::endl;
            this->running = false;
        });
        button1.setSize(sdlgui::Vector2i{ 100, 20 });
        button1.setPosition(width - 50, height - 30);
        controlWidgets.push_back(std::make_tuple(&button1, -50, -30));
    }
    performLayout(renderer);
    RedrawControlWidgets();
}

void GameWindow::CreateSpielbildschirm(uint32_t width, uint32_t height)
{
    // if (spielbildschirm && this->width == width && this->height == height)
    // {
    //   return;
    // }

    auto palette = Palette::Instance();

    s8 = sdl2::make_shared(SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0));
    SDL_SetPaletteColors(s8.get()->format->palette, palette->GetSDLColors(), 0, palette->size());
    fb = std::make_shared<FramebufferPal8>(width, height, 0, static_cast<uint8_t*>(s8.get()->pixels), (uint32_t)s8.get()->pitch);
    spielbildschirm = std::make_shared<Spielbildschirm>(*fb);
}

void GameWindow::RedrawControlWidgets()
{
    for (auto& w : controlWidgets)
    {
        auto widget = std::get<0>(w);
        int scaleRightBorder = scale->GetScreenWidth();
        int scaleUpperBorder = scale->GetScreenHeight();
        widget->setPosition(Vector2i{ scaleRightBorder + std::get<1>(w), scaleUpperBorder + std::get<2>(w) });
    }
}

void GameWindow::Handle()
{
    // auto palette = Palette::Instance();

    // s8 = SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0);
    // SDL_SetPaletteColors(s8->format->palette, palette->GetSDLColors(), 0, palette->size());

    std::ifstream f;
    f.open(gamName, std::ios_base::in | std::ios_base::binary);
    Welt welt = Welt(f);
    f.close();
    // FramebufferPal8 fb(width, height, 0, static_cast<uint8_t*>(s8->pixels), (uint32_t)s8->pitch);

    // Spielbildschirm spielbildschirm(fb);
    CreateSpielbildschirm(Scale::Instance()->GetScreenWidth(), Scale::Instance()->GetScreenHeight());
    spielbildschirm->zeichne_bild(welt, 0, 0);

    try
    {
        auto finalSurface = SDL_ConvertSurfaceFormat(s8.get(), SDL_PIXELFORMAT_RGB888, 0);
        auto texture = SDL_CreateTextureFromSurface(renderer, finalSurface);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        this->drawAll();
        SDL_RenderPresent(renderer);

        Fps fps;
        const Uint8* keystate = SDL_GetKeyboardState(NULL);

        SDL_Event e;
        while (running)
        {
            while (SDL_PollEvent(&e) != 0)
            {
                switch (e.type)
                {
                    case SDL_QUIT:
                        running = false;
                        break;
                    case SDL_USEREVENT:
                        break;
                    case SDL_KEYDOWN:
                        if (e.key.keysym.sym == SDLK_F2)
                        {
                            spielbildschirm->kamera->setze_vergroesserung(0);
                        }
                        if (e.key.keysym.sym == SDLK_F3)
                        {
                            spielbildschirm->kamera->setze_vergroesserung(1);
                        }
                        if (e.key.keysym.sym == SDLK_F4)
                        {
                            spielbildschirm->kamera->setze_vergroesserung(2);
                        }
                        if (e.key.keysym.sym == SDLK_x)
                        {
                            spielbildschirm->kamera->rechts_drehen();
                        }
                        if (e.key.keysym.sym == SDLK_y)
                        {
                            spielbildschirm->kamera->links_drehen();
                        }
                        if (e.key.keysym.sym == SDLK_F11)
                        {
                            fullscreen = !fullscreen;
                            scale->ToggleFullscreen();
                        }
                        break;
                    case SDL_WINDOWEVENT:
                        if (e.window.event == SDL_WINDOWEVENT_RESIZED)
                        {
                            scale->Update();
                            CreateSpielbildschirm(Scale::Instance()->GetScreenWidth(), Scale::Instance()->GetScreenHeight());
                            continue;
                        }
                        break;
                    default:
                        break;
                }
                this->onEvent(e);
            }
            RedrawControlWidgets();
            int x, y;
            SDL_GetMouseState(&x, &y);

            if (keystate[SDL_SCANCODE_LEFT] || (fullscreen && x == 0))
            {
                spielbildschirm->kamera->nach_links();
            }
            if (keystate[SDL_SCANCODE_RIGHT] || (fullscreen && x == width - 1))
            {
                spielbildschirm->kamera->nach_rechts();
            }
            if (keystate[SDL_SCANCODE_UP] || (fullscreen && y == 0))
            {
                spielbildschirm->kamera->nach_oben();
            }
            if (keystate[SDL_SCANCODE_DOWN] || (fullscreen && y == height - 1))
            {
                spielbildschirm->kamera->nach_unten();
            }

            welt.simulationsschritt();
            spielbildschirm->zeichne_bild(welt, x, y);
            finalSurface = SDL_ConvertSurfaceFormat(s8.get(), SDL_PIXELFORMAT_RGB888, 0);
            texture = SDL_CreateTextureFromSurface(renderer, finalSurface);
            SDL_FreeSurface(finalSurface);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            this->drawAll();
            SDL_RenderPresent(renderer);
            SDL_DestroyTexture(texture);
            fps.Next();
        }
    }
    catch (const std::runtime_error& e)
    {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
#if defined(_WIN32)
        MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
#else
        std::cerr << error_msg << std::endl;
#endif
    }
}

#endif