/*
 * When people ask "why did you make this mod?", I tell 'em, "for fun"
 */

#include "layer/MultiplayerLayer.hpp"

using namespace geode::prelude;

// TODO: Rewrite this entire thing

void MultiplayerLayer::disconnectButtonCallback(CCObject *object) {
    Global *global = Global::get();

    global->zmg_pull->close();
    global->zmg_push->close();
}

void MultiplayerLayer::connectButtonCallback(CCObject *object) {
    Global *global = Global::get();

    global->zmg_pull->close();
    global->zmg_push->close();

    auto addr = fmt::format("{}:{}", "rooot.gay", "34154");

    global->zmg_push->bind(addr);
    global->zmg_pull->connect(addr);
}

cocos2d::CCArray *MultiplayerLayer::getRoomListing() {
    cocos2d::CCArray *array = cocos2d::CCArray::create();
    array->addObject(RoomCell::create({false, "i dont know", false, 10, 10, 736, "ninXout", "a room thats full"}, this,
                                      {400, 68}, false));
    array->addObject(
            RoomCell::create({false, "i dont know", false, 10, 4, 166, "ninXout", "a room that isn't full"}, this,
                             {400, 68}, false));
    return array;
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

    auto cells = this->getRoomListing();

    auto listLayer = CCLayer::create();
    auto list = ListView::create(
            cells,
            60,
            400,
            190
    );

    list->setPositionY(-10.f);

    auto sideLeft = CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
    sideLeft->setAnchorPoint(ccp(0, 0));
    sideLeft->setScaleY(2.475f);
    sideLeft->setScaleX(1.2f);
    sideLeft->setPosition({-30, 24});
    sideLeft->setZOrder(9);

    auto sideTop = CCSprite::createWithSpriteFrameName("GJ_table_top02_001.png");
    sideTop->setAnchorPoint(ccp(0, 0));
    sideTop->setScaleX(1.15f);
    sideTop->setScaleY(1.5f);
    sideTop->setPosition(ccp(-29.f, 164.f));
    sideTop->setZOrder(9);

    auto sideBottom = CCSprite::createWithSpriteFrameName("GJ_table_bottom_001.png");
    //sideBottom->setFlipY(true);
    sideBottom->setAnchorPoint(ccp(0, 0));
    sideBottom->setPosition(ccp(-24.f, -17.f));
    sideBottom->setScaleX(1.14f);
    sideBottom->setZOrder(10);

    auto sideRight = CCSprite::createWithSpriteFrameName("GJ_table_side_001.png");
    sideRight->setFlipX(true);
    sideRight->setScaleY(2.475f);
    sideRight->setScaleX(1.2f);
    sideRight->setAnchorPoint(ccp(0, 0));
    sideRight->setPosition({389, 22});
    sideRight->setZOrder(9);

    listLayer->addChild(sideLeft);
    listLayer->addChild(sideTop);
    listLayer->addChild(sideBottom);
    listLayer->addChild(sideRight);
    listLayer->addChild(list);
    listLayer->setPosition(winSize / 2 - list->getScaledContentSize() / 2);
    addChild(listLayer);

    auto label = CCLabelBMFont::create("Multiplayer", "goldFont.fnt");
    label->setPosition(ccp(director->getWinSize().width / 2,
                           director->getWinSize().height - 25));
    addChild(label);
/*
    auto ipLabel = CCLabelBMFont::create("IP", "bigFont.fnt");
    ipLabel->setPosition(ccp((director->getWinSize().width / 2) - 150,
                             (director->getWinSize().height / 2) + 50));
    addChild(ipLabel);

    auto portLabel = CCLabelBMFont::create("Port", "bigFont.fnt");
    portLabel->setPosition(ccp((director->getWinSize().width / 2) + 150,
                               (director->getWinSize().height / 2) + 50));
    addChild(portLabel);

    ipInput = CCTextInputNode::create(100, 100, "Address", "bigFont.fnt");
    ipInput->setPosition(ccp((director->getWinSize().width / 2) - 150,
                             director->getWinSize().height / 2));
    ipInput->setAllowedChars("0123456789abcdefghijklmnopqrstuvwxyz.-");
    ipInput->setString("rooot.gay");
    addChild(ipInput);

    portInput = CCTextInputNode::create(100, 100, "Port", "bigFont.fnt");
    portInput->setPosition(ccp((director->getWinSize().width / 2) + 150,
                               director->getWinSize().height / 2));
    portInput->setAllowedChars("0123456789");
    portInput->setString("34154");
    addChild(portInput);
*/
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

    auto backgroundSprite = CCSprite::create("game_bg_01_001-uhd.png");
    auto backgroundSize = backgroundSprite->getContentSize();

    backgroundSprite->setScaleX(winSize.width / backgroundSize.width);
    backgroundSprite->setScaleY(winSize.height / backgroundSize.height);
    backgroundSprite->setAnchorPoint({0, 0});
    backgroundSprite->setColor({40, 125, 255});

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