/*
    sdlgui/example1.cpp -- C++ version of an example application that shows 
    how to use the various widget classes. 

    Based on NanoGUI by Wenzel Jakob <wenzel@inf.ethz.ch>.
    Adaptation for SDL by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <memory>
#include <sdlgui/button.h>
#include <sdlgui/screen.h>
#include <sdlgui/window.h>

#if defined(_WIN32)
#include <windows.h>
#endif
#include <iostream>

#if defined(_WIN32)
#include <SDL.h>
#else
#include <SDL2/SDL.h>
#endif
#if defined(_WIN32)
#include <SDL_image.h>
#else
#include <SDL2/SDL_image.h>
#endif

using std::cerr;
using std::cout;
using std::endl;

#undef main

using namespace sdlgui;

class TestWindow1;
class TestWindow2;

class Fps
{
public:
    explicit Fps(int tickInterval = 30)
        : m_tickInterval(tickInterval)
        , m_nextTime(SDL_GetTicks() + tickInterval)
    {
    }

    void next()
    {
        SDL_Delay(getTicksToNextFrame());

        m_nextTime += m_tickInterval;
    }

private:
    const int m_tickInterval;
    Uint32 m_nextTime;

    Uint32 getTicksToNextFrame() const
    {
        Uint32 now = SDL_GetTicks();

        return (m_nextTime <= now) ? 0 : m_nextTime - now;
    }
};

class TestWindow2 : public Screen
{
public:
    TestWindow2(SDL_Window* pwindow, int rwidth, int rheight)
        : Screen(pwindow, Vector2i(rwidth, rheight), "SDL_gui Test", false, false, true)
        , pwindow(pwindow)
        , quit(false)
        , width(rwidth)
        , height(rheight)
    {
        {
            auto& button = wdg<Button>("Button 2", [this] {
                std::cout << "screen 2 button pushed!" << std::endl;
            });
            button.setSize(sdlgui::Vector2i{ 100, 20 });
            button.setPosition(100, 100);

            auto& back = wdg<Button>("Back", [this] {
                std::cout << "back button pushed!" << std::endl;
                quit = true;
            });
            back.setSize(sdlgui::Vector2i{ 100, 20 });
            back.setPosition(100, 140);
        }
        performLayout(mSDL_Renderer);
    }

    ~TestWindow2()
    {
    }

    int Handle()
    {
        Fps fps;
        try
        {
            SDL_Event e;
            while (!quit)
            {
                //Handle events on queue
                while (SDL_PollEvent(&e) != 0)
                {
                    //User requests quit
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                }
                bool event = onEvent(e);
                std::cout << "event: " << event << std::endl;

                std::cout << "Screen 2 running" << std::endl;

                SDL_SetRenderDrawColor(mSDL_Renderer, 0xd3, 0xd3, 0xd3, 0xff);
                SDL_RenderClear(mSDL_Renderer);

                drawAll();

                // Render the rect to the screen
                SDL_RenderPresent(mSDL_Renderer);

                fps.next();
            }
        }
        catch (const std::runtime_error& e)
        {
            std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
#if defined(_WIN32)
            MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
#else
            std::cerr << error_msg << endl;
#endif
            return -1;
        }
        return 0;
    }

private:
    SDL_Window* pwindow;
    bool quit;
    int width;
    int height;
};

class TestWindow1 : public Screen
{
public:
    TestWindow1(SDL_Window* pwindow, int rwidth, int rheight)
        : Screen(pwindow, Vector2i(rwidth, rheight), "SDL_gui Test", false, false, true)
        , pwindow(pwindow)
        , quit(false)
        , loadScreenTwo(false)
        , width(rwidth)
        , height(rheight)
    {
        {
            auto& button = wdg<Button>("Button 1", [this] {
                std::cout << "screen 1 button pushed!" << std::endl;
                loadScreenTwo = true;
            });
            button.setSize(sdlgui::Vector2i{ 100, 20 });
            button.setPosition(100, 100);
            buttonPtr = &button;

            auto& exit = wdg<Button>("Exit", [&] {
                std::cout << "Exit button pushed!" << std::endl;
                quit = true;
            });
            exit.setSize(sdlgui::Vector2i{ 100, 20 });
            exit.setPosition(100, 140);
        }
        performLayout(mSDL_Renderer);
    }

    ~TestWindow1()
    {
    }

    int Handle()
    {
        Fps fps;

        bool quit = false;
        try
        {
            SDL_Event e;
            while (!quit)
            {
                //Handle events on queue
                while (SDL_PollEvent(&e) != 0)
                {
                    //User requests quit
                    if (e.type == SDL_QUIT)
                    {
                        quit = true;
                    }
                }
                bool event = onEvent(e);
                std::cout << "event: " << event << std::endl;

                std::cout << "Sceen 1 running" << std::endl;

                SDL_SetRenderDrawColor(mSDL_Renderer, 0xd3, 0xd3, 0xd3, 0xff);
                SDL_RenderClear(mSDL_Renderer);

                drawAll();

                // Render the rect to the screen
                SDL_RenderPresent(mSDL_Renderer);

                fps.next();
                if (loadScreenTwo)
                {
                    loadScreenTwo = false;
                    TestWindow2 testwindow2(pwindow, width, height);
                    testwindow2.Handle();
                    std::cout << "Screen2 left, welcome to screen 1" << std::endl;
                    performLayout(mSDL_Renderer);
                }
            }
        }
        catch (const std::runtime_error& e)
        {
            std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
#if defined(_WIN32)
            MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
#else
            std::cerr << error_msg << endl;
#endif
            return -1;
        }
        return 0;
    }

private:
    SDL_Window* pwindow;
    bool quit;
    bool loadScreenTwo;
    int width;
    int height;
    Widget* buttonPtr;
};

int main(int /* argc */, char** /* argv */)
{
    char rendername[256] = { 0 };
    SDL_RendererInfo info;

    SDL_Init(SDL_INIT_VIDEO); // Initialize SDL2

    SDL_Window* window; // Declare a pointer to an SDL_Window

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    int winWidth = 400;
    int winHeight = 200;

    // Create an application window with the following settings:
    window = SDL_CreateWindow(
        "An SDL2 window", //    const char* title
        SDL_WINDOWPOS_UNDEFINED, //    int x: initial x position
        SDL_WINDOWPOS_UNDEFINED, //    int y: initial y position
        winWidth, //    int w: width, in pixels
        winHeight, //    int h: height, in pixels
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI //    Uint32 flags: window options, see docs
    );

    // Check that the window was successfully made
    if (window == NULL)
    {
        // In the event that the window could not be made...
        std::cout << "Could not create window: " << SDL_GetError() << '\n';
        SDL_Quit();
        return 1;
    }

    auto context = SDL_GL_CreateContext(window);

    for (int it = 0; it < SDL_GetNumRenderDrivers(); it++)
    {
        SDL_GetRenderDriverInfo(it, &info);
        strcat(rendername, info.name);
        strcat(rendername, " ");
    }

    SDL_Renderer* mSDL_Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(mSDL_Renderer, SDL_BLENDMODE_BLEND);

    TestWindow1* screen1 = new TestWindow1(window, winWidth, winHeight);
    screen1->Handle();

    return 0;
}
