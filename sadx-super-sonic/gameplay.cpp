#include "pch.h"
#include "Characters/Common.h"
#include "Characters/Sonic.h"

FunctionPointer(void, ResetEventWorkThing, (unsigned __int8 id), 0x441080);
FunctionPointer(void, SomethingJumpPanel, (EntityData1* data), 0x4B8470);

//signed int __usercall Sonic_HoldingObject_NAct@<eax>(EntityData1 *data@<ecx>, int a2@<edx>, CharObj2 *co2@<eax>, void *data2)
static const void* const Sonic_HoldingObject_NActPtr = (void*)0x496880;
static inline signed int Sonic_HoldingObject_NAct(EntityData1* data, CharObj2* co2, EntityData2* data2)
{
    signed int result;
    __asm
    {
        push[data2]
        mov eax, [co2]
        mov ecx, [data]
        call Sonic_HoldingObject_NActPtr
        add esp, 4
        mov result, eax
    }
    return result;
}

signed int __cdecl SuperSonic_ImprovedNextAct(EntityData2* data2, CharObj2* co2, EntityData1* data) {
    if (data->Status & Status_DoNextAction) {
        switch (data->NextAction) {
        case NextAction_WindPath:
            data->Action = Act_SuperSonic_Path;
            co2->AnimationThing.Index = Anm_SuperSonic_Move2;
            co2->LoopDist = 0.0;

            if (data->Status & Status_Ball) {
                data->Status &= ~(Status_Attack | Status_Ball);
            }

            data->Status |= Status_OnPath;

            return true;
        case NextAction_VerticalSpring:
            data->Action = Act_SuperSonic_Spring;
            co2->AnimationThing.Index = Anm_SuperSonic_Spring;
            data->Status &= ~(Status_OnPath | Status_Attack | Status_Ball);

            return true;
        case NextAction_Spring:
            data->Action = Act_SuperSonic_Launched;
            co2->AnimationThing.Index = Anm_SuperSonic_Spring;

            if (data->Status & Status_Ball) {
                data->Status &= ~(Status_Attack | Status_Ball);
            }

            data->Status &= ~Status_OnPath;

            return true;
        case NextAction_CtrlStand:
            data->Action = Act_Sonic_ObjectControl;
            co2->AnimationThing.Index = Anm_SuperSonic_Stand;
            data->Rotation.z = 0;
            data->Rotation.x = 0;
            NullifyVelocity(data2, co2);

            if (data->Status & Status_Ball) {
                data->Status &= ~(Status_Attack | Status_Ball);
            }

            data->Status &= ~Status_OnPath;

            return true;
        case NextAction_CtrlSpin:
            data->Action = Act_Sonic_ObjectControl;
            co2->AnimationThing.Index = Anm_SuperSonic_Jump;
            data->Rotation.z = 0;
            data->Rotation.x = 0;
            NullifyVelocity(data2, co2);
            data->Status = data->Status & ~Status_OnPath | Status_Attack | Status_Ball;

            return true;
        case NextAction_CtrlRun:
            data->Action = Act_Sonic_ObjectControl;
            co2->AnimationThing.Index = Anm_SuperSonic_Move3;
            NullifyVelocity(data2, co2);

            if (data->Status & Status_Ball) {
                data->Status &= ~(Status_Attack | Status_Ball);
            }

            data->Status &= ~Status_OnPath;

            return true;
        case NextAction_Winning:
            data->Action = Act_Sonic_ObjectControl;
            co2->AnimationThing.Index = Anm_SuperSonic_Win;
            data->Rotation.z = 0;
            data->Rotation.x = 0;
            data->Position.y = 10.0;
            NullifyVelocity(data2, co2);
            data->Status &= ~(Status_OnPath | Status_Attack | Status_Ball);
            ResetEventWorkThing(data->CharIndex);

            return true;
        case NextAction_Reset:
            data->Action = Act_SuperSonic_Stand;
            co2->AnimationThing.Index = Anm_SuperSonic_Stand;

            if (data->Status & Status_Ball)
            {
                data->Status &= ~(Status_Attack | Status_Ball);
            }

            data->Status &= ~(Status_OnPath | Status_LightDash);

            return true;
        case NextAction_Loop:
            if (data->Action == Act_SuperSonic_Walk || co2->Speed.x > (double)co2->PhysicsData.RollEnd)
            {
                data->Status |= Status_OnPath;
            }

            return false;
        case NextAction_SuperSonicStop:
            co2->Upgrades &= ~Upgrades_SuperSonic;
            DoSonicGroundAnimation(co2, data);

            if (data->Status & Status_Ball) {
                data->Status &= ~(Status_Attack | Status_Ball);
            }

            data->Status &= ~Status_OnPath;

            return true;
        case NextAction_Death:
            co2->Upgrades &= ~Upgrades_SuperSonic;
            data->Action = Act_Sonic_Death;
            co2->AnimationThing.Index = Anm_Sonic_DeathAir;
            data->Status &= Status_Unknown6 | Status_DisableControl | Status_DoNextAction | Status_HoldObject | Status_LightDash;

            if (MetalSonicFlag)
            {
                PlayVoice(2046);
            }
            else
            {
                PlaySound(1502, 0, 0, 0);
            }

            return true;
        }
    }

    if (data->Status & Status_HoldObject) {
        return Sonic_HoldingObject_NAct(data, co2, data2);
    }
    
    return Sonic_NAct(co2, data, data2);
}

