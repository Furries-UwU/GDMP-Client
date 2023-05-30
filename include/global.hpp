#pragma once

#include "include.hpp"

class Global {
public:
    ENetHost *host = nullptr;
    ENetPeer *peer = nullptr;

    bool connected = false;

    // temporary, for testing
    // this needs to be replaced with some sort of class/map/whatever that keeps track over players (player/network id?)
    // and their player data
    SimplePlayer *tmp = nullptr;

    static Global *get();
};