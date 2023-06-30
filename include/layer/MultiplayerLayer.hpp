/* ==================================================================
 * If you feels worthless, remember that I am more worthless than you
 * ==================================================================
 */

#pragma once

#include "include.hpp"
#include "global.hpp"

class MultiplayerLayer : public cocos2d::CCLayer {
protected:
    virtual bool init();

    virtual void update(float);

    virtual void keyBackClicked();

    void backButtonCallback(cocos2d::CCObject *);

    void connectButtonCallback(cocos2d::CCObject *);

    void disconnectButtonCallback(cocos2d::CCObject *);

    cocos2d::CCLabelBMFont *connectionStatus;
    CCTextInputNode *ipInput;
    CCTextInputNode *portInput;
public:
    static MultiplayerLayer *create();

    void switchToCustomLayerButton(cocos2d::CCObject *);
};