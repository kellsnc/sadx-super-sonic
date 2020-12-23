#include "pch.h"

static bool ExtendedGamePlay = true;

static AnimData_t SuperSonicAnimData[SonicAnimData_Length];

signed int __cdecl SuperSonic_NAct_r(EntityData2* data2, CharObj2* co2, EntityData1* data) {
    if (data->Status & Status_DoNextAction) {
        co2->Upgrades &= ~Upgrades_SuperSonic;
        DoSonicGroundAnimation(co2, data);

        if (data->Status & Status_Ball) {
            data->Status &= ~(Status_Attack | Status_Ball);
        }

        data->Status &= ~Status_OnPath;

        return true;
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
        call SuperSonic_NAct_r
        add esp, 4 // data2<eax> is also used for return value
        pop ecx // co2
        pop esi // data
        retn
    }
}

void GamePlay_HackDisplay(EntityData1* data, CharObj2* co2) {
    if (ExtendedGamePlay == false || CurrentLevel == LevelIDs_PerfectChaos) {
        return;
    }

    if (co2->Upgrades & Upgrades_SuperSonic) {
        WriteData((NJS_TEXLIST**)0x4949E9, &SUPERSONIC_TEXLIST); // Force Super Sonic's texlist
        WriteData<1>((void*)0x494AED, co2->AnimationThing.Index); // Hack for the ball not to flash
        co2->AnimationThing.AnimData = SuperSonicAnimData;
    }
    else {
        WriteData((NJS_TEXLIST**)0x4949E9, &SONIC_TEXLIST);
        WriteData<1>((void*)0x494AED, 0x91);
        co2->AnimationThing.AnimData = SonicAnimData;
    }
}

void GamePlay_HackActions(EntityData1* data, motionwk* mwp, CharObj2* co2) {
    if (ExtendedGamePlay == false || CurrentLevel == LevelIDs_PerfectChaos) {
        return;
    }

    if (co2->Upgrades & Upgrades_SuperSonic) {
        switch (data->Action) {
        case Act_SuperSonic_Stand:
            data->Action = Act_Sonic_Stand;
            break;
        case Act_SuperSonic_Walk:
            data->Action = Act_Sonic_Walk;
            break;
        case Act_SuperSonic_Launched:
            data->Action = Act_Sonic_Launch;
            break;
        case Act_SuperSonic_Spring:
            data->Action = Act_Sonic_Spring;
            break;
        case Act_Sonic_Jump:
            data->Action = Act_SuperSonic_Jump;
            break;
        case Act_Sonic_HomingAttack:
            data->Action = Act_SuperSonic_Homing;
            break;
        case Act_Sonic_Path:
            data->Action = Act_SuperSonic_Path;
            break;
        }
    }
}

NJS_OBJECT* GetSuperSonicModel(unsigned int animation) {
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
        return SonicAnimData[animation].Animation->object;
    case Anm_Sonic_Jump:
    case Anm_Sonic_JumpOrSpin:
    case Anm_Sonic_Bowling:
    case Anm_Sonic_Roll:
        return SONIC_ACTIONS[142]->object;
    default:
        return SONIC_OBJECTS[22];
    }
}

void SuperSonic_InitAnimTable() {
    if (ExtendedGamePlay == true) {
        memcpy(&SuperSonicAnimData, &SonicAnimData, sizeof(AnimData_t) * SonicAnimData_Length);

        for (int i = 0; i < SonicAnimData_Length; ++i) {
            SuperSonicAnimData[i] = SonicAnimData[i];
            SuperSonicAnimData[i].Animation = new NJS_ACTION();

            if (SonicAnimData[i].Animation) {
                SuperSonicAnimData[i].Animation->motion = SonicAnimData[i].Animation->motion;
                SuperSonicAnimData[i].Animation->object = GetSuperSonicModel(i);
            }
        }
    }

    SuperSonicAnimData[Anm_Sonic_StandToStance] = SonicAnimData[Anm_SuperSonic_Stand];
    SuperSonicAnimData[Anm_Sonic_Stance] = SonicAnimData[Anm_SuperSonic_Stand];
    SuperSonicAnimData[Anm_Sonic_Stand] = SonicAnimData[Anm_SuperSonic_Stand];
    SuperSonicAnimData[Anm_Sonic_StandUpwardSlope] = SonicAnimData[Anm_SuperSonic_Stand];
    SuperSonicAnimData[Anm_Sonic_StandDownwardSlope] = SonicAnimData[Anm_SuperSonic_Stand];
    SuperSonicAnimData[Anm_Sonic_Walk1] = SonicAnimData[Anm_SuperSonic_StandToMove];
    SuperSonicAnimData[Anm_Sonic_Walk2] = SonicAnimData[Anm_SuperSonic_Move1];
    SuperSonicAnimData[Anm_Sonic_Walk3] = SonicAnimData[Anm_SuperSonic_Move2];
    SuperSonicAnimData[Anm_Sonic_Run1] = SonicAnimData[Anm_SuperSonic_Move2];
    SuperSonicAnimData[Anm_Sonic_Run2] = SonicAnimData[Anm_SuperSonic_Move3];
}

void GamePlay_Init(const IniFile* config) {
    ExtendedGamePlay = config->getBool("General", "ExtendedGameplay", true);

    if (ExtendedGamePlay == true) {
        WriteJump((void*)0x494CD0, _SuperSonic_NAct);
    }
}