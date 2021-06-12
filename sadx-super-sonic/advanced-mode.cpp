#include "pch.h"

// Series of modifications to allow Sonic actions on Super Sonic

bool UseAdvancedSuperSonic()
{
    return ExtendedGamePlay == true && (AlwaysSuperSonic == true || IsEventPerforming() == false);
}

void SuperSonic_HackDisplay(CharObj2* co2)
{
    if (UseAdvancedSuperSonic() == true)
    {
        if (SuperSonicFlag == 1)
        {
            WriteData((NJS_TEXLIST**)0x4949E9, &SUPERSONIC_TEXLIST); // Force Super Sonic's texlist
            WriteData<1>((void*)0x494AED, co2->AnimationThing.Index); // Hack for the ball not to flash
        }
        else
        {
            WriteData((NJS_TEXLIST**)0x4949E9, &SONIC_TEXLIST);
            WriteData<1>((void*)0x494AED, 0x91);
        }
    }
}

// Action handler for Advanced Super Sonic
void SuperSonic_Actions(EntityData1* data, motionwk* mwp, CharObj2* co2)
{
    // Skip if disabled
    if (UseAdvancedSuperSonic() == false || data->Action == Act_Sonic_Death)
    {
        return;
    }

    SetSuperAnims(co2);

    // Run next actions again, fixes weird things
    if (data->Action != Act_Sonic_Stand)
    {
        if (data->Status & Status_HoldObject)
        {
            Sonic_HoldingObject_NAct(data, co2, (EntityData2*)mwp);
        }
        else
        {
            Sonic_NAct(co2, data, (EntityData2*)mwp);
        }
    }

    // Use Super Sonic actions when we can, force Sonic's when it's better
    if (IsSuperSonic(co2) == true)
    {
        switch (data->Action)
        {
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