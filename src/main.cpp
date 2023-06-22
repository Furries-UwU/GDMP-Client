#include "include.hpp"
#include "global.hpp"
#include "hooks.hpp"
#include "net.hpp"
#include "proto/packet.pb.h"

#if __APPLE__ && TARGET_OS_MAC
#include "thread"
#endif

using namespace geode::prelude;

[[noreturn]] void pollEvent() {
    while (true) {
        ENetEvent event;
        Global *g = Global::get();
        while (enet_host_service(Global::get()->host, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE: {
                    // todo!!
                    fmt::print("recv'd packet with length {}\n", event.packet->dataLength);

                    auto* gdmp_packet = new gdmp::Packet();
                    gdmp_packet->ParseFromArray(event.packet->data, event.packet->dataLength);

                    fmt::print("parsed packet, somehow.. type: {}\n", gdmp_packet->packet_type());

                    if (gdmp_packet->has_player_join()) {
                        fmt::print("a player has suddenly appeared!!\n");

                        auto player_join = gdmp_packet->player_join();
                        fmt::print("-> player id {} joined room {}\n", player_join.p_id(), player_join.room().level_id());

                        // todo: handle join
                    } else if (gdmp_packet->has_player_move()) {
                        fmt::print("*player moving noises*\n");

                        auto player_move = gdmp_packet->player_move();
                        fmt::print("-> player id {} moved to P1[{} {}] P2[{} {}]\n", player_move.p_id(),
                                   player_move.pos_p1().pos_x(),player_move.pos_p1().pos_y(),
                                   player_move.pos_p2().pos_x(), player_move.pos_p2().pos_y());

                        // todo: handle move
                    } else if (gdmp_packet->has_player_leave()) {
                        fmt::print(":vanish:\n");
                        // todo: handle leave
                    } else {
                        fmt::print("wtf how\n");
                    }

                    break;
                }
                case ENET_EVENT_TYPE_CONNECT: {
                    g->connected = true;
                    fmt::print("connected to port {}\n", Global::get()->host->address.port);

                    //Notification::create("connected!!", NotificationIcon::Error)->show();
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    g->connected = false;
                    // todo!!

                    break;
                }
                case ENET_EVENT_TYPE_NONE: {
                    fmt::print("meow\n");
                    // :shrug:
                }
            }
        }
    }
}

$execute {
    fmt::print("loading meow :3\n");
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    if (enet_initialize() != 0) {
        fmt::print("failed to init enet!!\n");
        return;
    } else {
        fmt::print("enet init success!!\n");
    }
    // make sure to clean up!!
    atexit(enet_deinitialize);

    Global* g = Global::get();
    g->host = enet_host_create(nullptr, 1, 1, 0, 0);

    fmt::print("attempting to connect to server!!\n");

    // real..
    std::thread eventThread(&pollEvent);
    eventThread.detach();
}
