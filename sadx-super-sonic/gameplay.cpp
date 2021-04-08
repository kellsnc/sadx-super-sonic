#include "pch.h"

// Series of hacky hacks coming straight from hell to allow Sonic actions as Super Sonic

Trampoline* njAction_t = nullptr;
Trampoline* njAction_Queue_t = nullptr;

NJS_ACTION new_action;
AnimationFile* customAnims[4] = {};
std::vector<std::pair<AnimData, int>> Backups;

bool DrawSuperSonic = false;

#define REPLACEANIM(a, b) Backups.push_back({SonicAnimData[a], a}); SonicAnimData[a] = SonicAnimData[b];
#define REPLACEMOTION(a, b) Backups.push_back({SonicAnimData[a], a}); SonicAnimData[a].Animation->motion = customAnims[b]->getmotion();

NJS_OBJECT* SonicObjectToSuperSonic(NJS_OBJECT* object) {
    if (object == SONIC_OBJECTS[0]) {
        return SONIC_OBJECTS[22];
    } 
    else if (object == SONIC_OBJECTS[56] || object == SONIC_OBJECTS[70] || object == SONIC_OBJECTS[66]) {
        return SONIC_ACTIONS[142]->object;
    }
    else {
        return object;
    }
}

NJS_ACTION* njAction_Hack(NJS_ACTION* action) {
    if (DrawSuperSonic == true) {
        new_action = { SonicObjectToSuperSonic(action->object), action->motion };
        DrawSuperSonic = false;
        return &new_action;
    }
    else {
        return action;
    }
}

void __cdecl njAction_r(NJS_ACTION* action, Float frame) {
    TARGET_DYNAMIC(njAction)(njAction_Hack(action), frame);
}

void __cdecl njAction_Queue_r(NJS_ACTION* action, float frame, QueuedModelFlagsB flags) {
    TARGET_DYNAMIC(njAction_Queue)(njAction_Hack(action), frame, flags);
}

void GamePlay_HackDisplay(EntityData1* data, CharObj2* co2) {
    if (ExtendedGamePlay == false || (AlwaysSuperSonic == false && IsEventPerforming() == true)) {
        return;
    }
    
    if (co2->Upgrades & Upgrades_SuperSonic) {
        WriteData((NJS_TEXLIST**)0x4949E9, &SUPERSONIC_TEXLIST); // Force Super Sonic's texlist
        WriteData<1>((void*)0x494AED, co2->AnimationThing.Index); // Hack for the ball not to flash
        DrawSuperSonic = true;

        SonicAnimData[Anm_Sonic_Stand].AnimationSpeed = 0.25f;
        SonicAnimData[Anm_Sonic_Walk1].AnimationSpeed = 0.5f;
        SonicAnimData[Anm_Sonic_Walk2].AnimationSpeed = 0.5f;
        SonicAnimData[Anm_Sonic_Walk3].AnimationSpeed = 0.5f;
    }
    else {
        WriteData((NJS_TEXLIST**)0x4949E9, &SONIC_TEXLIST);
        WriteData<1>((void*)0x494AED, 0x91);
    }
}

void GamePlay_UnsetSuperAnims() {
    if (ExtendedGamePlay == true) {
        for (auto& i : Backups) {
            SonicAnimData[i.second] = i.first;
        }

        Backups.clear();
    }
}

void GamePlay_SetSuperAnims() {
    if (ExtendedGamePlay == true) {
        if (Backups.empty() == true) {
            REPLACEANIM(Anm_Sonic_Stand, Anm_SuperSonic_Stand);
            REPLACEANIM(Anm_Sonic_Stance, Anm_SuperSonic_Stand);
            REPLACEANIM(Anm_Sonic_StandToStance, Anm_SuperSonic_Stand);
            REPLACEANIM(Anm_Sonic_StandUpwardSlope, Anm_SuperSonic_Stand);
            REPLACEANIM(Anm_Sonic_StandDownwardSlope, Anm_SuperSonic_Stand);
            REPLACEANIM(Anm_Sonic_Walk1, Anm_SuperSonic_StandToMove);
            REPLACEANIM(Anm_Sonic_Walk2, Anm_SuperSonic_Move1);
            REPLACEANIM(Anm_Sonic_Walk3, Anm_SuperSonic_Move2);
            REPLACEANIM(Anm_Sonic_Run1, Anm_SuperSonic_Move2);
            REPLACEANIM(Anm_Sonic_Run2, Anm_SuperSonic_Move3);
            REPLACEANIM(Anm_Sonic_Win, Anm_SuperSonic_Win);
    
            if (CustomAnims == true) {
                REPLACEMOTION(Anm_Sonic_Land, 0);
                REPLACEMOTION(Anm_Sonic_MonkeyBarsMove, 1);
                REPLACEMOTION(Anm_Sonic_MonkeyBarsHoldLeft, 2);
                REPLACEMOTION(Anm_Sonic_MonkeyBarsHoldRight, 3);
            }
        }
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
    if (data->Action == Act_Sonic_Death || (ExtendedGamePlay == false && IsEventPerforming() == false) || SonicDetransformNAct(data, co2)) {
        return; // Live above + check detransformation
    }

    if (data->Action != Act_Sonic_Stand) {
        if (data->Status & Status_HoldObject) {
            Sonic_HoldingObject_NAct(data, co2, (EntityData2*)mwp);
        }
        else {
            Sonic_NAct(co2, data, (EntityData2*)mwp);
        }
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

void GamePlay_Init() {
    if (ExtendedGamePlay == true) {
        njAction_t = new Trampoline((int)njAction, (int)njAction + 0x9, njAction_r);
        njAction_Queue_t = new Trampoline((int)njAction_Queue, (int)njAction_Queue + 0x8, njAction_Queue_r);

        if (CustomAnims == true) {
            LoadAnimationFile(&customAnims[0], "SS_011");
            LoadAnimationFile(&customAnims[1], "SS_051");
            LoadAnimationFile(&customAnims[2], "SS_053");
            LoadAnimationFile(&customAnims[3], "SS_052");
        }
    }
}