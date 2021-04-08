#include "pch.h"

// Series of modifications to allow Sonic actions on Super Sonic

bool UseAdvancedSuperSonic() {
    return ExtendedGamePlay == true && (AlwaysSuperSonic == true || IsEventPerforming() == false);
}

void SuperSonic_Display(EntityData1* data, CharObj2* co2) {
    if (!MissedFrames && IsVisible(&data->Position, 15.0f)) {
        Direct3D_SetZFunc(1u);
        BackupConstantAttr();
        AddConstantAttr(0, NJD_FLAG_IGNORE_SPECULAR);
        njControl3D_Backup();
        njControl3D(NJD_CONTROL_3D_CONSTANT_MATERIAL);
        SetMaterialAndSpriteColor_Float(1.0f, 1.0f, 1.0f, 1.0f);
        Direct3D_PerformLighting(4);

        int current_anim = co2->AnimationThing.Index;

        if (co2->AnimationThing.State == 2)
        {
            current_anim = co2->AnimationThing.LastIndex;
        }

        if (!(data->InvulnerableTime & 2)) {
            njSetTexture(&SUPERSONIC_TEXLIST);

            njPushMatrixEx();
            njTranslateEx(&data->CollisionInfo->CollisionArray->center);
            njRotateZ_(data->Rotation.z);
            njRotateX_(data->Rotation.x);
            njRotateY_((-0x8000 - LOWORD(data->Rotation.y)));

            // Spindash deform
            if (current_anim == Anm_Sonic_JumpOrSpin && data->Status & (Status_Unknown1 | Status_Ground)) {
                njTranslateY(-1.0f);
                njRotateZ(nullptr, 0x2000);
                njScale(nullptr, 0.7f, 1.1f, 0.8f);
            }

            njAction_SuperSonic(co2->AnimationThing.AnimData[current_anim].Animation, co2->AnimationThing.Frame);

            njPopMatrixEx();
            Direct3D_PerformLighting(0);
            ClampGlobalColorThing_Thing();
            njControl3D_Restore();
            RestoreConstantAttr();
            Direct3D_ResetZFunc();

            if (IsGamePaused()) {
                DrawCharacterShadow(data, &co2->_struct_a3);
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

void SuperSonic_Actions(EntityData1* data, motionwk* mwp, CharObj2* co2) {
    if (data->Action == Act_Sonic_Death || SonicDetransformNAct(data, co2)) {
        return;
    }

    // Always force Super Sonic if enabled
    if (AlwaysSuperSonic == true && !(co2->Upgrades & Upgrades_SuperSonic)) {
        ForcePlayerAction(data->CharIndex, NextAction_SuperSonic);
        LoadPVM("SUPERSONIC", &SUPERSONIC_TEXLIST);
        return;
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