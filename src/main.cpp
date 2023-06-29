#include "include.hpp"
#include "global.hpp"
#include "hooks.hpp"

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
                    auto *gdmp_packet = new gdmp::Packet();
                    gdmp_packet->ParseFromArray(event.packet->data, event.packet->dataLength);

                    if (gdmp_packet->has_player_join()) {
                        fmt::print("a player has suddenly appeared!!\n");
                        auto player_join = gdmp_packet->player_join();

                        executeInGDThread([player_join]() {
                            auto playLayer = GameManager::sharedState()->getPlayLayer();
                            if (!playLayer) return;

                            const auto &visuals = player_join.visual();

                            auto p1 = PlayerObject::create(1, 1, 0);
                            p1->setPosition({0, 0});

                            auto col_primary_p1 = visuals.colors().color_p1().primary();
                            auto col_secondary_p1 = visuals.colors().color_p1().secondary();
                            auto glowy = visuals.colors().glowy();

                            p1->setColor(ccc3((col_primary_p1 >> 16) & 0xff,
                                              (col_primary_p1 >> 8) & 0xff,
                                              col_primary_p1 & 0xff
                            ));
                            p1->setSecondColor(ccc3((col_secondary_p1 >> 16) & 0xff,
                                                    (col_secondary_p1 >> 8) & 0xff,
                                                    col_secondary_p1 & 0xff
                            ));
                            p1->m_iconGlow->setVisible(glowy);

                            playLayer->getObjectLayer()->addChild(p1);

                            Player p{
                                    p1,
                                    nullptr,
                                    visuals.icon_cube(),
                                    visuals.icon_ship(),
                                    visuals.icon_ball(),
                                    visuals.icon_ufo(),
                                    visuals.icon_wave(),
                                    visuals.icon_robot(),
                                    visuals.icon_spider()
                            };

                            Global::get()->players[player_join.p_id()] = p;
                        });

                    } else if (gdmp_packet->has_player_move()) {
                        auto player_move = gdmp_packet->player_move();

                        auto global = Global::get();

                        auto p_id = player_move.p_id();
                        auto playLayer = GameManager::sharedState()->getPlayLayer();
                        if (!playLayer || !global->players.contains(p_id)) {
                            continue;
                        }

                        auto p = global->players[p_id];

                        auto p1 = p.p1;

                        auto pos_p1_x = player_move.pos_p1().pos_x();
                        auto pos_p1_y = player_move.pos_p1().pos_y();
                        auto rot_p1 = player_move.pos_p1().rotation();
                        auto scale_p1 = player_move.pos_p1().scale();
                        auto button_p1 = player_move.pos_p1().pressed();
                        auto iconType_p1 = getIconType(getGamemodeFromGameMode(player_move.gamemode_p1()));
                        auto iconID_p1 = 0;
                        switch (iconType_p1) {
                            case IconType::Cube: {
                                iconID_p1 = p.cube;
                                p1->toggleFlyMode(false);
                                p1->toggleBirdMode(false);
                                p1->toggleRollMode(false);
                                p1->toggleDartMode(false);
                                p1->toggleRobotMode(false);
                                p1->toggleSpiderMode(false);
                                break;
                            }
                            case IconType::Ship: {
                                iconID_p1 = p.ship;
                                p1->toggleFlyMode(true);
                                break;
                            }
                            case IconType::Ball: {
                                iconID_p1 = p.ball;
                                p1->toggleRollMode(true);
                                break;
                            }
                            case IconType::Ufo: {
                                iconID_p1 = p.ufo;
                                p1->toggleBirdMode(true);
                                break;
                            }
                            case IconType::Wave:
                                iconID_p1 = p.wave;
                                p1->toggleDartMode(true);
                                break;
                            case IconType::Robot:
                                iconID_p1 = p.robot;
                                p1->toggleRobotMode(true);
                                break;
                            case IconType::Spider:
                                iconID_p1 = p.spider;
                                p1->toggleSpiderMode(true);
                                break;
                            case IconType::DeathEffect:
                            case IconType::Special:
                                break;
                        }

                        if (scale_p1 > 1.0f || scale_p1 < 0.0f) scale_p1 = 1.0f;

                        executeInGDThread([pos_p1_x, pos_p1_y, rot_p1, scale_p1, button_p1, p1]() {
                            p1->update(1.0 / 60.0);
                            p1->setPosition({pos_p1_x, pos_p1_y});
                            p1->setRotation(rot_p1);
                            p1->setScale(scale_p1);
                            if (!Global::get()->P1_pushing && button_p1) {
                                p1->pushButton(1);
                            } else if (Global::get()->P1_pushing && !button_p1) {
                                p1->releaseButton(1);
                            }
                            //p1->updatePlayerFrame(iconID_p1, iconType_p1);
                        });
                    } else if (gdmp_packet->has_player_leave()) {
                        fmt::print(":vanish:\n");
                        auto global = Global::get();

                        auto player_leave = gdmp_packet->player_leave();
                        auto p_id = player_leave.p_id();

                        if (!global->players.contains(p_id))
                            continue;

                        auto p = global->players[p_id];

                        if (p.p1) p.p1->removeMeAndCleanup();
                        if (p.p2) p.p2->removeMeAndCleanup();
                        global->players.erase(p_id);
                    } else {
                        fmt::print("wtf how\n");
                    }

                    break;
                }
                case ENET_EVENT_TYPE_CONNECT: {
                    g->connected = true;
                    executeInGDThread([]() {
                        Notification::create("Connected to the server!", NotificationIcon::Success)->show();
                    });
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    g->connected = false;
                    g->peer = nullptr;
                    executeInGDThread([]() {
                        Notification::create("Disconnected from the server!", NotificationIcon::Error)->show();
                    });
                    break;
                }
                case ENET_EVENT_TYPE_NONE: {
                    break;
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

    Global *g = Global::get();
    g->host = enet_host_create(nullptr, 1, 1, 0, 0);

    // real..
    std::thread eventThread(&pollEvent);
    eventThread.detach();
}