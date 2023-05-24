#pragma once

#include "include.hpp"

class Global {
public:
    PlayLayer *playLayer = nullptr;

    // temporary, for testing
    // this needs to be replaced with some sort of class/map/whatever that keeps track over players (player/network id?)
    // and their player data
    SimplePlayer *tmp = nullptr;

    static Global *get();
};