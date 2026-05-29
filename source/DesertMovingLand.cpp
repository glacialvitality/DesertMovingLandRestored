#include "DesertMovingLand.h"

namespace NrvDesertMovingLand {
    FULL_NERVE(HostTypeWaitTop, DesertMovingLand, Wait);
    FULL_NERVE(HostTypeWaitBottom, DesertMovingLand, Wait);
    FULL_NERVE(HostTypeMoveDown, DesertMovingLand, MoveDown);
    FULL_NERVE(HostTypeMoveUp, DesertMovingLand, MoveUp);
    FULL_NERVE(HostTypeMoveDownSign, DesertMovingLand, MoveSign);
    FULL_NERVE(HostTypeMoveUpSign, DesertMovingLand, MoveSign);
    FULL_NERVE(HostTypeStop, DesertMovingLand, Stop);
};  // namespace NrvDesertMovingLand

namespace {
    static const char* cDemoName = "砂漠ＵＦＯ上昇";
};

DesertMovingLand::DesertMovingLand(const char* pName) : MapObjActor(pName), _C4(0.0f, 0.0f, 0.0f), _D0(0.0f, 0.0f, 0.0f) {
    _DC = 720;
    _E0 = 720;
    _E4.identity();
}
void DesertMovingLand::init(const JMapInfoIter& rIter) {
    MapObjActor::init(rIter);
    MapObjActorInitInfo info = MapObjActorInitInfo();
    MapObjActorUtil::setupInitInfoSimpleMapObj(&info);
    info.setupHitSensor();
    info.setupNerve(&NrvDesertMovingLand::HostTypeWaitTop::sInstance);
    info.setupBaseMtxFollowTarget();
    info.setupFarClipping(-1.0f);
    //info.mNoAppearRiddleSE = false;
    initialize(rIter, info);

    if (!MR::isValidSwitchA(this))
        setNerve(&NrvDesertMovingLand::HostTypeWaitBottom::sInstance);

    MtxPtr jointMtx = MR::getJointMtx(this, mObjName);
    HitSensor* pSensorMove = getSensor(0);
    MR::initCollisionParts(this, "Move", pSensorMove, jointMtx);

    if (MR::isDead(this))
        MR::invalidateCollisionParts(this);
    else
        MR::validateCollisionParts(this);

    f32 arg = 2000.0f;
    MR::getJMapInfoArg0NoInit(rIter, &arg);
    MR::getJMapInfoArg1NoInit(rIter, &_DC);
    MR::getJMapInfoArg2NoInit(rIter, &_E0);
    TVec3f stack_3C;
    MR::calcUpVec(&stack_3C, this);


    if (MR::isValidSwitchA(this)) {
        _C4.set(mTranslation);
        TVec3f scaledOffset = stack_3C;
        scaledOffset.scale(arg);
        _D0.set(_C4 - scaledOffset);
        //_D0.set(_C4 - stack_3C.scaleInline(arg));
    } else {
        _D0.set(mTranslation);
        TVec3f scaledOffset = stack_3C;
        scaledOffset.scale(arg);
        _C4.set(_D0 + scaledOffset);
        //_C4.set(_D0 + stack_3C.scaleInline(arg));
    }
    MR::setBodySensorType(this, 0x76); //MR::setBodySensorTypePress(this);
}

void DesertMovingLand::control() {
    MapObjActor::control();
    if (MR::isDemoActive(cDemoName))
        updateDemoPlayerPos();

    MR::startLevelSound(this, "SE_AT_LV_WIND_MOVING_DESERT", -1, -1, -1);
    if (!isNerve(&NrvDesertMovingLand::HostTypeStop::sInstance) && isPlayerOnPress())
        setNerve(&NrvDesertMovingLand::HostTypeStop::sInstance);
}

void DesertMovingLand::startDemo() {
    MtxPtr playerBaseMtx = MR::getPlayerDemoActor()->getBaseMtx();
    TMtx34f stack_38;
    stack_38.set(playerBaseMtx);
    MtxPtr jointMtx = MR::getJointMtx(this, mObjName);
    TMtx34f stack_8;
    stack_8.set(jointMtx);
    stack_8.invert(stack_8);
    _E4.concat(stack_8, stack_38);
}

void DesertMovingLand::updateDemoPlayerPos() {
    MtxPtr jointMtx = MR::getJointMtx(this, mObjName);
    TPos3f stack_8;
    stack_8.set(jointMtx);
    stack_8.concat(stack_8, _E4);
    stack_8.getTransInline(MR::getPlayerDemoActor()->mTranslation);
    MR::setPlayerBaseMtx(stack_8);
}