static void __declspec(naked) _SuperSonic_NAct() {
    __asm {
        push esi // data
        push ecx // co2
        push eax // data2

        // Call your __cdecl function here:
        call SuperSonic_ImprovedNextAct

        add esp, 4 // data2<eax> is also used for return value
        pop ecx // co2
        pop esi // data
        retn
    }
}

int GetModelType(int animation) {
    switch (animation) {
    case Anm_SuperSonic_Stand:
    case Anm_SuperSonic_StandToMove:
    case Anm_SuperSonic_Move1:
    case Anm_SuperSonic_Move2:
    case Anm_SuperSonic_Move3:
    case Anm_SuperSonic_Spring:
    case Anm_SuperSonic_SpringFall:
    case Anm_SuperSonic_Fall:
    case Anm_SuperSonic_Land:
    case Anm_SuperSonic_Win:
    case Anm_SuperSonic_WinToStand:
    case Anm_SuperSonic_Jump:
        return -1;
    case Anm_Sonic_Jump:
    case Anm_Sonic_JumpOrSpin:
    case Anm_Sonic_Bowling:
    case Anm_Sonic_Roll:
        return 1;
    default:
        return 0;
    }
}

void SetSuperSonicModel(AnimData* anim, int animation) {
    switch (GetModelType(animation)) {
    case 0:
        anim[animation].Animation->object = SONIC_OBJECTS[22];
        break;
    case 1:
        anim[animation].Animation->object = anim[Anm_SuperSonic_Jump].Animation->object;
        break;
    }
}

void SetSonicModel(AnimData* anim, int animation) {
    switch (GetModelType(animation)) {
    case 0:
        anim[animation].Animation->object = SONIC_OBJECTS[0];
        break;
    case 1:
        break;
    }
}

void GamePlay_HackDisplay(EntityData1* data, CharObj2* co2) {
    if (CurrentLevel == LevelIDs_PerfectChaos) {
        return;
    }

    if (co2->Upgrades & Upgrades_SuperSonic) {
        WriteData((NJS_TEXLIST**)0x4949E9, &SUPERSONIC_TEXLIST); // Force Super Sonic's texlist
        WriteData<1>((void*)0x494AED, co2->AnimationThing.Index); // Hack for the ball not to flash
        SetSuperSonicModel(co2->AnimationThing.AnimData, co2->AnimationThing.Index);
    }
    else {
        WriteData((NJS_TEXLIST**)0x4949E9, &SONIC_TEXLIST);
        WriteData<1>((void*)0x494AED, 0x91);
        SetSonicModel(co2->AnimationThing.AnimData, co2->AnimationThing.Index);
    }
}

void GamePlay_HackActions(EntityData1* data, motionwk* mwp, CharObj2* co2) {
    if (CurrentLevel == LevelIDs_PerfectChaos) {
        return;
    }

    if (co2 && co2->Upgrades & Upgrades_SuperSonic) {
        switch (data->Action) {
        case Act_Sonic_Stand:
            data->Action = Act_SuperSonic_Stand;
        case Act_Sonic_Walk:
            data->Action = Act_SuperSonic_Walk;
        case Act_SuperSonic_Stand:
            if (JumpAllowed(data) == 2) {
                data->Status &= ~(Status_HoldObject | Status_Unknown1);
                SomethingJumpPanel(data);
                data->Action = Act_Sonic_JumpPanel;
                Sonic_JumpPadAni(co2);
                PlaySound(17, 0, 0, 0);
                co2->JumpTime = 0;
                return;
            }

            Sonic_CheckLightDash(co2, data);
            Sonic_CheckPickUpObject(co2, data);
            Sonic_ChargeSpindash(co2, data);
            break;
        case Act_SuperSonic_Walk:
            Sonic_CheckLightDash(co2, data);
            Sonic_CheckPickUpObject(co2, data);
            Sonic_ChargeSpindash(co2, data);
            break;
        }
    }
}

void GamePlay_Init() {
	WriteJump((void*)0x494CD0, _SuperSonic_NAct);
}