#include "pch.h"

static bool ExtendedGamePlay = true;
static bool AlwaysSuperSonic = false;
static bool CustomAnims = true;

static AnimData_t SuperSonicAnimData[SonicAnimData_Length];

void GamePlay_HackDisplay(EntityData1* data, CharObj2* co2) {
    if (ExtendedGamePlay == false && IsEventPerforming() == false) {
        return; // Only run this function if extended gameplay is enabled, or it's an event (with "Always Super Sonic")
    }

    if (co2->Upgrades & Upgrades_SuperSonic) {
        WriteData((NJS_TEXLIST**)0x4949E9, &SUPERSONIC_TEXLIST); // Force Super Sonic's texlist
        WriteData<1>((void*)0x494AED, co2->AnimationThing.Index); // Hack for the ball not to flash
        co2->AnimationThing.AnimData = SuperSonicAnimData; // Change the animation set
    }
    else {
        WriteData((NJS_TEXLIST**)0x4949E9, &SONIC_TEXLIST);
        WriteData<1>((void*)0x494AED, 0x91);
        co2->AnimationThing.AnimData = SonicAnimData;
    }
}

bool SonicDetransformNAct(EntityData1* data, CharObj2* co2) {
    if (data->Status & Status_DoNextAction) {
        if (data->NextAction == NextAction_SuperSonicStop) {
            co2->Upgrades &= ~(Upgrades_SuperSonic | Powerups_Invincibility | Status_OnPath);
            DoSonicGroundAnimation(co2, data);

            if (data->Status & Status_Ball) {
                data->Status &= ~(Status_Attack | Status_Ball);
            }

            return true;
        }
    }

    return false;
}

void GamePlay_HackActions(EntityData1* data, motionwk* mwp, CharObj2* co2) {
    if ((ExtendedGamePlay == false && IsEventPerforming() == false) || SonicDetransformNAct(data, co2)) {
        return; // Live above + check detransformation
    }

    if (data->Status & Status_HoldObject) {
        Sonic_HoldingObject_NAct(data, co2, (EntityData2*)mwp);
    }
    else {
        Sonic_NAct(co2, data, (EntityData2*)mwp);
    }

    // Use Super Sonic actions when we can, force Sonic's when it's better
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
        case Act_SuperSonic_Homing:
            data->Action = Act_Sonic_HomingAttack;
            break;
        case Act_Sonic_Path:
            data->Action = Act_SuperSonic_Path;
            break;
        case Act_SuperSonic_Jump:
            data->Action = Act_Sonic_Jump;
            break;
        case Act_Sonic_JumpPanel: // sadx is bad
            data->Action = Act_Sonic_Fall;
            NullifyVelocity((EntityData2*)mwp, co2);
            PrintDebug("[SuperSonic] Invalid action \"Act_Sonic_JumpPanel\" cancelled.\n");

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

void CustomSuperSonicAnim(int id, const char* name) {
    AnimationFile* file = nullptr;
    LoadAnimationFile(&file, name);

    if (file) {
        SuperSonicAnimData[id].Animation->motion = file->getmotion();
    }
}

// Initialize the custom animation table for Super Sonic
void SuperSonic_InitAnimTable() {
    if (ExtendedGamePlay == true || AlwaysSuperSonic == true) {
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

    CustomSuperSonicAnim(19, "SS_011");
    CustomSuperSonicAnim(66, "SS_051");
    CustomSuperSonicAnim(67, "SS_053");
    CustomSuperSonicAnim(68, "SS_052");

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
    SuperSonicAnimData[Anm_Sonic_Win] = SonicAnimData[Anm_SuperSonic_Win];

    if (AlwaysSuperSonic == true) {
        CharSelDataList[0].anonymous_1[0] = SonicAnimData[Anm_SuperSonic_Stand].Animation;
        CharSelDataList[0].anonymous_1[1]->object = SONIC_OBJECTS[22];
        CharSelDataList[0].anonymous_1[2]->object = SONIC_OBJECTS[22];
        CharSelDataList[0].TextureList = &SUPERSONIC_TEXLIST;

        //todo cutscene swaps
    }
}

void GamePlay_Init(const IniFile* config) {
    ExtendedGamePlay = config->getBool("General", "ExtendedGameplay", true);
    AlwaysSuperSonic = config->getBool("General", "AlwaysSuperSonic", false);
    CustomAnims = config->getBool("General", "CustomAnims", true);
}