void DesertMovingLand::exeWait() {
    if (MR::isValidSwitchA(this) && MR::isOnSwitchA(this) && isNerve(&NrvDesertMovingLand::HostTypeWaitTop::sInstance)) {
        startDemo();
        setNerve(&NrvDesertMovingLand::HostTypeMoveDownSign::sInstance);
    } else if (MR::isOnSwitchB(this) && isNerve(&NrvDesertMovingLand::HostTypeWaitBottom::sInstance)) {
        startDemo();
        setNerve(&NrvDesertMovingLand::HostTypeMoveUpSign::sInstance);
    }
}

void DesertMovingLand::exeMoveDown() {
    if (MR::isFirstStep(this)) {
        TVec3f stack_8(_D0 - _C4);
        stack_8.setLength(PSVECMag(stack_8) / _DC);
        mVelocity.set(stack_8);
        MR::startAtmosphereSE("SE_OJ_DESERT_LAND_MOVE_ST", -1, -1);
    }
    MR::startAtmosphereLevelSE("SE_OJ_LV_DESERT_LAND_MOVE", -1, -1);
    if (MR::isLessStep(this, 0x5A)) {
        MR::tryRumblePadWeak(this, 0);
    }

    if (MR::isStep(this, 0x5A)) {
        MR::stopShakingCamera(this);
    }

    if (MR::isStep(this, _DC)) {
        MR::offSwitchB(this);
        mVelocity.zero();
        mTranslation.set(_D0);
        MR::startAtmosphereSE("SE_OJ_DESERT_LAND_MOVE_ED", -1, -1);
        setNerve(&NrvDesertMovingLand::HostTypeWaitBottom::sInstance);
    }
}

void DesertMovingLand::exeMoveUp() {
    if (MR::isFirstStep(this)) {
        TVec3f stack_8(_C4 - _D0);
        stack_8.setLength(PSVECMag(stack_8) / _E0);
        mVelocity.set(stack_8);
        MR::startAtmosphereSE("SE_OJ_DESERT_LAND_MOVE_ST", -1, -1);
    }
    MR::startAtmosphereLevelSE("SE_OJ_LV_DESERT_LAND_MOVE", -1, -1);

    if (MR::isLessStep(this, 0x5A)) {
        MR::tryRumblePadWeak(this, 0);
    }

    if (MR::isStep(this, 0x5A)) {
        MR::stopShakingCamera(this);
    }

    if (MR::isStep(this, _E0)) {
        if (MR::isValidSwitchA(this))
            MR::offSwitchA(this);

        mVelocity.zero();
        mTranslation.set(_C4);
        MR::startAtmosphereSE("SE_OJ_DESERT_LAND_MOVE_ED", -1, -1);
        setNerve(&NrvDesertMovingLand::HostTypeWaitTop::sInstance);
    }
}

void DesertMovingLand::exeMoveSign() {
    if (MR::isFirstStep(this)) {
        MR::startBck(this, "Vibration", 0);

        if (MR::isDemoActive("砂ＵＦＯ下降"))
            MR::stopStageBGM(0x3C);

        MR::shakeCameraInfinity(this, 0.15f, 1.2f);
    }

    MR::startLevelSound(this, "SE_OJ_LV_DESERT_LAND_SIGN", -1, -1, -1);
    MR::tryRumblePadMiddle(this, 0);
    if (MR::isBckStopped(this)) {
        if (isNerve(&NrvDesertMovingLand::HostTypeMoveUpSign::sInstance))
            setNerve(&NrvDesertMovingLand::HostTypeMoveUp::sInstance);
        else {
            if (isNerve(&NrvDesertMovingLand::HostTypeMoveDownSign::sInstance)) {
                if (MR::isDemoActive("砂ＵＦＯ下降"))
                    //MR::startEventBGM(0); //what the fuck is this?

                setNerve(&NrvDesertMovingLand::HostTypeMoveDown::sInstance);
            }
        }
    }
}

void DesertMovingLand::exeStop() {
    if (MR::isFirstStep(this))
        MR::invalidateHitSensors(this);

    if (MR::isStep(this, 0x1E))
        mVelocity.zero();
}

bool DesertMovingLand::isPlayerOnPress() {
        MarioActor* pMarioActor = MR::getMarioHolder()->getMarioActor();
        return pMarioActor->_6BC != 0;
    }

void DesertMovingLand::connectToScene(const MapObjActorInitInfo& rInfo) {
    MR::connectToSceneCollisionMapObj(this);
}

void DesertMovingLand::initCaseNoUseSwitchB(const MapObjActorInitInfo& rInfo) {}

void DesertMovingLand::initCaseUseSwitchB(const MapObjActorInitInfo& rInfo) {}

void DesertMovingLand::initCaseUseSwitchA(const MapObjActorInitInfo& rInfo) {}

DesertMovingLand::~DesertMovingLand(){};