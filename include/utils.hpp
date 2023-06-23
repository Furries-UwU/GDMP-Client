#include "include.hpp"
#include "packet.pb.h"

enum Gamemode:uint8_t {
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
    if(player->m_isShip) {
        return Gamemode::SHIP;
    } else if(player->m_isBall) {
        return Gamemode::BALL;
    } else if(player->m_isBird) {
        return Gamemode::UFO;
    } else if(player->m_isDart) {
        return Gamemode::WAVE;
    } else if(player->m_isRobot) {
        return Gamemode::ROBOT;
    } else if(player->m_isSpider) {
        return Gamemode::SPIDER;
    } else {
        return Gamemode::CUBE;
    }
}

gdmp::GameMode getGameModeFromGamemode(Gamemode gamemode) {
    switch (gamemode) {
        case NONE:   return gdmp::GameMode::NONE;
        case CUBE:   return gdmp::GameMode::CUBE;
        case SHIP:   return gdmp::GameMode::SHIP;
        case BALL:   return gdmp::GameMode::BALL;
        case UFO:    return gdmp::GameMode::UFO;
        case WAVE:   return gdmp::GameMode::WAVE;
        case ROBOT:  return gdmp::GameMode::ROBOT;
        case SPIDER: return gdmp::GameMode::SPIDER;
    }
}

Gamemode getGamemodeFromGameMode(gdmp::GameMode gamemode) {
    switch (gamemode) {
        case gdmp::GameMode::NONE:   return Gamemode::NONE;
        case gdmp::GameMode::CUBE:   return Gamemode::CUBE;
        case gdmp::GameMode::SHIP:   return Gamemode::SHIP;
        case gdmp::GameMode::BALL:   return Gamemode::BALL;
        case gdmp::GameMode::UFO:    return Gamemode::UFO;
        case gdmp::GameMode::WAVE:   return Gamemode::WAVE;
        case gdmp::GameMode::ROBOT:  return Gamemode::ROBOT;
        case gdmp::GameMode::SPIDER: return Gamemode::SPIDER;
    }

    return Gamemode::CUBE;
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
    return pos;
}

uint32_t getIntFromCCColor(const cocos2d::ccColor3B &cccolor) {
    uint32_t color = cccolor.r;
    color = (color << 8) + cccolor.g;
    color = (color << 8) + cccolor.b;

    return color;
}

gdmp::PlayerVisuals* getPlayerVisualData(PlayerObject *p1, PlayerObject *p2) {
    auto gameManager = GameManager::sharedState();

    auto visuals = new gdmp::PlayerVisuals();
    visuals->set_icon_cube(gameManager->getPlayerFrame());
    visuals->set_icon_ball(gameManager->getPlayerBall());
    visuals->set_icon_ufo(gameManager->getPlayerBird());
    visuals->set_icon_wave(gameManager->getPlayerDart());
    visuals->set_icon_spider(gameManager->getPlayerSpider());

    gdmp::Colors color_p1;
    gdmp::Colors color_p2;

#if __APPLE__ && TARGET_OS_MAC
    // PlayerObject::getSecondaryColor has an binding on mac but not anywhere else and i cba to find it lmao
    auto secondaryColor = getIntFromCCColor(p1->getSecondaryColor());
#else
    uint32_t secondaryColor = 0;

    // hope this works c:
    if (p1->m_iconSprite) {
        secondaryColor = getIntFromCCColor(p1->m_iconSprite->getColor());
    }
#endif

    color_p1.set_primary(getIntFromCCColor(p1->getColor()));
    color_p1.set_secondary(secondaryColor);

    auto colors = new gdmp::ColorInfo();
    colors->set_allocated_color_p1(&color_p1);
    colors->set_allocated_color_p2(&color_p1); // todo: use p2 colors

    colors->set_glowy(gameManager->getPlayerGlow());

    fmt::print("meow!!\n");
    visuals->set_allocated_colors(colors);
    fmt::print("meow!! return visuals\n");
    return visuals;
}