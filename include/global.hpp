#pragma once

#include "include.hpp"

struct Player {
    SimplePlayer *p1 = nullptr;
    SimplePlayer *p2 = nullptr;

    uint32_t cube   = 0;
    uint32_t ship   = 0;
    uint32_t ball   = 0;
    uint32_t ufo    = 0;
    uint32_t wave   = 0;
    uint32_t robot  = 0;
    uint32_t spider = 0;
    bool glow       = 0;
};

class Global {
public:
    ENetHost *host = nullptr;
    ENetPeer *peer = nullptr;

    bool connected = false;

    std::unordered_map<uint64_t, Player> players;

    static Global *get();
};