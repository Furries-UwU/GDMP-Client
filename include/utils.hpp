#pragma once

#include "include.hpp"

enum Gamemode : uint8_t {
    NONE = 0,
    CUBE = 1,
    SHIP = 2,
    BALL = 3,
    UFO = 4,
    WAVE = 5,
    ROBOT = 6,
    SPIDER = 7
};

Gamemode getGamemodeFromPlayer(PlayerObject *player) {
    if (player->m_isShip) {
        return Gamemode::SHIP;
    } else if (player->m_isBall) {
        return Gamemode::BALL;
    } else if (player->m_isBird) {
        return Gamemode::UFO;
    } else if (player->m_isDart) {
        return Gamemode::WAVE;
    } else if (player->m_isRobot) {
        return Gamemode::ROBOT;
    } else if (player->m_isSpider) {
        return Gamemode::SPIDER;
    } else {
        return Gamemode::CUBE;
    }
}

gdmp::GameMode getGameModeFromGamemode(Gamemode gamemode) {
    switch (gamemode) {
        case NONE:
            return gdmp::GameMode::NONE;
        case CUBE:
            return gdmp::GameMode::CUBE;
        case SHIP:
            return gdmp::GameMode::SHIP;
        case BALL:
            return gdmp::GameMode::BALL;
        case UFO:
            return gdmp::GameMode::UFO;
        case WAVE:
            return gdmp::GameMode::WAVE;
        case ROBOT:
            return gdmp::GameMode::ROBOT;
        case SPIDER:
            return gdmp::GameMode::SPIDER;
    }
}

Gamemode getGamemodeFromGameMode(gdmp::GameMode gamemode) {
    switch (gamemode) {
        case gdmp::GameMode::NONE:
            return Gamemode::NONE;
        case gdmp::GameMode::CUBE:
            return Gamemode::CUBE;
        case gdmp::GameMode::SHIP:
            return Gamemode::SHIP;
        case gdmp::GameMode::BALL:
            return Gamemode::BALL;
        case gdmp::GameMode::UFO:
            return Gamemode::UFO;
        case gdmp::GameMode::WAVE:
            return Gamemode::WAVE;
        case gdmp::GameMode::ROBOT:
            return Gamemode::ROBOT;
        case gdmp::GameMode::SPIDER:
            return Gamemode::SPIDER;
            // default include:
            // gdmp::GameMode::CUBE
            // gdmp::GameMode_INT_MIN_SENTINEL_DO_NOT_USE_
            // gdmp::GameMode_INT_MAX_SENTINEL_DO_NOT_USE_
        default:
            return Gamemode::CUBE;
    }
}

IconType getIconType(Gamemode gamemode) {
    switch (gamemode) {
        default:
        case CUBE:
            return IconType::Cube;
        case SHIP:
            return IconType::Ship;
        case BALL:
            return IconType::Ball;
        case UFO:
            return IconType::Ufo;
        case WAVE:
            return IconType::Wave;
        case ROBOT:
            return IconType::Robot;
        case SPIDER:
            return IconType::Spider;
    }
}

gdmp::Position getPositionDataFromPlayer(PlayerObject *player) {
    gdmp::Position pos;
    pos.set_pos_x(player->m_position.x);
    pos.set_pos_y(player->m_position.y);
    pos.set_rotation(player->getRotation());
    pos.set_scale(player->getScale());
    if (player == PlayLayer::get()->m_player1) {
        pos.set_pressed(Global::get()->P1_pushing);
    } else {
        pos.set_pressed(Global::get()->P2_pushing);
    }
    return pos;
}

uint32_t getIntFromCCColor(const cocos2d::ccColor3B &cccolor) {
    uint32_t color = cccolor.r;
    color = (color << 8) + cccolor.g;
    color = (color << 8) + cccolor.b;

    return color;
}