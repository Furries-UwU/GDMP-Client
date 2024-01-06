/*
 * Rust support in Geode when?
 */

#include "global.hpp"
#include "hooks.hpp"
#include "include.hpp"

#if __APPLE__ && TARGET_OS_MAC
    #include "thread"
#endif

using namespace geode::prelude;

[[noreturn]] void pollEvent() {
    while (true) {
        ENetEvent event;
        while (enet_host_service(Global::get()->host, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE: {
                    auto* gdmp_packet = new gdmp::Packet();
                    gdmp_packet->ParseFromArray(event.packet->data, event.packet->dataLength);

                    if (gdmp_packet->has_player_join()) {
                        fmt::print("a player has suddenly appeared!!\n");
                        auto player_join = gdmp_packet->player_join();

                        executeInGDThread([player_join]() {
                            auto playLayer = GameManager::sharedState()->getPlayLayer();
                            auto gameLayer = GameManager::sharedState()->getGameLayer();
                            if (!playLayer) return;
                            if (!gameLayer) return;

                            const auto& visuals = player_join.visual();

                            auto p1 = PlayerObject::create(1, 1, gameLayer, gameLayer, false); // meow , (idk)
                            p1->setPosition({0, 105}); // todo

                            auto col_primary_p1 = visuals.colors().color_p1().primary();
                            auto col_secondary_p1 = visuals.colors().color_p1().secondary();
                            auto glowy = visuals.colors().glowy();

                            /*p1->setColor(ccc3(
                                (col_primary_p1 >> 16) & 0xff,
                                (col_primary_p1 >> 8) & 0xff,
                                col_primary_p1 & 0xff
                            ));*/ // todo
                            p1->setSecondColor(ccc3(
                                (col_secondary_p1 >> 16) & 0xff,
                                (col_secondary_p1 >> 8) & 0xff,
                                col_secondary_p1 & 0xff
                            ));
                            //p1->m_iconGlow->setVisible(glowy); // todo

                            playLayer->m_objectLayer->addChild(p1);

                            Player p{
                                p1,
                                nullptr,
                                visuals.icon_cube(),
                                visuals.icon_ship(),
                                visuals.icon_ball(),
                                visuals.icon_ufo(),
                                visuals.icon_wave(),
                                visuals.icon_robot(),
                                visuals.icon_spider()};

                            p1->updatePlayerBirdFrame(visuals.icon_ufo());
                            p1->updatePlayerDartFrame(visuals.icon_wave());
                            p1->updatePlayerRollFrame(visuals.icon_ball());
                            p1->updatePlayerShipFrame(visuals.icon_ship());
                            p1->updatePlayerFrame(visuals.icon_cube());
                            //p1->updatePlayerRobotFrame(visuals.icon_robot()); // todo
                            //p1->updatePlayerSpiderFrame(visuals.icon_spider()); // todo

                            Global::get()->players[player_join.p_id()] = p;
                        });
                    }
                    else if (gdmp_packet->has_player_move()) {
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
                        auto iconType_p1 =
                            getIconType(getGamemodeFromGameMode(player_move.gamemode_p1()));

                        // TODO: i have no idea what the 2nd bool does
                        p1->toggleFlyMode(iconType_p1 == IconType::Ship, false);
                        p1->toggleRollMode(iconType_p1 == IconType::Ball, false);
                        p1->toggleBirdMode(iconType_p1 == IconType::Ufo, false);
                        p1->toggleDartMode(iconType_p1 == IconType::Wave, false);
                        p1->toggleRobotMode(iconType_p1 == IconType::Robot, false);
                        p1->toggleSpiderMode(iconType_p1 == IconType::Spider, false);

                        if (scale_p1 > 1.0f || scale_p1 < 0.0f) scale_p1 = 1.0f;

                        executeInGDThread([pos_p1_x, pos_p1_y, rot_p1, scale_p1, button_p1, p1]() {
                            auto global = Global::get();

                            p1->setPosition({pos_p1_x, pos_p1_y}); // todo
                            p1->setRotation(rot_p1); // todo
                            p1->setScale(scale_p1);
                            if (!global->P1_pushing && button_p1) {
                                p1->pushButton(static_cast<PlayerButton>(1));
                            }
                            else if (global->P1_pushing && !button_p1) {
                                p1->releaseButton(static_cast<PlayerButton>(1));
                            }
                        });
                    }
                    else if (gdmp_packet->has_player_leave()) {
                        fmt::print(":vanish:\n");

                        auto global = Global::get();

                        auto player_leave = gdmp_packet->player_leave();
                        auto p_id = player_leave.p_id();

                        if (!global->players.contains(p_id)) continue;

                        auto p = global->players[p_id];

                        if (p.p1) p.p1->removeMeAndCleanup();
                        if (p.p2) p.p2->removeMeAndCleanup();
                        global->players.erase(p_id);
                    }
                    else {
                        fmt::print("wtf how\n");
                    }

                    break;
                }
                case ENET_EVENT_TYPE_CONNECT: {
                    auto global = Global::get();

                    global->connected = true;

                    executeInGDThread([]() {
                        Notification::create("Connected to the server!", NotificationIcon::Success)
                            ->show();
                    });
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT: {
                    auto global = Global::get();

                    global->connected = false;
                    global->peer = nullptr;

                    executeInGDThread([]() {
                        Notification::create("Disconnected from the server!", NotificationIcon::Error)
                            ->show();
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

// clang-format off

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

    Global* global = Global::get();
    global->host = enet_host_create(nullptr, 1, 1, 0, 0);

    // real..
    std::thread eventThread(&pollEvent);
    eventThread.detach();
}

$on_mod(Unloaded) {
    fmt::print("unloading meow :3\n");
    Global* global = Global::get();
    if (global->peer)
        enet_peer_disconnect(global->peer, 0);
}

// clang-format on
