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
                    //fmt::print("recv'd packet with length {}\n", event.packet->dataLength);

                    auto* gdmp_packet = new gdmp::Packet();
                    gdmp_packet->ParseFromArray(event.packet->data, event.packet->dataLength);

                    //fmt::print("parsed packet, somehow.. type: {}\n", gdmp_packet->packet_type());

                    if (gdmp_packet->has_player_join()) {
                        fmt::print("a player has suddenly appeared!!\n");

                        auto player_join = gdmp_packet->player_join();
                        fmt::print("-> player id {} joined room {}\n", player_join.p_id(), player_join.room().level_id());

                        // todo: handle join

                        executeInGDThread([player_join]() {
                            auto playLayer = GameManager::sharedState()->getPlayLayer();
                            if (!playLayer) return;
                            fmt::print("hi from gd thread, getting visuals rn\n");

                            auto visuals = player_join.visual();

                            auto p1 = SimplePlayer::create(visuals.icon_cube());
                            p1->setPosition({0, 0});

                            auto col_primary_p1 = visuals.colors().color_p1().primary();
                            auto col_secondary_p1 = visuals.colors().color_p1().secondary();


                            fmt::print("RECV SETCOLOR PRIMARY {}: {} {} {}\n",
                                       col_primary_p1,
                                       (col_primary_p1 >> 16) & 0xff,
                                       (col_primary_p1 >> 8) & 0xff,
                                       col_primary_p1  & 0xff
                                       );

                            p1->setColor(ccc3((col_primary_p1 >> 16) & 0xff,
                                              (col_primary_p1 >> 8) & 0xff,
                                              col_primary_p1  & 0xff
                                              ));
                            p1->setSecondColor(ccc3((col_secondary_p1 >> 16) & 0xff,
                                                    (col_secondary_p1 >> 8) & 0xff,
                                                    col_secondary_p1  & 0xff
                            ));

                            playLayer->getObjectLayer()->addChild(p1);

                            Player p {
                                    p1,
                                    nullptr,
                                    visuals.icon_cube(),
                                    visuals.icon_ship(),
                                    visuals.icon_ball(),
                                    visuals.icon_ufo(),
                                    visuals.icon_wave(),
                                    visuals.icon_robot(),
                                    visuals.icon_spider(),
                            };

                            Global::get()->players[player_join.p_id()] = p;

                            fmt::print("added to playlayer-\n");
                        });

                    } else if (gdmp_packet->has_player_move()) {
                        //fmt::print("*player moving noises*\n");

                        auto player_move = gdmp_packet->player_move();
                        fmt::print("-> player id {} moved to P1[{} {}] P2[{} {}]\n", player_move.p_id(),
                                   player_move.pos_p1().pos_x(),player_move.pos_p1().pos_y(),
                                   player_move.pos_p2().pos_x(), player_move.pos_p2().pos_y());

                        // todo: handle move

                        auto global = Global::get();

                        auto p_id = player_move.p_id();
                        auto playLayer = GameManager::sharedState()->getPlayLayer();
                        if (!playLayer) {
                            fmt::print("playlayer is null\n");
                            continue;
                        }
                        if (!global->players.contains(p_id)) {
                            fmt::print("player doesn't exist\n");
                            continue;
                        }

                        auto p = global->players[p_id];

                        auto p1 = p.p1;

                        auto pos_p1_x = player_move.pos_p1().pos_x();
                        auto pos_p1_y = player_move.pos_p1().pos_y();
                        auto rot_p1 = player_move.pos_p1().rotation();
                        auto scale_p1 = player_move.pos_p1().scale();
                        auto iconType_p1 = getIconType(getGamemodeFromGameMode(player_move.gamemode_p1()));
                        auto iconID_p1 = 0;
                        switch (iconType_p1) {
                            case IconType::Cube: {
                                iconID_p1 = p.cube;
                                break;
                            }
                            case IconType::Ship: {
                                iconID_p1 = p.ship;
                                break;
                            }
                            case IconType::Ball: {
                                iconID_p1 = p.ball;
                                break;
                            }
                            case IconType::Ufo: {
                                iconID_p1 = p.ufo;
                                break;
                            }
                            case IconType::Wave:
                                iconID_p1 = p.wave;
                                break;
                            case IconType::Robot:
                                iconID_p1 = p.robot;
                                break;
                            case IconType::Spider:
                                iconID_p1 = p.spider;
                                break;
                            case IconType::DeathEffect:
                            case IconType::Special:
                                break;
                        }

                        if (scale_p1 > 1.0f || scale_p1 < 0.0f) scale_p1 = 1.0f;

                        executeInGDThread([pos_p1_x, pos_p1_y, rot_p1, scale_p1, iconID_p1, iconType_p1, p1]() {
                            p1->setPosition({ pos_p1_x, pos_p1_y });
                            p1->setRotation(rot_p1);
                            p1->setScale(scale_p1);
                            p1->updatePlayerFrame(iconID_p1, iconType_p1);
                        });
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
