[![CircleCI](https://circleci.com/gh/circleci/circleci-docs.svg?style=shield)](https://circleci.com/gh/siredmar/mdcii-engine) [![CodeFactor](https://www.codefactor.io/repository/github/siredmar/mdcii-engine/badge)](https://www.codefactor.io/repository/github/siredmar/mdcii-engine)

# MDCII Game Engine

The main goal of this project is to provid an independent reimplementation of the game engine for Anno 1602/1602 AD under a free license.

Currently this project contains several helper tools:

-   [`bshdump`](doc/bshdump.md)
-   [`bshpacker`](doc/bshpacker.md)
-   `cod_parser`   
-   `codcat`
-   `gam_parser`
-   `paldump`
-   `zeidump`
-   `inselbmp`
-   `weltbmp`
-   `zeitext`

The most complex program is [`mdcii-sdltest`](doc/mdcii-sdltest.md) that can load savegames and scenario files and animate the buildings.

## Media

See this video for a short demonstration of an earlier stage

<iframe width="560" height="315" src="https://www.youtube.com/embed/1Nw7DcvG0gk" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

## How to build

Clone the repo

    mkdir build
    cd build
    cmake ..
    make -j8

The main program is called `mdcii-sdltest`.

## How to run the game

The game uses the original files from your Anno 1602/1602 AD installation.

    $ ./mdcii-sdltest -p <path-to-1602-ad-installation>

## License

GPL Version 2 or newer, see [COPYING](COPYING).
