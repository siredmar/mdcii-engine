
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

#include <iostream>

#include "SDL2/SDL.h"

#include "sdlgui/layout.h"
#include "sdlgui/messagedialog.h"
#include "sdlgui/screen.h"
#include "sdlgui/texturebutton.h"
#include "sdlgui/textureview.h"
#include "sdlgui/window.h"

#include "menu/mainmenu.hpp"
#include "menu/singleplayerwindow.hpp"

using namespace sdlgui;
MainMenu::MainMenu(SDL_Renderer* renderer, std::shared_ptr<Basegad> basegad, SDL_Window* pwindow, int width, int height, bool fullscreen)
    : Screen(pwindow, Vector2i(width, height), "Game", false, true, true)
    , renderer(renderer)
    , buildings(Buildings::Instance())
    , basegad(basegad)
    , width(width)
    , height(height)
    , fullscreen(fullscreen)
    , triggerSinglePlayer(false)
    , pwindow(pwindow)
    , files(Files::Instance())
    , quit(false)
    , scale(Scale::Instance())
{
    std::cout << "Basegad: " << basegad->GetGadgetsSize() << std::endl;
    BshReader bsh_leser(files->FindPathForFile("toolgfx/start.bsh"));
    BshImageToSDLTextureConverter converter(renderer);

    int scaleLeftBorder = (scale->GetScreenSize().width - bsh_leser.GetBshImage(0).width) / 2;
    int scaleUpperBorder = (scale->GetScreenSize().height - bsh_leser.GetBshImage(0).height) / 2;

    SDL_Texture* backgroundTexture = converter.Convert(&bsh_leser.GetBshImage(0));

    auto shipGad = basegad->GetGadgetsByIndex(1);
    SDL_Texture* shipTexture = converter.Convert(&bsh_leser.GetBshImage(shipGad->Gfxnr));

    auto singlePlayerButtonGad = basegad->GetGadgetsByIndex(2);
    SDL_Texture* singlePlayerTexture = converter.Convert(&bsh_leser.GetBshImage(singlePlayerButtonGad->Gfxnr));
    SDL_Texture* singlePlayerTextureClicked = converter.Convert(&bsh_leser.GetBshImage(singlePlayerButtonGad->Gfxnr + singlePlayerButtonGad->Pressoff));

    auto multiPlayerButtonGad = basegad->GetGadgetsByIndex(3);
    SDL_Texture* multiPlayerTexture = converter.Convert(&bsh_leser.GetBshImage(multiPlayerButtonGad->Gfxnr));
    SDL_Texture* multiPlayerTextureClicked = converter.Convert(&bsh_leser.GetBshImage(multiPlayerButtonGad->Gfxnr + multiPlayerButtonGad->Pressoff));

    auto creditsButtonGad = basegad->GetGadgetsByIndex(4);
    SDL_Texture* creditsTexture = converter.Convert(&bsh_leser.GetBshImage(creditsButtonGad->Gfxnr));
    SDL_Texture* creditsTextureClicked = converter.Convert(&bsh_leser.GetBshImage(creditsButtonGad->Gfxnr + creditsButtonGad->Pressoff));

    auto introButtonGad = basegad->GetGadgetsByIndex(5);
    SDL_Texture* introTexture = converter.Convert(&bsh_leser.GetBshImage(introButtonGad->Gfxnr));
    SDL_Texture* introTextureClicked = converter.Convert(&bsh_leser.GetBshImage(introButtonGad->Gfxnr + introButtonGad->Pressoff));

    auto exitButtonGad = basegad->GetGadgetsByIndex(6);
    SDL_Texture* exitTexture = converter.Convert(&bsh_leser.GetBshImage(exitButtonGad->Gfxnr));
    SDL_Texture* exitTextureClicked = converter.Convert(&bsh_leser.GetBshImage(exitButtonGad->Gfxnr + exitButtonGad->Pressoff));

    {
        auto& background = wdg<TextureView>(backgroundTexture);
        background.setPosition(scaleLeftBorder, scaleUpperBorder);
        widgets.push_back(std::make_tuple(&background, 0, 0));

        auto& ship = wdg<TextureView>(shipTexture);
        ship.setPosition(shipGad->Pos.x, shipGad->Pos.y);
        widgets.push_back(std::make_tuple(&ship, shipGad->Pos.x, shipGad->Pos.y));

        auto& singlePlayerButton = wdg<TextureButton>(singlePlayerTexture, [this] {
            std::cout << "Singleplayer pressed" << std::endl;
            triggerSinglePlayer = true;
        });
        singlePlayerButton.setPosition(singlePlayerButtonGad->Pos.x, singlePlayerButtonGad->Pos.y);
        singlePlayerButton.setSecondaryTexture(singlePlayerTextureClicked);
        singlePlayerButton.setTextureSwitchFlags(TextureButton::OnClick);
        widgets.push_back(std::make_tuple(&singlePlayerButton, singlePlayerButtonGad->Pos.x, singlePlayerButtonGad->Pos.y));

        auto& multiPlayerButton = wdg<TextureButton>(multiPlayerTexture, [this] { std::cout << "Multiplayer pressed" << std::endl; });
        multiPlayerButton.setPosition(multiPlayerButtonGad->Pos.x, multiPlayerButtonGad->Pos.y);
        multiPlayerButton.setSecondaryTexture(multiPlayerTextureClicked);
        multiPlayerButton.setTextureSwitchFlags(TextureButton::OnClick);
        widgets.push_back(std::make_tuple(&multiPlayerButton, multiPlayerButtonGad->Pos.x, multiPlayerButtonGad->Pos.y));

        auto& creditsButton = wdg<TextureButton>(creditsTexture, [this] { std::cout << "credits pressed" << std::endl; });
        creditsButton.setPosition(creditsButtonGad->Pos.x, creditsButtonGad->Pos.y);
        creditsButton.setSecondaryTexture(creditsTextureClicked);
        creditsButton.setTextureSwitchFlags(TextureButton::OnClick);
        widgets.push_back(std::make_tuple(&creditsButton, creditsButtonGad->Pos.x, creditsButtonGad->Pos.y));

        auto& introButton = wdg<TextureButton>(introTexture, [this] { std::cout << "intro pressed" << std::endl; });
        introButton.setPosition(introButtonGad->Pos.x, introButtonGad->Pos.y);
        introButton.setSecondaryTexture(introTextureClicked);
        introButton.setTextureSwitchFlags(TextureButton::OnClick);
        widgets.push_back(std::make_tuple(&introButton, introButtonGad->Pos.x, introButtonGad->Pos.y));

        auto& exitButton = wdg<TextureButton>(exitTexture, [this] {
            std::cout << "exit pressed" << std::endl;
            quit = true;
        });
        exitButton.setPosition(exitButtonGad->Pos.x, exitButtonGad->Pos.y);
        exitButton.setSecondaryTexture(exitTextureClicked);
        exitButton.setTextureSwitchFlags(TextureButton::OnClick);
        widgets.push_back(std::make_tuple(&exitButton, exitButtonGad->Pos.x, exitButtonGad->Pos.y));
    }
    performLayout(renderer);
    Redraw();
}

