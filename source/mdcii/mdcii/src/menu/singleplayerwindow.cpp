
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
#include <random>

#include "SDL2/SDL.h"

#include "sdlgui/label.h"
#include "sdlgui/layout.h"
#include "sdlgui/messagedialog.h"
#include "sdlgui/screen.h"
#include "sdlgui/texturebutton.h"
#include "sdlgui/textureview.h"
#include "sdlgui/window.h"

#include "cod/text_cod.hpp"
#include "menu/gamewindow.hpp"
#include "menu/scale.hpp"
#include "menu/singleplayerwindow.hpp"
#include "menu/startgamewindow.hpp"
#include "savegames/savegames.hpp"
#include "savegames/scenarios.hpp"

#include "gamelist.pb.h"

static std::default_random_engine dre(std::chrono::steady_clock::now().time_since_epoch().count());

using namespace sdlgui;
SinglePlayerWindow::SinglePlayerWindow(SDL_Renderer* renderer, SDL_Window* pwindow, int width, int height, bool fullscreen)
    : Screen(pwindow, Vector2i(width, height), "Game", false, true, true)
    , renderer(renderer)
    , width(width)
    , height(height)
    , fullscreen(fullscreen)
    , buildings(Buildings::Instance())
    , pwindow(pwindow)
    , files(Files::Instance())
    , hostgad(std::make_shared<Hostgad>(std::make_shared<CodParser>(files->FindPathForFile("host.gad"), false, false)))
    , quit(false)
    , stringConverter(StringToSDLTextureConverter(renderer, "zei20v.zei"))
    , savegameSingleGame(GamesPb::SingleGame())
    , savegame("")
    , triggerStartSingleGame(false)
    , triggerStartGame(false)
    , scale(Scale::Instance())
{
    std::cout << "host.gad: " << hostgad->GetGadgetsSize() << std::endl;
    BshReader bsh_leser(files->FindPathForFile("toolgfx/start.bsh"));
    BshImageToSDLTextureConverter converter(renderer);

    int scaleLeftBorder = (scale->GetScreenSize().width - bsh_leser.GetBshImage(0).width) / 2;
    int scaleUpperBorder = (scale->GetScreenSize().height - bsh_leser.GetBshImage(0).height) / 2;

    SDL_Texture* backgroundTexture = converter.Convert(&bsh_leser.GetBshImage(0));

    auto tableGad = hostgad->GetGadgetByIndex(1);
    SDL_Texture* tableTexture = converter.Convert(&bsh_leser.GetBshImage(tableGad->Gfxnr));

    for (int i = 0; i < 5; i++)
    {
        tableStars.push_back(converter.Convert(&bsh_leser.GetBshImage(21 + i)));
    }

    auto newGameButtonGad = hostgad->GetGadgetByIndex(2);
    SDL_Texture* newGameTexture = converter.Convert(&bsh_leser.GetBshImage(newGameButtonGad->Gfxnr));
    SDL_Texture* newGameTextureClicked = converter.Convert(&bsh_leser.GetBshImage(newGameButtonGad->Gfxnr + newGameButtonGad->Pressoff));

    auto loadGameButtonGad = hostgad->GetGadgetByIndex(3);
    SDL_Texture* loadGameTexture = converter.Convert(&bsh_leser.GetBshImage(loadGameButtonGad->Gfxnr));
    SDL_Texture* loadGameTextureClicked = converter.Convert(&bsh_leser.GetBshImage(loadGameButtonGad->Gfxnr + loadGameButtonGad->Pressoff));

    auto continueGameButtonGad = hostgad->GetGadgetByIndex(4);
    SDL_Texture* continueTexture = converter.Convert(&bsh_leser.GetBshImage(continueGameButtonGad->Gfxnr));
    SDL_Texture* continueTextureClicked = converter.Convert(&bsh_leser.GetBshImage(continueGameButtonGad->Gfxnr + continueGameButtonGad->Pressoff));

    auto mainMenuButtonGad = hostgad->GetGadgetByIndex(5);
    SDL_Texture* mainMenuTexture = converter.Convert(&bsh_leser.GetBshImage(mainMenuButtonGad->Gfxnr));
    SDL_Texture* mainMenuTextureClicked = converter.Convert(&bsh_leser.GetBshImage(mainMenuButtonGad->Gfxnr + mainMenuButtonGad->Pressoff));

    auto scrollUpButtonGad = hostgad->GetGadgetByIndex(6);
    SDL_Texture* scrollUpTexture = converter.Convert(&bsh_leser.GetBshImage(scrollUpButtonGad->Gfxnr));
    SDL_Texture* scrollUpTextureClicked = converter.Convert(&bsh_leser.GetBshImage(scrollUpButtonGad->Gfxnr + scrollUpButtonGad->Pressoff));

    auto scrollDownButtonGad = hostgad->GetGadgetByIndex(7);
    SDL_Texture* scrollDownTexture = converter.Convert(&bsh_leser.GetBshImage(scrollDownButtonGad->Gfxnr));
    SDL_Texture* scrollDownTextureClicked = converter.Convert(&bsh_leser.GetBshImage(scrollDownButtonGad->Gfxnr + scrollDownButtonGad->Pressoff));

    {
        // BACKGROUND
        auto& background = wdg<TextureView>(backgroundTexture);
        background.setPosition(scaleLeftBorder, scaleUpperBorder);
        widgets.push_back(std::make_tuple(&background, 0, 0));

        // BUTTONS
        auto& scrollUpButton = wdg<TextureButton>(scrollUpTexture, [this] { currentTablePtr->scrollPositive(); });
        scrollUpButton.setPosition(scaleLeftBorder + scrollUpButtonGad->Pos.x, scaleUpperBorder + scrollUpButtonGad->Pos.y);
        scrollUpButton.setSecondaryTexture(scrollUpTextureClicked);
        scrollUpButton.setTextureSwitchFlags(TextureButton::OnClick);
        widgets.push_back(std::make_tuple(&scrollUpButton, scrollUpButtonGad->Pos.x, scrollUpButtonGad->Pos.y));

        auto& scrollDownButton = wdg<TextureButton>(scrollDownTexture, [this] { currentTablePtr->scrollNegative(); });
        scrollDownButton.setPosition(scaleLeftBorder + scrollDownButtonGad->Pos.x, scaleUpperBorder + scrollDownButtonGad->Pos.y);
        scrollDownButton.setSecondaryTexture(scrollDownTextureClicked);
        scrollDownButton.setTextureSwitchFlags(TextureButton::OnClick);
        widgets.push_back(std::make_tuple(&scrollDownButton, scrollDownButtonGad->Pos.x, scrollDownButtonGad->Pos.y));

        // TABLES
        auto& tableFrame = wdg<TextureView>(tableTexture);
        tableFrame.setPosition(scaleLeftBorder + tableGad->Pos.x, scaleUpperBorder + tableGad->Pos.y);
        widgets.push_back(std::make_tuple(&tableFrame, tableGad->Pos.x, tableGad->Pos.y));

        // SCENARIOS
        Scenarios szenesRaw("/szenes", "szs");
        scenes = szenesRaw.Get();

        std::cout << "Single Missions" << std::endl;
        for (int i = 0; i < scenes.single_size(); i++)
        {
            std::cout << "--------------------------------------------------" << std::endl;
            std::cout << scenes.single(i).name() << std::endl;
            std::cout << scenes.single(i).path() << std::endl;
            std::cout << scenes.single(i).stars() << std::endl;
            std::cout << scenes.single(i).flags() << std::endl;
        }

        std::cout << "Endless games" << std::endl;
        for (int i = 0; i < scenes.endless_size(); i++)
        {
            std::cout << "--------------------------------------------------" << std::endl;
            std::cout << scenes.endless(i).name() << std::endl;
            std::cout << scenes.endless(i).path() << std::endl;
            std::cout << scenes.endless(i).stars() << std::endl;
            std::cout << scenes.endless(i).flags() << std::endl;
        }

        std::cout << "Campaigns" << std::endl;
        for (int i = 0; i < scenes.campaign_size(); i++)
        {
            std::cout << "--------------------------------------------------" << std::endl;
            std::cout << scenes.campaign(i).name() << std::endl;
            for (int a = 0; a < scenes.campaign(i).game_size(); a++)
            {
                std::cout << "\t" << scenes.campaign(i).game(a).name() << std::endl;
                std::cout << "\t" << scenes.campaign(i).game(a).path() << std::endl;
                std::cout << "\t" << scenes.campaign(i).game(a).stars() << std::endl;
                std::cout << "\t" << scenes.campaign(i).game(a).flags() << std::endl;
            }
        }

        auto& scenariosTable = *(new TextureTable<GamesPb::Games*>(
            this,
            // auto& scenariosTable = wdg<TextureTable>(
            Vector2i{ tableGad->Pos.x + 20, tableGad->Pos.y + 13 }, Vector2i{ tableGad->Size.w, tableGad->Size.h },
            [&](GamesPb::Games* elements) -> Widget* {
                auto& table = wdg<Widget>();

                // Original list entry
                auto& originalMissionsWidget = table.widget();
                auto originalMissionsText = TextCod::Instance()->GetValue("GAME", 85);
                auto originalMissionsTexture = stringConverter.Convert(originalMissionsText, 243, 0, 0);
                auto originalMissionsTextureHover = stringConverter.Convert(originalMissionsText, 245, 0, 0);
                auto& originalMissionsButton = originalMissionsWidget.texturebutton(originalMissionsTexture, [this, originalMissionsText] {
                    std::cout << originalMissionsText << " clicked" << std::endl;
                });
                originalMissionsButton.setPosition(0, 0);
                originalMissionsButton.setWidth(tableGad->Size.w);
                originalMissionsButton.setSecondaryTexture(originalMissionsTextureHover);
                originalMissionsButton.setTextureSwitchFlags(TextureButton::OnClick);
                originalMissionsWidget.setSize(originalMissionsButton.size());
                originalMissionsWidget.setVisible(false);

                // Original missions entries
                std::cout << elements->endless_size() << std::endl;
                auto entry = elements->endless(0);
                auto& endlessEntry = table.widget();
                auto endlessTexture = stringConverter.Convert(entry.name(), 243, 0, 0);
                auto endlessTextureHover = stringConverter.Convert(entry.name(), 245, 0, 0);
                auto& endlessButton = endlessEntry.texturebutton(endlessTexture, [this, entry, elements] {
                    std::cout << elements->endless_size() << std::endl;
                    std::cout << entry.name() << " clicked" << std::endl;
                    std::uniform_int_distribution<int> uid{ 0, elements->endless_size() - 1 };
                    int randomIndex = uid(dre);

                    std::cout << elements->endless(randomIndex).path() << " clicked" << std::endl;
                    savegame = elements->endless(randomIndex).path();
                    triggerStartGame = true;
                });
                endlessButton.setPosition(0, 0);
                endlessButton.setWidth(tableGad->Size.w);
                endlessButton.setSecondaryTexture(endlessTextureHover);
                endlessButton.setTextureSwitchFlags(TextureButton::OnClick);
                endlessEntry.setSize(endlessButton.size());

                if (entry.stars() >= 0)
                {
                    auto& star = endlessEntry.textureview(tableStars.at(entry.stars()));
                    star.setPosition(320, 0);
                }
                endlessEntry.setVisible(false);

                // New Adventures list entry
                auto& newAdventuresWidget = table.widget();
                auto newAdventuresText = TextCod::Instance()->GetValue("GAME", 87);
                auto texture = stringConverter.Convert(newAdventuresText, 243, 0, 0);
                auto textureHover = stringConverter.Convert(newAdventuresText, 245, 0, 0);
                auto& button = newAdventuresWidget.texturebutton(texture, [this, newAdventuresText] {
                    std::cout << newAdventuresText << " clicked" << std::endl;
                });
                button.setPosition(0, 0);
                button.setWidth(tableGad->Size.w);
                button.setSecondaryTexture(textureHover);
                button.setTextureSwitchFlags(TextureButton::OnClick);
                newAdventuresWidget.setSize(button.size());
                newAdventuresWidget.setVisible(false);

                // New Adventures missions entries
                for (int i = 0; i < elements->single_size(); i++)
                {
                    auto entry = elements->single(i);
                    auto& tableEntry = table.widget();
                    auto texture = stringConverter.Convert(entry.name(), 243, 0, 0);
                    auto textureHover = stringConverter.Convert(entry.name(), 245, 0, 0);
                    auto& button = tableEntry.texturebutton(texture, [this, entry] {
                        std::cout << entry.name() << " clicked" << std::endl;
                        savegameSingleGame = entry;
                        triggerStartSingleGame = true;
                    });
                    button.setPosition(0, 0);
                    button.setWidth(tableGad->Size.w);
                    button.setSecondaryTexture(textureHover);
                    button.setTextureSwitchFlags(TextureButton::OnClick);
                    tableEntry.setSize(button.size());

                    if (entry.stars() >= 0)
                    {
                        auto& star = tableEntry.textureview(tableStars.at(entry.stars()));
                        star.setPosition(320, 0);
                    }
                    tableEntry.setVisible(false);
                }
                return &table;
            },
            &scenes, 14, 14, true));
        scenariosTable.setPosition(scaleLeftBorder + tableGad->Pos.x + 20, scaleUpperBorder + tableGad->Pos.y + 13);
        scenariosTable.setVisible(true);
        scenariosTablePtr = &scenariosTable;
        currentTablePtr = scenariosTablePtr;
        widgets.push_back(std::make_tuple(&scenariosTable, tableGad->Pos.x + 20, tableGad->Pos.y + 13));

        // SAVEGAMES
        Savegames savesRaw("/savegame", ".gam");
        auto saves = savesRaw.GetSavegames();

        for (unsigned int i = 0; i < saves.size(); i++)
        {
            std::cout << "Saves " << i << ": " << std::get<1>(saves[i]) << std::endl;
        }
        auto& savegamesTable = *(new TextureTable<const std::vector<std::tuple<std::string, std::string, int>>&>(
            this,
            // auto& savegamesTable = wdg<TextureTable>(
            Vector2i{ tableGad->Pos.x + 20, tableGad->Pos.y + 13 }, Vector2i{ tableGad->Size.w, tableGad->Size.h },
            [&](const std::vector<std::tuple<std::string, std::string, int>>& elements) -> Widget* {
                auto& table = wdg<Widget>();
                for (auto& entry : elements)
                {
                    auto& tableEntry = table.widget();
                    auto texture = stringConverter.Convert(std::get<1>(entry), 243, 0, 0);
                    auto textureHover = stringConverter.Convert(std::get<1>(entry), 245, 0, 0);
                    auto& button = tableEntry.texturebutton(texture, [this, entry] {
                        std::cout << std::get<1>(entry) << " clicked" << std::endl;
                        savegame = std::get<0>(entry);
                        triggerStartGame = true;
                    });
                    button.setPosition(0, 0);
                    button.setWidth(tableGad->Size.w);
                    button.setSecondaryTexture(textureHover);
                    button.setTextureSwitchFlags(TextureButton::OnClick);
                    tableEntry.setSize(button.size());
                    tableEntry.setVisible(false);
                }
                return &table;
            },
            saves, 14, 14, true));
        savegamesTable.setPosition(scaleLeftBorder + tableGad->Pos.x + 20, scaleUpperBorder + tableGad->Pos.y + 13);
        savegamesTable.setVisible(false);
        savegamesTablePtr = &savegamesTable;
        widgets.push_back(std::make_tuple(&savegamesTable, tableGad->Pos.x + 20, tableGad->Pos.y + 13));

        // BUTTONS 2nd
        auto& newGameButton = wdg<TextureButton>(newGameTexture, [&] {
            std::cout << "Singleplayer pressed" << std::endl;
            savegamesTable.setVisible(false);
            scenariosTable.setVisible(true);
            currentTablePtr = scenariosTablePtr;
            scrollUpButton.setVisible(scenariosTablePtr->childAt(0)->childCount() < 14 ? false : true);
            scrollDownButton.setVisible(scenariosTablePtr->childAt(0)->childCount() < 14 ? false : true);
        });
        newGameButton.setPosition(scaleLeftBorder + newGameButtonGad->Pos.x, scaleUpperBorder + newGameButtonGad->Pos.y);
        newGameButton.setSecondaryTexture(newGameTextureClicked);
        newGameButton.setTextureSwitchFlags(TextureButton::OnClick);
        widgets.push_back(std::make_tuple(&newGameButton, newGameButtonGad->Pos.x, newGameButtonGad->Pos.y));

        auto& loadGameButton = wdg<TextureButton>(loadGameTexture, [&] {
            std::cout << "Load Game pressed" << std::endl;
            savegamesTable.setVisible(true);
            scenariosTable.setVisible(false);
            currentTablePtr = savegamesTablePtr;
            scrollUpButton.setVisible(savegamesTablePtr->childAt(0)->childCount() < 14 ? false : true);
            scrollDownButton.setVisible(savegamesTablePtr->childAt(0)->childCount() < 14 ? false : true);
        });
        loadGameButton.setPosition(scaleLeftBorder + loadGameButtonGad->Pos.x, scaleUpperBorder + loadGameButtonGad->Pos.y);
        loadGameButton.setSecondaryTexture(loadGameTextureClicked);
        loadGameButton.setTextureSwitchFlags(TextureButton::OnClick);
        widgets.push_back(std::make_tuple(&loadGameButton, loadGameButtonGad->Pos.x, loadGameButtonGad->Pos.y));

        auto& continueGameButton = wdg<TextureButton>(continueTexture, [this] {
            std::cout << "Continue Game pressed" << std::endl;
            LoadGame(Files::Instance()->FindPathForFile("lastgame.gam"));
        });
        continueGameButton.setPosition(scaleLeftBorder + continueGameButtonGad->Pos.x, scaleUpperBorder + continueGameButtonGad->Pos.y);
        continueGameButton.setSecondaryTexture(continueTextureClicked);
        continueGameButton.setTextureSwitchFlags(TextureButton::OnClick);
        widgets.push_back(std::make_tuple(&continueGameButton, continueGameButtonGad->Pos.x, continueGameButtonGad->Pos.y));

        auto& mainMenuButton = wdg<TextureButton>(mainMenuTexture, [this] {
            std::cout << "Main Menu pressed" << std::endl;
            quit = true;
        });
        mainMenuButton.setPosition(scaleLeftBorder + mainMenuButtonGad->Pos.x, scaleUpperBorder + mainMenuButtonGad->Pos.y);
        mainMenuButton.setSecondaryTexture(mainMenuTextureClicked);
        mainMenuButton.setTextureSwitchFlags(TextureButton::OnClick);
        widgets.push_back(std::make_tuple(&mainMenuButton, mainMenuButtonGad->Pos.x, mainMenuButtonGad->Pos.y));
    }

    performLayout(renderer);
    Redraw();
}

void SinglePlayerWindow::LoadGame(const GamesPb::SingleGame& gamName)
{
    auto startGameWindow = std::make_shared<StartGameWindow>(renderer, pwindow, width, height, fullscreen);
    startGameWindow->Handle(gamName);
    startGameWindow.reset();
}

void SinglePlayerWindow::LoadGame(const std::string& gamName)
{
    if (files->CheckFile(gamName) == false)
    {
        std::cout << "[ERR] Could not load savegame: " << gamName << std::endl;
        exit(EXIT_FAILURE);
    }
    auto gameWindow = std::make_shared<GameWindow>(renderer, pwindow, gamName, fullscreen);
    gameWindow->Handle();
}

void SinglePlayerWindow::Redraw()
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
void SinglePlayerWindow::Handle()
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
            if (triggerStartSingleGame)
            {
                triggerStartSingleGame = false;
                LoadGame(savegameSingleGame);
            }
            else if (triggerStartGame)
            {
                triggerStartGame = false;
                LoadGame(savegame);
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
