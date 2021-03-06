#pragma once

#include "include.hpp"
#include "utility.hpp"
#include "global.hpp"

USE_GEODE_NAMESPACE();

class MultiplayerSimplePlayer : public SimplePlayer {
//private:
//    CCLabelBMFont* usernameLabel;
public:
    int playerId = 0;
    bool isPlayerOne = false;

    static MultiplayerSimplePlayer *create(int iconID);

    bool init(int iconID);
    void update(float delta) override;
};