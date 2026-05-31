#pragma once
#include "Syati.h"

class DesertLandMoveSwitch : public LiveActor {
public:
    DesertLandMoveSwitch(const char* pName);

    virtual void init(const JMapInfoIter& rIter);
    virtual void initAfterPlacement();
    virtual void calcAnim();
    virtual void control();
    virtual void calcAndSetBaseMtx();
    virtual bool receiveMsgPlayerAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver);
    virtual bool receiveOtherMsg(u32 msg, HitSensor* pSender, HitSensor* pReceiver);
    void initModelAndCollision(const JMapInfoIter& rIter);
    bool tryOn();
    bool trySwitchDown();
    bool tryConnect();
    void updateTimerSE();
    void exeWait();
    void exeSwitchDown();
    void exeOn();
    void exeReturn();

    CollisionParts* mCollisionParts;
    /*
    f32 _0; -> mRestValue
	f32 _4; -> mSpringValue (// 0x90)
	f32 _8; -> mAccelFactor
	f32 _C; -> mDamping
	f32 _10; -> mVelocity
    */
    SpringValue* mSpringValue;          
    MapObjConnector* mMapObjConnector;  // 0x94
    bool _98;
    bool _99;
    bool _9A;
    s32 _9C;
    const char* _A0;
};

namespace MR {
    bool isMsgHipDropFloor(u32);
}

//const char* gDesertLandMoveSwitchvar = 0; why the fuck is this here?