#include "DesertLandMoveSwitch.h"
#include "Game/LiveActor/HitSensor.h"
#include "Game/LiveActor/LiveActor.h"
#include "Game/MapObj/MapObjConnector.h"
#include "Game/Util/ActorMovementUtil.h"
#include "Game/Util/ActorSensorUtil.h"
#include "Game/Util/ActorSwitchUtil.h"
#include "Game/Util/DemoUtil.h"
#include "Game/Util/JMapInfo.h"
#include "Game/Util/JMapUtil.h"
#include "Game/Util/LiveActorUtil.h"
#include "Game/Util/ModelUtil.h"
#include "Game/Util/ObjUtil.h"
#include "Game/Util/PlayerUtil.h"
#include "Game/Util/SceneUtil.h"
#include "Game/Util/SoundUtil.h"
#include "Game/Util/SpringValue.h"
#include "JSystem/JGeometry/TMatrix.h"
#include "JSystem/JGeometry/TVec.h"

namespace NrvDesertLandMoveSwitch {
    FULL_NERVE(HostTypeWait, DesertLandMoveSwitch, Wait);
    FULL_NERVE(HostTypeSwitchDown, DesertLandMoveSwitch, SwitchDown);
    FULL_NERVE(HostTypeOn, DesertLandMoveSwitch, On);
    FULL_NERVE(HostTypeReturn, DesertLandMoveSwitch, Return);
};  // namespace NrvDesertLandMoveSwitch

DesertLandMoveSwitch::DesertLandMoveSwitch(const char* pName) : LiveActor(pName) {
    mCollisionParts = 0;
    mSpringValue = new SpringValue();
    mMapObjConnector = new MapObjConnector(this);
    _98 = 0;
    _99 = 0;
    _9A = 0;
    _9C = 0;
    _A0 = 0;
    mSpringValue->setParam(0.0f, 0.0f, 0.35f, 0.75f, 0.0f);
}

void DesertLandMoveSwitch::init(const JMapInfoIter& rIter) {
    MR::initDefaultPos(this, rIter);
    MR::getJMapInfoArg0NoInit(rIter, &_9C);
    s32 stack_C = -1;
    MR::getJMapInfoArg1NoInit(rIter, &stack_C);
    initModelAndCollision(rIter);
    MR::connectToSceneMapObj(this);
    if (stack_C == -1) {
        initNerve(&NrvDesertLandMoveSwitch::HostTypeWait::sInstance, 0);
    } else {
        MR::startBck(this, "On", 0);
        MR::setAllAnimFrameAtEnd(this, "On");
        initNerve(&NrvDesertLandMoveSwitch::HostTypeOn::sInstance, 0);
    }
    f32 stack_8 = -1.0f;
    MR::calcModelBoundingRadius(&stack_8, this);
    MR::setClippingTypeSphere(this, stack_8);
    MR::setGroupClipping(this, rIter, 0x10);
    initSound(4, 0, 0, 0);
    MR::needStageSwitchWriteA(this, rIter);
    MR::tryRegisterDemoCast(this, rIter);
    makeActorAppeared();
}

void DesertLandMoveSwitch::initAfterPlacement() {
    mMapObjConnector->attachToUnder();
}

void DesertLandMoveSwitch::calcAnim() {
    LiveActor::calcAnim();
    MtxPtr mtx = MR::getJointMtx(this, "Move");
    TPos3f mtx2;
    mtx2.setInline(mtx);
    f32 val = mSpringValue->_4;
    if (!MR::isNearZero(val, 0.001f)) {
        TVec3f stack_20;
        f32 f3 = mtx2[2][3];
        f32 f2 = mtx2[1][3];
        f32 f1 = mtx2[0][3];
        stack_20.set(f1, f2, f3); //stack_20.set< f32 >(f1, f2, f3);
        TVec3f stack_14, stack_8; //stack_8.setPS(stack_14);
        MR::calcUpVec(&stack_14, this);
        stack_8 = stack_14;
        stack_8.x *= val;
        stack_8.y *= val;
        stack_8.z *= val;
        JMathInlineVEC::PSVECAdd(stack_20, stack_8, stack_20);
        mtx2[0][3] = stack_20.x;
        mtx2[1][3] = stack_20.y;
        mtx2[2][3] = stack_20.z;
        PSMTXCopy(mtx2, mtx);
    }
    mCollisionParts->setMtx(mtx2);
}

void DesertLandMoveSwitch::control() {
    _98 = 0;
    _99 = _9A;
    _9A = 0;
}

void DesertLandMoveSwitch::calcAndSetBaseMtx() {
    LiveActor::calcAndSetBaseMtx();
    mMapObjConnector->connect();
}

bool DesertLandMoveSwitch::receiveMsgPlayerAttack(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
    if (MR::isMsgPlayerHipDropFloor(msg) && !_98) {
        _98 = 1;
        return true;
    }
    return false;
}

