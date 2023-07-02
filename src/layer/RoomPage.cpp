#include "layer/MultiplayerLayer.hpp"

static RoomPage* create(Room roomInfo, int state) {
    auto node = new RoomPage;
    if (node && node->init(roomInfo, state)) {
        node->autorelease();
    }
    else {
        CC_SAFE_DELETE(node);
    }
    return node;
}

bool RoomPage::init(Room roomInfo, int state) {
    auto win_size = cocos2d::CCDirector::sharedDirector()->getWinSize();

    if (!initWithColor({0, 0, 0, 105})) return false;
    auto m_pLayer = CCLayer::create();
    addChild(m_pLayer);

    auto bg =
        cocos2d::extension::CCScale9Sprite::create("GJ_square01.png", {0.0f, 0.0f, 80.0f, 80.0f});
    const CCSize window_size(400, 250);
    bg->setContentSize(window_size);
    bg->setPosition(win_size / 2);
    m_pLayer->addChild(bg);

    return true;
}
