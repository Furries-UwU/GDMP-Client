#include "layer/MultiplayerLayer.hpp"

bool RoomListCell::init(CCLayer* layer, CCSize const& size) {
    m_width = size.width;
    m_height = size.height;
    m_layer = layer;
    this->setContentSize(size);
    return true;
}

void RoomListCell::draw() {
    reinterpret_cast<StatsCell*>(this)->StatsCell::draw();
}

bool RoomCell::init(Room info, MultiplayerLayer* originalLayer, CCSize const& size, bool selected) {
    if (!RoomListCell::init(originalLayer, size)) return false;

    this->roomInfo = info;
    this->theOriginalLayer = originalLayer;

    CCMenuItemSpriteExtra* button;

    if (selected) {
        auto sprite = ButtonSprite::create("View", "goldFont.fnt", "GJ_button_02.png");
        button = CCMenuItemSpriteExtra::create(sprite, this, menu_selector(RoomCell::onSet));
    }
    else {
        button = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("View"), this, menu_selector(RoomCell::onSet)
        );
    }
    button->setAnchorPoint(ccp(0.5f, 0.5f));
    button->setPositionX(24.f);

    auto menu = CCMenu::create();
    menu->addChild(button);

    menu->setAnchorPoint(ccp(0, 0));
    menu->setPosition(ccp(320.f, 29.f));
    this->addChild(menu);

    this->theLayer = CCLayer::create();

    this->roomName = CCLabelBMFont::create(roomInfo.roomName.c_str(), "bigFont.fnt");
    this->roomName->limitLabelWidth(240.f, 0.8f, 0.1f);
    this->roomName->setScale(0.6f);

    if (selected) {
        this->roomName->setColor(ccc3(188, 254, 206));
    }

    this->hostName = CCLabelBMFont::create(roomInfo.hostName.c_str(), "goldFont.fnt");
    this->hostName->limitLabelWidth(240.f, 0.7f, 0.1f);

    auto playerSprite = CCSprite::createWithSpriteFrameName("accountBtn_friends_001.png");
    playerSprite->setScale(0.4f);
    playerSprite->setPosition({102, 20});

    auto players =
        std::to_string(roomInfo.currentPlayers) + "/" + std::to_string(roomInfo.playerCapacity);
    auto playerData = CCLabelBMFont::create(players.c_str(), "bigFont.fnt");
    playerData->setScale(0.4f);
    playerData->setPosition({132, 20});

    auto lockSprite = CCSprite::createWithSpriteFrameName("GJLargeLock_001.png");
    lockSprite->setScale(0.2f);
    lockSprite->setPosition({162, 20});

    auto lockData =
        CCLabelBMFont::create(roomInfo.passProtected ? "Locked" : "Unlocked", "bigFont.fnt");
    lockData->setScale(0.4f);
    lockData->setPosition({202, 20});

    auto levelSprite = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
    levelSprite->setScale(0.2f);
    levelSprite->setPosition({250, 20});

    auto levelData = CCLabelBMFont::create(std::to_string(roomInfo.levelID).c_str(), "bigFont.fnt");
    levelData->setScale(0.4f);
    levelData->setPosition({272, 20});

    this->theLayer->addChild(this->hostName);
    this->theLayer->addChild(this->roomName);
    this->addChild(playerSprite);
    this->addChild(playerData);
    this->addChild(lockSprite);
    this->addChild(lockData);
    this->addChild(levelSprite);
    this->addChild(levelData);
    auto layout = ColumnLayout::create();
    layout->setAutoScale(false);
    layout->setAxisAlignment(AxisAlignment::Even);
    layout->setCrossAxisLineAlignment(AxisAlignment::Start);
    this->theLayer->setContentSize(ccp(240.f, this->getContentSize().height - 12.f));
    this->theLayer->setAnchorPoint(ccp(0.f, 0.f));
    this->theLayer->setPosition(ccp(12.f, 1.5f));
    this->theLayer->setLayout(layout);
    this->hostName->setPosition(this->hostName->getPositionX(), this->hostName->getPositionY() + 6);

    this->addChild(theLayer);
    return true;
}

void RoomCell::onSet(CCObject* target) {}

RoomCell* RoomCell::create(Room info, MultiplayerLayer* originalLayer, CCSize const& size, bool selected) {
    auto ret = new RoomCell();
    if (ret && ret->init(info, originalLayer, size, selected)) {
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void RoomCell::FLAlert_Clicked(FLAlertLayer* layer, bool btn2) {
    if (btn2) {
        // this->theOriginalLayer->deleteSong(this->m_Room);
    }
}