#include "pch.h"

bool DrawSuperSonic = false;

void GamePlay_HackDisplay(EntityData1* data, CharObj2* co2) {
    if (ExtendedGamePlay == false || (AlwaysSuperSonic == false && IsEventPerforming() == true)) {
        return;
    }
    
    if (co2->Upgrades & Upgrades_SuperSonic) {
        WriteData<1>((void*)0x494AED, co2->AnimationThing.Index); // Hack for the ball not to flash
        DrawSuperSonic = true;
    }
    else {
        WriteData<1>((void*)0x494AED, 0x91); // Retore ball hack
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