void MainMenu::Redraw()
{
    for (auto& w : widgets)
    {
        auto widget = std::get<0>(w);
        int scaleLeftBorder = (scale->GetScreenSize().width - 1024) / 2;
        int scaleUpperBorder = (scale->GetScreenSize().height - 768) / 2;
        widget->setPosition(Vector2i{ std::get<1>(w) + scaleLeftBorder, std::get<2>(w) + scaleUpperBorder });
    }
}

// todo: add signal/slot for exiting window
void MainMenu::Handle()
{
    auto palette = Palette::Instance();

    auto transparentColor = palette->GetColor(palette->GetTransparentColor());
    std::cout << "[INFO] Transparent color: " << (int)transparentColor.getRed() << ", " << (int)transparentColor.getGreen() << ", "
              << (int)transparentColor.getBlue() << std::endl;

    auto s8 = SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0);
    SDL_SetPaletteColors(s8->format->palette, palette->GetSDLColors(), 0, palette->size());

    try
    {
        auto finalSurface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
        auto texture = SDL_CreateTextureFromSurface(renderer, finalSurface);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        this->drawAll();
        SDL_RenderPresent(renderer);

        Fps fps;

        SDL_Event e;
        while (!quit)
        {
            while (SDL_PollEvent(&e) != 0)
            {
                switch (e.type)
                {
                    case SDL_QUIT:
                        quit = true;
                        break;
                    case SDL_USEREVENT:
                        break;
                    case SDL_KEYDOWN:
                        if (e.key.keysym.sym == SDLK_ESCAPE)
                        {
                            quit = true;
                        }
                        else if (e.key.keysym.sym == SDLK_F11)
                        {
                            fullscreen = !fullscreen;
                            scale->ToggleFullscreen();
                        }
                        break;
                    case SDL_WINDOWEVENT:
                        if (e.window.event == SDL_WINDOWEVENT_RESIZED)
                        {
                            scale->SetScreenSize(scale->GetScreenSize());
                        }
                        break;
                    default:
                        break;
                }
                this->onEvent(e);
            }
            Redraw();
            int x, y;
            SDL_GetMouseState(&x, &y);

            finalSurface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
            texture = SDL_CreateTextureFromSurface(renderer, finalSurface);
            SDL_FreeSurface(finalSurface);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
            if (triggerSinglePlayer)
            {
                triggerSinglePlayer = false;
                auto singleplayerwindow = std::make_shared<SinglePlayerWindow>(renderer, pwindow, width, height, fullscreen);
                singleplayerwindow->Handle();
            }
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
    SDL_FreeSurface(s8);
}
