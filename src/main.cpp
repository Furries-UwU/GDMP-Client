#include "include.hpp"
#include "global.hpp"
#include "hooks.hpp"
#include "net.hpp"

using namespace geode::prelude;

[[noreturn]] void pollEvent() {
    while (true) {
        ENetEvent event;
        Global *g = Global::get();
        while (enet_host_service(Global::get()->host, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_RECEIVE: {
                    // todo!!
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
