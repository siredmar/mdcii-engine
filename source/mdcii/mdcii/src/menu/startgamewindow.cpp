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

#include <codecvt>
#include <iostream>
#include <random>
#include <string>

#include "SDL2/SDL.h"

#include "sdlgui/label.h"
#include "sdlgui/layout.h"
#include "sdlgui/messagedialog.h"
#include "sdlgui/screen.h"
#include "sdlgui/texturebutton.h"
#include "sdlgui/textureview.h"
#include "sdlgui/vscrollpanel.h"
#include "sdlgui/window.h"

#include "cod/mission_gad.hpp"
#include "cod/text_cod.hpp"
#include "common/stringhelpers.hpp"
#include "menu/gamewindow.hpp"
#include "menu/scale.hpp"
#include "menu/singleplayerwindow.hpp"
#include "menu/startgamewindow.hpp"
#include "sdl2shared/sdl2shared.hpp"

using namespace sdlgui;
StartGameWindow::StartGameWindow(SDL_Renderer* renderer, SDL_Window* pwindow, int width, int height, bool fullscreen)
    : Screen(pwindow, Vector2i(width, height), "Game", false, true, true)
    , renderer(renderer)
    , width(width)
    , height(height)
    , fullscreen(fullscreen)
    , buildings(Buildings::Instance())
    , pwindow(pwindow)
    , files(Files::Instance())
    , gad(std::make_shared<MissionGad>(std::make_shared<CodParser>(files->FindPathForFile("mission.gad"), false, false)))
    , quit(false)
    , stringConverter(StringToSDLTextureConverter(renderer, "zei20v.zei"))
    , triggerStartGame(false)
    , scale(Scale::Instance())
    , singleGame(GamesPb::SingleGame())
    , campaign(GamesPb::Campaign())
    , theme(new Theme(renderer))
{
    std::cout << "mission.gad: " << gad->GetGadgetsSize() << std::endl;
    BshReader bsh_leser(files->FindPathForFile("toolgfx/start.bsh"));
    BshImageToSDLTextureConverter converter(renderer);

    int scaleLeftBorder = (scale->GetScreenSize().width - bsh_leser.GetBshImage(0).width) / 2;
    int scaleUpperBorder = (scale->GetScreenSize().height - bsh_leser.GetBshImage(0).height) / 2;

    SDL_Texture* backgroundTexture = converter.Convert(&bsh_leser.GetBshImage(0));

    auto startGameButtonGad = gad->GetGadgetByIndex(1);
    SDL_Texture* startGameTexture = converter.Convert(&bsh_leser.GetBshImage(startGameButtonGad->Gfxnr));
    SDL_Texture* startGameTextureClicked = converter.Convert(&bsh_leser.GetBshImage(startGameButtonGad->Gfxnr + startGameButtonGad->Pressoff));

    auto abortButtonGad = gad->GetGadgetByIndex(2);
    SDL_Texture* abortTexture = converter.Convert(&bsh_leser.GetBshImage(abortButtonGad->Gfxnr));
    SDL_Texture* abortTextureClicked = converter.Convert(&bsh_leser.GetBshImage(abortButtonGad->Gfxnr + abortButtonGad->Pressoff));

    auto missionGad = gad->GetGadgetByIndex(3);
    SDL_Texture* missionTexture = converter.Convert(&bsh_leser.GetBshImage(missionGad->Gfxnr));

    auto highscoreGad = gad->GetGadgetByIndex(4);
    SDL_Texture* highscoreTexture = converter.Convert(&bsh_leser.GetBshImage(highscoreGad->Gfxnr));

    // auto missionTextGad = gad->GetGadgetByIndex();

    auto tableGad = gad->GetGadgetByIndex(5);
    SDL_Texture* tableTexture = converter.Convert(&bsh_leser.GetBshImage(tableGad->Gfxnr));

    auto scrollUpButtonGad = gad->GetGadgetByIndex(6);
    SDL_Texture* scrollUpTexture = converter.Convert(&bsh_leser.GetBshImage(scrollUpButtonGad->Gfxnr));
    SDL_Texture* scrollUpTextureClicked = converter.Convert(&bsh_leser.GetBshImage(scrollUpButtonGad->Gfxnr + scrollUpButtonGad->Pressoff));

    auto scrollDownButtonGad = gad->GetGadgetByIndex(7);
    SDL_Texture* scrollDownTexture = converter.Convert(&bsh_leser.GetBshImage(scrollDownButtonGad->Gfxnr));
    SDL_Texture* scrollDownTextureClicked = converter.Convert(&bsh_leser.GetBshImage(scrollDownButtonGad->Gfxnr + scrollDownButtonGad->Pressoff));

    auto emptyMissionTextTexture = stringConverter.Convert("mission", 243, 0, 0);

    for (int i = 8; i < 13; i++)
    {
        auto missionUnselectedGad = gad->GetGadgetByIndex(i);
        auto missionUnselectedTexture = sdl2::make_shared(converter.Convert(&bsh_leser.GetBshImage(missionUnselectedGad->Gfxnr)));
        auto missionSelectedTexture = sdl2::make_shared(converter.Convert(&bsh_leser.GetBshImage(missionUnselectedGad->Gfxnr + missionUnselectedGad->Pressoff)));
        missionSelect.push_back(std::make_tuple(missionUnselectedGad, missionUnselectedTexture, missionSelectedTexture));
    }

    {
        // BACKGROUND
        auto& background = wdg<TextureView>(backgroundTexture);
        background.setPosition(scaleLeftBorder, scaleUpperBorder);
        widgets.push_back(std::make_tuple(&background, 0, 0));

        // LABELS
        // // Mission
        auto& mission = wdg<TextureView>(missionTexture);
        mission.setPosition(scaleLeftBorder, scaleUpperBorder);
        widgets.push_back(std::make_tuple(&mission, scaleLeftBorder + missionGad->Pos.x, scaleUpperBorder + missionGad->Pos.y));

        // // Mission text

        theme->mTextColor = Color(202, 172, 52, 255);
        theme->mTextColorShadow = Color(0, 0, 0, 100);

        auto& slider = wdg<VScrollPanel>();
        slider.setPosition(Vector2i{ 450 + scaleLeftBorder, 385 + scaleUpperBorder });
        slider.setFixedWidth(470);
        slider.setFixedHeight(200);

        widgets.push_back(std::make_tuple(&slider, scaleLeftBorder + 450, scaleUpperBorder + 385));
        missionLabel = new Label(&slider, "");
        missionLabel->setMultiline(420);
        missionLabel->setTheme(theme);
        // missionLabel->setFixedSize(Vector2i{ 450, 300 });

        // // Highscore
        auto& highscore = wdg<TextureView>(highscoreTexture);
        highscore.setPosition(highscoreGad->Pos.x + scaleLeftBorder, highscoreGad->Pos.y + scaleUpperBorder);
        widgets.push_back(std::make_tuple(&highscore, scaleLeftBorder + highscoreGad->Pos.x, scaleUpperBorder + highscoreGad->Pos.y));

        // BUTTONS
        auto& scrollUpButton = wdg<TextureButton>(scrollUpTexture, [this] { /*currentTablePtr->scrollPositive();*/ });
        scrollUpButton.setPosition(scaleLeftBorder + scrollUpButtonGad->Pos.x, scaleUpperBorder + scrollUpButtonGad->Pos.y);
        scrollUpButton.setSecondaryTexture(scrollUpTextureClicked);
        scrollUpButton.setFlags(TextureButton::NormalButton | TextureButton::OnClick);
        widgets.push_back(std::make_tuple(&scrollUpButton, scrollUpButtonGad->Pos.x, scrollUpButtonGad->Pos.y));

        auto& scrollDownButton = wdg<TextureButton>(scrollDownTexture, [this] { /*currentTablePtr->scrollNegative();*/ });
        scrollDownButton.setPosition(scaleLeftBorder + scrollDownButtonGad->Pos.x, scaleUpperBorder + scrollDownButtonGad->Pos.y);
        scrollDownButton.setSecondaryTexture(scrollDownTextureClicked);
        scrollDownButton.setFlags(TextureButton::NormalButton | TextureButton::OnClick);
        widgets.push_back(std::make_tuple(&scrollDownButton, scrollDownButtonGad->Pos.x, scrollDownButtonGad->Pos.y));

        // TABLES
        auto& tableFrame = wdg<TextureView>(tableTexture);
        tableFrame.setPosition(scaleLeftBorder + tableGad->Pos.x, scaleUpperBorder + tableGad->Pos.y);
        widgets.push_back(std::make_tuple(&tableFrame, tableGad->Pos.x, tableGad->Pos.y));

        // BUTTONS 2nd
        auto& startGameButton = wdg<TextureButton>(startGameTexture, [&] {
            std::cout << "Singleplayer pressed" << std::endl;
            triggerStartGame = true;
        });
        startGameButton.setPosition(scaleLeftBorder + startGameButtonGad->Pos.x, scaleUpperBorder + startGameButtonGad->Pos.y);
        startGameButton.setSecondaryTexture(startGameTextureClicked);
        startGameButton.setFlags(TextureButton::NormalButton | TextureButton::OnClick);
        widgets.push_back(std::make_tuple(&startGameButton, startGameButtonGad->Pos.x, startGameButtonGad->Pos.y));

        auto& abortButton = wdg<TextureButton>(abortTexture, [&] {
            std::cout << "Abort pressed" << std::endl;
            quit = true;
        });
        abortButton.setPosition(scaleLeftBorder + abortButtonGad->Pos.x, scaleUpperBorder + abortButtonGad->Pos.y);
        abortButton.setSecondaryTexture(abortTextureClicked);
        abortButton.setFlags(TextureButton::NormalButton | TextureButton::OnClick);
        widgets.push_back(std::make_tuple(&abortButton, abortButtonGad->Pos.x, abortButtonGad->Pos.y));
        // if (campaign.game_size() > 0)
        missionLabel->setCaption(this->singleGame.missiontext());
        {
            // missionLabel->setCaption(this->campaign.game(0).missiontext());
            for (int i = 0; i < 5; i++)
            {
                auto& missionSelectButton = wdg<TextureButton>(std::get<1>(missionSelect[i]).get(), [this, i, &slider] {
                    std::cout << this->campaign.name() << std::endl;
                    for (int missions = 0; missions < 5; missions++)
                    {
                        if (missions == i)
                        {
                            continue;
                        }
                        missionSelectButtons[missions]->setPushed(false);
                    }
                    if (this->campaign.game_size() > 0 && this->campaign.game_size() > i)
                    {
                        this->singleGame = this->campaign.game(i);
                        std::cout << "Mission " + this->singleGame.name() + " selected" << std::endl;
                        missionLabel->setCaption(this->singleGame.missiontext());
                        slider.setScrollAmount(0);
                    }
                });
                missionSelectButton.setPosition(scaleLeftBorder + std::get<0>(missionSelect[i])->Pos.x, scaleUpperBorder + std::get<0>(missionSelect[i])->Pos.y);
                missionSelectButton.setSecondaryTexture(std::get<2>(missionSelect[i]).get());
                missionSelectButton.setFlags(TextureButton::OnClick | TextureButton::ToggleButton);
                missionSelectButton.setVisible(false);
                widgets.push_back(std::make_tuple(&missionSelectButton, std::get<0>(missionSelect[i])->Pos.x, std::get<0>(missionSelect[i])->Pos.y));
                missionSelectButtons.push_back(&missionSelectButton);

                auto& missionSelectLabel = wdg<TextureView>(emptyMissionTextTexture);
                missionSelectLabel.setPosition(scaleLeftBorder + std::get<0>(missionSelect[i])->Pos.x + 30, scaleUpperBorder + std::get<0>(missionSelect[i])->Pos.y);
                missionSelectLabel.setVisible(false);
                widgets.push_back(std::make_tuple(&missionSelectLabel, std::get<0>(missionSelect[i])->Pos.x + 30, std::get<0>(missionSelect[i])->Pos.y));
                missionSelectLabels.push_back(&missionSelectLabel);
            }
        }
        // else
        // {
        //     missionLabel->setCaption(this->singleGame.missiontext());
        // }
    }
    performLayout(renderer);
    Redraw();
}

