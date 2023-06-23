#include "include.hpp"
#include "utils.hpp"

std::vector<std::function<void()>> functionQueue;
std::mutex threadMutex;

void executeInGDThread(std::function<void()> f) {
    std::lock_guard<std::mutex> lock(threadMutex);
    functionQueue.push_back(std::move(f));
}

class $modify(cocos2d::CCScheduler) {
    void update(float dt) {
        CCScheduler::update(dt);

        threadMutex.lock();
        auto buffer = std::move(functionQueue);
        threadMutex.unlock();

        for (auto &f: buffer)
            f();
    }
};

class $modify(MenuLayer) {
    void onMoreGames(cocos2d::CCObject* p0) {
        Global *g = Global::get();

        ENetAddress addr;
        enet_address_set_host(&addr, "127.0.0.1");
        addr.port = 34154;

        g->peer = enet_host_connect(g->host, &addr, 1, 0);
        if (!g->peer) {
            fmt::print("failed to connect to server!!\n");
            return;
        }
        fmt::print("peer id: {} {}\n", g->peer->incomingPeerID, g->peer->outgoingPeerID);
    }
};

class $modify(PlayLayer) {
        bool init(GJGameLevel *level) {
            if (!PlayLayer::init(level)) return false;

            Global* global = Global::get();

            fmt::print("level id {}\n", level->m_levelID.value());

            if (global->connected) {
                int32_t level_id = level->m_levelID;

                gdmp::Packet packet;

                auto gameManager = GameManager::sharedState();

                auto visuals = new gdmp::PlayerVisuals();
                visuals->set_icon_cube(gameManager->getPlayerFrame());
                visuals->set_icon_ship(gameManager->getPlayerShip());
                visuals->set_icon_ball(gameManager->getPlayerBall());
                visuals->set_icon_ufo(gameManager->getPlayerBird());
                visuals->set_icon_wave(gameManager->getPlayerDart());
                visuals->set_icon_robot(gameManager->getPlayerRobot());
                visuals->set_icon_spider(gameManager->getPlayerSpider());

                gdmp::Colors color_p1;
                gdmp::Colors color_p2;

#if __APPLE__ && TARGET_OS_MAC
                // PlayerObject::getSecondaryColor has an binding on mac but not anywhere else and i cba to find it lmao
    auto secondaryColor = getIntFromCCColor(p1->getSecondaryColor());
#else
                uint32_t secondaryColor = 0;

                // hope this works c:
                if (this->m_player1->m_iconSprite) {
                    secondaryColor = getIntFromCCColor(this->m_player1->m_iconSprite->getColor());
                }
#endif

                color_p1.set_primary(getIntFromCCColor(this->m_player1->getColor()));
                color_p1.set_secondary(secondaryColor);

                auto colors = new gdmp::ColorInfo();
                colors->set_allocated_color_p1(&color_p1);
                colors->set_allocated_color_p2(&color_p1); // todo: use p2 colors

                colors->set_glowy(gameManager->getPlayerGlow());

                visuals->set_allocated_colors(colors);

                gdmp::Room room;
                room.set_level_id(level_id);

                auto player_join = new gdmp::PlayerJoinPacket();
                player_join->set_allocated_room(&room);
                player_join->set_allocated_visual(visuals);

                packet.set_allocated_player_join(player_join);

                size_t size = packet.ByteSizeLong();
                void *buffer = malloc(size);
                packet.SerializeToArray(buffer, size);
                auto enetPacket = enet_packet_create(buffer, size, ENET_PACKET_FLAG_RELIABLE);
                free(buffer);

                enet_peer_send(global->peer, 0, enetPacket);
            }

            return true;
        }

        void onQuit() {
            Global* global = Global::get();
            for (auto &item: global->players) {
                item.second.p1->removeMeAndCleanup();
            }
            global->players.clear();

            if (global->connected) {
                int32_t level_id = this->m_level->m_levelID;
                gdmp::Packet packet;

                gdmp::Room room;
                room.set_level_id(level_id);

                auto leavePacket = new gdmp::PlayerLeavePacket();
                leavePacket->set_allocated_room(&room);

                packet.set_allocated_player_leave(leavePacket);

                size_t size = packet.ByteSizeLong();
                void *buffer = malloc(size);
                packet.SerializeToArray(buffer, size);
                auto enetPacket = enet_packet_create(buffer, size, ENET_PACKET_FLAG_RELIABLE);
                free(buffer);

                enet_peer_send(global->peer, 0, enetPacket);
            }

            PlayLayer::onQuit();
        }

        void update(float p0) {
            PlayLayer::update(p0);

            Global* global = Global::get();

            if (global->connected) {
                gdmp::Packet packet;

                // player move
                auto player_move = new gdmp::PlayerMovePacket();

                auto pos1 = getPositionDataFromPlayer(this->m_player1);
                player_move->set_allocated_pos_p1(&pos1);

                auto gameMode1 = getGameModeFromGamemode(getGamemodeFromPlayer(this->m_player1));
                player_move->set_gamemode_p1(gameMode1);

                if (this->m_player2 && this->m_isDualMode) {
                    auto pos2 = getPositionDataFromPlayer(this->m_player2);
                    player_move->set_allocated_pos_p2(&pos2);

                    auto gameMode2 = getGameModeFromGamemode(getGamemodeFromPlayer(this->m_player2));
                    player_move->set_gamemode_p2(gameMode2);
                }

                packet.set_allocated_player_move(player_move);

                size_t size = packet.ByteSizeLong();
                void *buffer = malloc(size);
                packet.SerializeToArray(buffer, size);
                auto enetPacket = enet_packet_create(buffer, size, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT); /* is this how u do unreliable packets? */
                free(buffer);

                enet_peer_send(global->peer, 0, enetPacket);
            }
        }
};
