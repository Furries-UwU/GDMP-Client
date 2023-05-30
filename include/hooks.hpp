#include "include.hpp"

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
        }
};
