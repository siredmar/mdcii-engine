[![Codacy Badge](https://api.codacy.com/project/badge/Grade/22d6e29f7c7646fcaf269b691536c2b5)](https://app.codacy.com/manual/armin.schlegel/mdcii-engine?utm_source=github.com&utm_medium=referral&utm_content=siredmar/mdcii-engine&utm_campaign=Badge_Grade_Dashboard)
[![CircleCI](https://circleci.com/gh/circleci/circleci-docs.svg?style=shield)](https://circleci.com/gh/siredmar/mdcii-engine) [![CodeFactor](https://www.codefactor.io/repository/github/siredmar/mdcii-engine/badge)](https://www.codefactor.io/repository/github/siredmar/mdcii-engine)

# MDCII Game Engine

The main goal of this project is to provid an independent reimplementation of the game engine for Anno 1602/1602 AD under a free license.

Currently this project contains several helper tools:

-   [`bshdump`](docs/doc/bshdump.md)
-   [`bshpacker`](docs/doc/bshpacker.md)
-   `cod_parser`   
-   `codcat`
-   `gam_parser`
-   `paldump`
-   `zeidump`
-   `inselbmp`
-   `weltbmp`
-   `zeitext`

The most complex program is [`mdcii-sdltest`](docs/doc/mdcii-sdltest.md) that can load savegames and scenario files and animate the buildings.

## Media

See this video for a short demonstration of an earlier stage

[![mdcii youtube playlist](http://img.youtube.com/vi/1Nw7DcvG0gk/0.jpg)](https://www.youtube.com/playlist?list=PLsCp-i-X4SH-TQPoUgN8kicQza2BJ5K0h)

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
