#pragma once

#include "include.hpp"
#include "global.hpp"

using namespace geode::prelude;

class MultiplayerLayer : public CCLayer {
protected:
    virtual bool init();

    virtual void update(float);

    virtual void keyBackClicked();

    void backButtonCallback(CCObject *);

    void connectButtonCallback(CCObject *);

    void disconnectButtonCallback(CCObject *);

    cocos2d::CCLabelBMFont *connectionStatus;
    CCTextInputNode *ipInput;
    CCTextInputNode *portInput;
public:
    static MultiplayerLayer *create();

    void switchToCustomLayerButton(CCObject *);
};