void StartGameWindow::SetGame(const GamesPb::SingleGame& game)
{
    singleGame = game;
}

void StartGameWindow::SetGame(const GamesPb::Campaign& game)
{
    campaign = game;
    if (campaign.game_size() > 0)
    {
        missionSelectButtons[0]->setPushed(true);
        singleGame = campaign.game(0);
        std::cout << "[INFO] Setting campaign " + campaign.name() + ". Missions: " << campaign.game_size() << std::endl;
        for (int i = 0; i < campaign.game_size(); i++)
        {
            missionSelectButtons[i]->setVisible(true);
            missionSelectLabels[i]->setVisible(true);
            auto name = campaign.game(i).name();
            std::cout << name << std::endl;
            missionSelectLabels[i]->setTexture(stringConverter.Convert(campaign.game(i).name(), 243, 0, 0));
        }
    }
    else
    {
        throw(std::string("[ERR] Campaign " + campaign.name() + "has no missions!"));
    }
}

void StartGameWindow::LoadGame(const GamesPb::SingleGame& gamName)
{
    if (files->CheckFile(gamName.path()) == false)
    {
        std::cout << "[ERR] Could not load savegame: " << gamName.path() << std::endl;
        exit(EXIT_FAILURE);
    }
    auto gameWindow = std::make_shared<GameWindow>(renderer, pwindow, gamName.path(), fullscreen);
    gameWindow->Handle();
    gameWindow.reset();
}

void StartGameWindow::Redraw()
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
void StartGameWindow::Handle()
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
        // later used for key inputs, uncommented for now
        // const Uint8* keystate = SDL_GetKeyboardState(NULL);
        SDL_Event e;
        while (!quit)
        {
            int x, y;
            SDL_GetMouseState(&x, &y);
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

            finalSurface = SDL_ConvertSurfaceFormat(s8, SDL_PIXELFORMAT_RGB888, 0);
            texture = SDL_CreateTextureFromSurface(renderer, finalSurface);
            SDL_FreeSurface(finalSurface);
            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
            if (triggerStartGame)
            {
                triggerStartGame = false;
                LoadGame(singleGame);
                quit = true;
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
