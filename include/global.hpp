#pragma once

#include "include.hpp"

class Global {
public:
    ENetHost *host = nullptr;
    ENetPeer *peer = nullptr;

    bool connected = false;

    std::unordered_map<uint64_t, SimplePlayer*> players;

    static Global *get();
};