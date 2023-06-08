#include "include.hpp"
#include "utils.hpp"
#include "proto/packet.pb.h"

class $modify(MenuLayer) {
    void onMoreGames(cocos2d::CCObject* p0) {
        Global *g = Global::get();

        fmt::print("meow meeow!\n");

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
                fmt::print("making join packet!!");
                int32_t level_id = level->m_levelID;

                gdmp::Packet packet;
                packet.set_packet_type(4); // 4 = join ig

                fmt::print("getting visuals!!\n");
                auto visual = *getPlayerVisualData(this->m_player1, this->m_player2);
                fmt::print("got visuals!!\n");

                gdmp::Room room;
                room.set_level_id(level_id);

                auto player_join = new gdmp::PlayerJoinPacket();
                player_join->set_allocated_room(&room);
                player_join->set_allocated_visual(&visual);

                packet.set_allocated_player_join(player_join);

                // ugly shit stupid code vvvvv -----
                size_t size = packet.ByteSizeLong();
                void *buffer = malloc(size); /* manual memory allocation my beloved <3 */
                packet.SerializeToArray(buffer, size);
                auto enetpacket = enet_packet_create(buffer, size, ENET_PACKET_FLAG_RELIABLE);
                free(buffer); /* I AM FREEEEEEEEEEEEEE */

                enet_peer_send(global->peer, 0, enetpacket);
                //enet_packet_destroy(enetpacket);
                /* this causes the packet to not get sent fully because you're not supposed to call enet_packet_destroy
                right after sending the packet */
            }

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

            gdmp::Packet packet;
            packet.set_packet_type(5);

            // player move
            auto player_move = new gdmp::PlayerMovePacket();

            auto pos1 = getPositionDataFromPlayer(this->m_player1);
            player_move->set_allocated_pos_p1(&pos1);

            if (this->m_player2 && this->m_isDualMode) {
                auto pos2 = getPositionDataFromPlayer(this->m_player2);
                player_move->set_allocated_pos_p2(&pos2);
            }

            packet.set_allocated_player_move(player_move);

            // ugly shit stupid code vvvvv -----
            size_t size = packet.ByteSizeLong();
            void *buffer = malloc(size); /* manual memory allocation my beloved <3 */
            packet.SerializeToArray(buffer, size);
            auto enetpacket = enet_packet_create(buffer, size, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT); /* is this how u do unreliable packets? */
            free(buffer); /* I AM FREEEEEEEEEEEEEE */

            enet_peer_send(global->peer, 0, enetpacket);
        }
};
