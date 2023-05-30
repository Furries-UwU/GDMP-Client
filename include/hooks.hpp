#include "include.hpp"
#include "utils.hpp"

class $modify(MenuLayer) {
    void onMoreGames(cocos2d::CCObject* p0) {
        fmt::print("meow meeow!\n");
        Global* g = Global::get();

        ENetAddress addr;
        enet_address_set_host(&addr, "0.0.0.0");
        addr.port = 34154;

        g->peer = enet_host_connect(g->host, &addr, 1, 0);
        if (!g->peer) {
            fmt::print("failed to connect to server!!");
            return;
        }
        fmt::print("peer id: {}", g->peer->connectID);
    }
};

class $modify(PlayLayer) {
        bool init(GJGameLevel *level) {
            if (!PlayLayer::init(level)) return false;

            Global* global = Global::get();

            fmt::print("level id {}\n", level->m_levelID.value());

            auto idk = SimplePlayer::create(0);

            this->m_objectLayer->addChild(idk);
            global->tmp = idk;

            return true;
        }

        void onQuit() {
            PlayLayer::onQuit();

            fmt::print("quit!\n");
        }

        void update(float p0) {
            PlayLayer::update(p0);

            Global* global = Global::get();

            auto idk = global->tmp;

            if(!idk) return;

            idk->setPositionX(this->m_player1->m_position.x);
            idk->setPositionY(this->m_player1->m_position.y + 15.f);
            idk->setColor(this->m_player1->getColor());
            idk->setID(this->m_player1->getID());
            idk->setRotation(this->m_player1->getRotation());

            auto gamemode = getGamemodeFromPlayer(m_player1);

            auto gameerrr = GameManager::sharedState();

            int iconID = 0;
            if (gamemode == Gamemode::SHIP) {
                iconID = gameerrr->getPlayerShip();
            } else if (gamemode == Gamemode::BALL) {
                iconID = gameerrr->getPlayerBall();
            } else if (gamemode == Gamemode::UFO) {
                iconID = gameerrr->getPlayerBird();
            } else if (gamemode == Gamemode::WAVE) {
                iconID = gameerrr->getPlayerDart();
            } else if (gamemode == Gamemode::ROBOT) {
                iconID = gameerrr->getPlayerRobot();
            } else if (gamemode == Gamemode::SPIDER) {
                iconID = gameerrr->getPlayerSpider();
            } else {
                iconID = gameerrr->getPlayerFrame();
            }

            idk->updatePlayerFrame(iconID, getIconType(gamemode));

            //this->m_player1->playDeathEffect();

            idk->setFlipY(true);

            idk->setScale(2.f);
        }
};
