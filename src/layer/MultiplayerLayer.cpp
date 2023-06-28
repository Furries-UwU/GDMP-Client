#include "layer/MultiplayerLayer.hpp"

using namespace geode::prelude;

// TODO: Rewrite this entire thing

void MultiplayerLayer::disconnectButtonCallback(CCObject *object) {
    Global *global = Global::get();

    if (global->connected) enet_peer_disconnect(global->peer, 0);
}

void MultiplayerLayer::connectButtonCallback(CCObject *object) {
    Global *global = Global::get();

    if (global->connected) enet_peer_disconnect(global->peer, 0);

    ENetAddress addr;
    enet_address_set_host(&addr, ipInput->getString());
    addr.port = std::stoi(portInput->getString());

    global->peer = enet_host_connect(global->host, &addr, 1, 0);
    if (!global->peer) {
        FLAlertLayer::create(
                "Error",
                "Failed to connect to the server",
                "OK"
        )->show();
        return;
    };
}

MultiplayerLayer *MultiplayerLayer::create() {
    auto ret = new MultiplayerLayer();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool MultiplayerLayer::init() {
    if (!CCLayer::init())
        return false;

    Global *global = Global::get();

    auto director = CCDirector::get();
    auto winSize = director->getWinSize();

    auto label = CCLabelBMFont::create("Multiplayer", "bigFont.fnt");
    label->setPosition(ccp(director->getWinSize().width / 2,
                           director->getWinSize().height - 25));
    addChild(label);

    auto ipLabel = CCLabelBMFont::create("IP", "bigFont.fnt");
    ipLabel->setPosition(ccp((director->getWinSize().width / 2) - 150,
                             (director->getWinSize().height / 2) + 50));
    addChild(ipLabel);

    auto portLabel = CCLabelBMFont::create("Port", "bigFont.fnt");
    portLabel->setPosition(ccp((director->getWinSize().width / 2) + 150,
                               (director->getWinSize().height / 2) + 50));
    addChild(portLabel);

    ipInput = CCTextInputNode::create(100, 100, "IP", "bigFont.fnt");
    ipInput->setPosition(ccp((director->getWinSize().width / 2) - 150,
                             director->getWinSize().height / 2));
    ipInput->setAllowedChars("0123456789abcdefghijklmnopqrstuvwxyz.-");
    ipInput->setString("127.0.0.1");
    addChild(ipInput);

    portInput = CCTextInputNode::create(100, 100, "Port", "bigFont.fnt");
    portInput->setPosition(ccp((director->getWinSize().width / 2) + 150,
                               director->getWinSize().height / 2));
    portInput->setAllowedChars("0123456789");
    portInput->setString("34154");
    addChild(portInput);

    connectionStatus = CCLabelBMFont::create(
            fmt::format("Status: {}", global->connected ? "Connected" : "Not connected").c_str(), "bigFont.fnt");
    connectionStatus->setPosition(ccp((director->getWinSize().width / 2), (director->getWinSize().height / 2) - 120));
    connectionStatus->setScaleX(0.5);
    connectionStatus->setScaleY(0.5);
    addChild(connectionStatus);

    auto disconnectButtonSprite = CCSprite::createWithSpriteFrameName("GJ_stopEditorBtn_001.png");

    auto connectButtonSprite = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
    connectButtonSprite->setScaleX(0.5);
    connectButtonSprite->setScaleY(0.5);

    auto disconnectButton = CCMenuItemSpriteExtra::create(
            disconnectButtonSprite,
            this,
            menu_selector(MultiplayerLayer::disconnectButtonCallback)
    );

    auto connectButton = CCMenuItemSpriteExtra::create(
            connectButtonSprite,
            this,
            menu_selector(MultiplayerLayer::connectButtonCallback)
    );

    auto backButton = CCMenuItemSpriteExtra::create(
            CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
            this,
            menu_selector(MultiplayerLayer::backButtonCallback)
    );

    auto backgroundSprite = CCSprite::create("GJ_gradientBG.png");
    auto backgroundSize = backgroundSprite->getContentSize();

    backgroundSprite->setScaleX(winSize.width / backgroundSize.width);
    backgroundSprite->setScaleY(winSize.height / backgroundSize.height);
    backgroundSprite->setAnchorPoint({0, 0});
    backgroundSprite->setColor({0, 0, 0});

    backgroundSprite->setZOrder(-1);
    addChild(backgroundSprite);

    auto menu = CCMenu::create();
    menu->addChild(backButton);
    menu->setPosition({25, winSize.height - 25});
    addChild(menu);

    menu = CCMenu::create();
    menu->addChild(connectButton);
    menu->setPosition({(winSize.width / 2) - 50, winSize.height / 2 - 75});
    addChild(menu);

    menu = CCMenu::create();
    menu->addChild(disconnectButton);
    menu->setPosition({(winSize.width / 2) + 50, winSize.height / 2 - 75});
    addChild(menu);

    setKeypadEnabled(true);

    scheduleUpdate();

    return true;
}

void exitMenu() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

void MultiplayerLayer::keyBackClicked() {
    exitMenu();
}

void MultiplayerLayer::backButtonCallback(CCObject *object) {
    exitMenu();
}

void MultiplayerLayer::switchToCustomLayerButton(CCObject *object) {
    auto layer = MultiplayerLayer::create();
    auto scene = CCScene::create();
    scene->addChild(layer);

    CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, scene));
}

void MultiplayerLayer::update(float dt) {
    Global *global = Global::get();

    connectionStatus->setString(fmt::format("Status: {}", global->connected ? "Connected" : "Not connected").c_str());
}