bool DesertLandMoveSwitch::receiveOtherMsg(u32 msg, HitSensor* pSender, HitSensor* pReceiver) {
    if (msg == 0xB4) {
        _9A = 1;
        return _99 == 0;
    }
    return false;
}

void DesertLandMoveSwitch::initModelAndCollision(const JMapInfoIter& rIter) {
    const char* pObjName = "";
    MR::getObjectName(&pObjName, rIter);
    _A0 = pObjName;
    initModelManagerWithAnm(pObjName, 0, 0, false);
    initHitSensor(2);
    HitSensor* sensorBody = MR::addHitSensorMapObj(this, "body", 8, 0.0f, TVec3f(0.0f, 0.0f, 0.0f));
    HitSensor* sensorMove = MR::addHitSensorMapObj(this, "move", 8, 0.0f, TVec3f(0.0f, 0.0f, 0.0f));
    MR::initCollisionParts(this, pObjName, sensorBody, 0);
    mCollisionParts = MR::createCollisionPartsFromLiveActor(this, "Move", sensorMove, (MR::CollisionScaleType)2);
    MR::validateCollisionParts(mCollisionParts);
}

bool DesertLandMoveSwitch::tryOn() {
    if (MR::isGreaterStep(this, 6) && MR::isBckStopped(this)) {
        MR::startSound(this, "SE_OJ_HIPDROP_SWITCH_ON", -1, -1);
        MR::shakeCameraNormal();
        setNerve(&NrvDesertLandMoveSwitch::HostTypeOn::sInstance);
        return true;
    }
    return false;
}

bool DesertLandMoveSwitch::trySwitchDown() {
    if (!_98)
        return false;

    setNerve(&NrvDesertLandMoveSwitch::HostTypeSwitchDown::sInstance);
    return true;
}

bool DesertLandMoveSwitch::tryConnect() {
    if (mMapObjConnector->_4 != 0)
        return true;

    return mMapObjConnector->attachToUnder();
}

void DesertLandMoveSwitch::updateTimerSE() {
    if (!MR::isPlayerDead() && !MR::isPowerStarGetDemoActive()) {
        s32 l = _9C % 60;
        s32 step = getNerveStep();
        s32 lf = step - l;
        if (l != 0) {
            lf += 60;
        }
        s32 step2 = getNerveStep();
        if (MR::isEqualString(_A0, "DesertLandDownSwitch") && MR::isEqualStageName("SandClockGalaxy")) {
            lf += 180;
            step2 += 180;
        }
        if (lf >= 60) {
            s32 s = _9C;
            if (lf <= s) {
                if (step2 == s) {
                    MR::startSystemSE("SE_SY_TIMER_A_0", -1, -1);
                } else {
                    if (!(lf % 60)) {
                        if (lf < s - 600)
                            MR::startSystemSE("SE_SY_TIMER_A_2", -1, -1);
                        else if (lf < s - 120)
                            MR::startSystemSE("SE_SY_TIMER_A_1", -1, -1);
                        else if (lf < s)
                            MR::startSystemSE("SE_SY_TIMER_A_QUASI_0", -1, -1);
                    }
                }
            }
        }
    }
}

void DesertLandMoveSwitch::exeWait() {
    if (MR::isFirstStep(this)) {
        MR::startBck(this, "Wait", 0);
        _98 = 0;
        mSpringValue->reset();
    }

    tryConnect();
    if (!_99 && _9A) {
        mSpringValue->_10 += -10.0f;
    }

    mSpringValue->update();
    if (trySwitchDown()) {
        mSpringValue->reset();
    }
}

void DesertLandMoveSwitch::exeSwitchDown() {
    if (MR::isFirstStep(this))
        MR::invalidateClipping(this);

    if (MR::isStep(this, 6))
        MR::startBck(this, "On", 0);

    if (tryOn())
        return;
}

void DesertLandMoveSwitch::exeOn() {
    if (MR::isFirstStep(this))
        MR::onSwitchA(this);
    tryConnect();
    if (_9C > 0) {
        updateTimerSE();
        if (MR::isStep(this, _9C))
            setNerve(&NrvDesertLandMoveSwitch::HostTypeReturn::sInstance);
    } else if (!MR::isOnSwitchA(this))
        setNerve(&NrvDesertLandMoveSwitch::HostTypeReturn::sInstance);
}

void DesertLandMoveSwitch::exeReturn() {
    if (MR::isFirstStep(this)) {
        MR::startBck(this, "Return", 0);
        MR::offSwitchA(this);
        _98 = 0;
        mSpringValue->reset();
    }

    if (!_99 && _9A)
        mSpringValue->_10 += -10.0f;

    mSpringValue->update();

    if (trySwitchDown())
        mSpringValue->reset();
    else if (MR::isBckStopped(this)) {
        setNerve(&NrvDesertLandMoveSwitch::HostTypeWait::sInstance);
    }
}

void SpringValue::reset() {
    _4 = _0;
    _10 = 0.0f;
}