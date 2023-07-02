/*
 * If you ever feel worthless, remember that I am more worthless than you
 */

#pragma once

#include "include.hpp"
#include "global.hpp"

using namespace geode::prelude;

struct Room {
    bool isPrivate;
    std::string roomCode;
    bool passProtected;

    int playerCapacity;
    int currentPlayers;

    int levelID;
    std::string hostName;
    std::string roomName;
};

class MultiplayerLayer : public cocos2d::CCLayer {
protected:
    virtual bool init();

    virtual void update(float);

    virtual void keyBackClicked();

    void backButtonCallback(cocos2d::CCObject *);

    void connectButtonCallback(cocos2d::CCObject *);

    void disconnectButtonCallback(cocos2d::CCObject *);

    cocos2d::CCArray *getRoomListing();

    cocos2d::CCLabelBMFont *connectionStatus;
    CCTextInputNode *ipInput;
    CCTextInputNode *portInput;

    Room currentRoom;
public:
    static MultiplayerLayer *create();

    void switchToCustomLayerButton(cocos2d::CCObject *);
};

class RoomListCell : public CCLayer, public FLAlertLayerProtocol {
protected:
    float m_width;
    float m_height;
    CCLayer *m_layer;

    bool init(CCLayer *layer, CCSize const &size);

    void draw() override;
};

class RoomCell : public RoomListCell {
protected:
    Room roomInfo;
    CCLabelBMFont *roomName;
    CCLabelBMFont *hostName;
    CCLayer *theLayer;

    MultiplayerLayer *theOriginalLayer;

    bool init(Room info, MultiplayerLayer *originalLayer, CCSize const &size, bool selected);

    virtual void FLAlert_Clicked(FLAlertLayer *, bool btn2);

public:
    static RoomCell *create(Room info, MultiplayerLayer *originalLayer, CCSize const &size, bool selected);

    void onSet(CCObject *);

    void deleteSong(CCObject *);
};

class RoomPage : public FLAlertLayer {
public:
    CCLabelBMFont *m_path_label;
    Room room;

    static RoomPage *create(Room roomInfo, int state);

    bool init(Room roomInfo, int state);
};