#include "pch.h"
#include "FunctionHook.h"
#include "UsercallFunctionHandler.h"

// Main code for transforming, detransforming, losing rings, etc.

TaskHook Sonic_Exec_t(SonicTheHedgehog);
TaskHook Sonic_Display_t(SonicDisplay);
TaskHook Sonic_Delete_t(SonicDestruct);
UsercallFunc(signed int, SuperSonicNAct_t, (motionwk2* a1, playerwk* a2, taskwk* a3), (a1, a2, a3), 0x494CD0, rEAX, rEAX, rECX, rESI);
UsercallFunc(signed int, SonicNAct_t, (taskwk* a1, playerwk* a2, motionwk2* a3), (a1, a2, a3), 0x496880, rEAX, rECX, rEAX, stack4);

static NJS_TEXNAME SUPERSONIC_EXTRA_TEXNAME[6];
NJS_TEXLIST SUPERSONIC_EXTRA_TEXLIST = { arrayptrandlength(SUPERSONIC_EXTRA_TEXNAME) };

static int RingTimer = 0;

bool IsSuperSonic(CharObj2* co2)
{
	return (co2->Upgrades & Upgrades_SuperSonic);
}

bool IsSuperSonic(int pnum)
{
	return playerpwp[pnum] && IsSuperSonic((CharObj2*)playerpwp[pnum]);
}

bool IsStoryFinished()
{
	return GetEventFlag(EventFlags_SuperSonicAdventureComplete);
}

bool IsPerfectChaosLevel()
{
	return CurrentLevel == LevelIDs_PerfectChaos && CurrentAct == 0;
}

bool UseAdvancedSuperSonic()
{
	return ExtendedGamePlay == true && (AlwaysSuperSonic == true || IsEventPerforming() == false);
}

static void __cdecl njSetTextureHack(NJS_TEXLIST* texlist)
{
	if (SuperSonicFlag)
	{
		njSetTexture(&SUPERSONIC_TEXLIST);
	}
	else
	{
		njSetTexture(texlist);
	}
}

static void __cdecl Sonic_Display_r(task* tsk)
{
	CharObj2* co2 = (CharObj2*)tsk->mwp->work.ptr;

	// Just a flag to change sonic's lighting, no other use
	SuperSonicFlag = IsSuperSonic(co2) == true ? 1 : 0;

	Sonic_Display_t.Original(tsk);
}

static void TransformSuperSonic(EntityData1* data, CharObj2* co2)
{
	ForcePlayerAction(data->CharIndex, NextAction_SuperSonic);
	RingTimer = 0;

	SetSuperAnims(co2);

	// If it's player 1, play sound & update music
	if (data->CharIndex == 0)
	{
		TransformMusicAndSound();
	}
}

static void CheckSuperSonicTransform(EntityData1* data, CharObj2* co2)
{
	if (PressedButtons[data->CharIndex] & TransformButton && IsEventPerforming() == false)
	{
		// If Super Sonic story is finished & more than 50 rings
		if (RemoveLimitations == false && (IsStoryFinished() == false || (data->CharIndex == 0 && Rings < 50))) {
			return;
		}

		TransformSuperSonic(data, co2);
	}
}

static void DetransformSuperSonic(EntityData1* data, CharObj2* co2)
{
	if (AlwaysSuperSonic == false)
	{
		co2->Upgrades &= ~(Upgrades_SuperSonic | Powerups_Invincibility | Status_OnPath);
		
		if (co2->AnimationThing.Index >= 134 && co2->AnimationThing.Index <= 145)
		{
			DoSonicGroundAnimation(co2, data);
		}

		if (data->Status & Status_Ball)
		{
			data->Status &= ~(Status_Attack | Status_Ball);
		}

		co2->Powerups &= ~Powerups_Invincibility;
		
		UnsetSuperAnims(co2);

		// If it's player 1, play sound & reset music
		if (data->CharIndex == 0)
		{
			DetransformMusicAndSound();
		}
	}
}

static void CheckSuperSonicDetransform(EntityData1* data, CharObj2* co2)
{
	if (IsSuperSonic(co2) == true && ((DetransformButton == true && PressedButtons[data->CharIndex] & TransformButton) || IsEventPerforming() == true))
	{
		ForcePlayerAction(data->CharIndex, NextAction_SuperSonicStop);
		DetransformSuperSonic(data, co2);
	}
}

static void SuperSonic_Rings(EntityData1* data, CharObj2* co2)
{
	// Consume rings:
	if (RemoveLimitations == false && data->CharIndex == 0 && TimeThing == 1)
	{
		if (Rings > 0)
		{
			++RingTimer %= 60;
			
			if (RingTimer == 0)
			{
				AddRings(-1);
			}
		}
		else
		{
			Rings = 0; // just in case
			DetransformSuperSonic(data, co2);
		}
	}
}

static void Sonic_NewActions(EntityData1* data, motionwk* mwp, CharObj2* co2)
{
	switch (data->Action)
	{
	case Act_Sonic_Jump:
		if (IsSuperSonic(co2)) {
			CheckSuperSonicDetransform(data, co2);
		}
		else {
			CheckSuperSonicTransform(data, co2);
		}

		break;
	case Act_SuperSonic_Jump:
		CheckSuperSonicDetransform(data, co2);
		break;
	}
}

static void SuperSonic_Actions(EntityData1* data, motionwk* mwp, CharObj2* co2)
{
	// Skip if disabled
	if (UseAdvancedSuperSonic() == false || data->Action == Act_Sonic_Death)
	{
		return;
	}

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
		}
	}
}

static bool Blacklist_NormalSuperSonic(EntityData1* data, CharObj2* co2)
{
	switch (CurrentLevel)
	{
	default:
		break;
	case LevelIDs_Casinopolis:
		return CurrentAct == 2 || CurrentAct == 3;
	case LevelIDs_SpeedHighway:
		return CurrentAct == 1;
	}

	if (co2->AnimationThing.Index < Anm_SuperSonic_Stand || co2->AnimationThing.Index > Anm_SuperSonic_Jump)
	{
		return true;
	}

	return data->Status & Status_DoNextAction &&
		(data->NextAction == 12 || (data->NextAction == 13 && CurrentLevel == LevelIDs_TwinklePark));
}

static void __cdecl Sonic_Exec_r(task* tp)
{
	EntityData1* data = (EntityData1*)tp->twp;
	motionwk* mwp = tp->mwp;
	CharObj2* co2 = (CharObj2*)mwp->work.ptr;

	if (IsPerfectChaosLevel() == false && MetalSonicFlag == false)
	{
		if (data->Action == Act_Sonic_Init)
		{
			InitSuperSonicEyes(data->CharIndex);
			Sonic_Exec_t.Original(tp);
		}
		else
		{
			if (AlwaysSuperSonic == false)
			{
				Sonic_NewActions(data, mwp, co2);
			}

			// Super Sonic actions
			if (IsSuperSonic(co2) == true)
			{
				co2->Powerups |= Powerups_Invincibility;

				RunSuperMusic();

				SuperSonic_Actions(data, mwp, co2); // advanced actions if enabled
				SuperSonic_Rings(data, co2); // deplete rings if enabled
				
				// if advanced super sonic is disabled, detransform Super on invalid actions.
				if (UseAdvancedSuperSonic() == false && Blacklist_NormalSuperSonic(data, co2))
				{
					DetransformSuperSonic(data, co2); 
				}
			}
			else if (AlwaysSuperSonic == true)
			{
				TransformSuperSonic(data, co2);
				LoadPVM("SUPERSONIC", &SUPERSONIC_TEXLIST);
				LoadPVM("SUPERSONIC_EXTRA", &SUPERSONIC_EXTRA_TEXLIST);
			}
		}
	}

	Sonic_Exec_t.Original(tp);
}

static void __cdecl Sonic_Delete_r(task* tsk) {
	EntityData1* data = (EntityData1*)tsk->twp;
	
	// Restore things if the player is deleted, useful for compatiblity with Character Select
	if (data->CharIndex == 0)
	{
		RestoreMusic();
	}

	Sonic_Delete_t.Original(tsk);
}

BOOL  __cdecl SonicNAct_r(taskwk* data, playerwk* co2, motionwk2* data2)
{
	// In case an external mod sets Super Sonic
	if (IsPerfectChaosLevel() == false && data->smode == NextAction_SuperSonic && (data->flag & Status_DoNextAction))
	{
		SetSuperAnims((CharObj2*)co2);
	}

	return SonicNAct_t.Original(data, co2, data2);
}

BOOL __cdecl SuperSonicNAct_r(motionwk2* data2, playerwk* co2, taskwk* data)
{
	// In case an external mod unsets Super Sonic
	if (IsPerfectChaosLevel() == false && data->smode == NextAction_Winning && (data->flag & Status_DoNextAction))
	{
		data->mode = Act_Sonic_ObjectControl;
		co2->mj.reqaction = Anm_SuperSonic_Win;
		NullifyVelocity((EntityData2*)data2, (CharObj2*)co2);
		data->ang.z = 0;
		data->ang.x = 0;
		data->flag &= ~(Status_OnPath | Status_Attack | Status_Ball);
		StopPlayerLookAt(0);
		UnsetSuperAnims((CharObj2*)co2);
		return true;
	}

	return SuperSonicNAct_t.Original(data2, co2, data);
}

void SuperSonic_Init()
{
	HelperFunctionsGlobal.RegisterCharacterPVM(Characters_Sonic, { "SUPERSONIC", &SUPERSONIC_TEXLIST });
	HelperFunctionsGlobal.RegisterCharacterPVM(Characters_Sonic, { "SUPERSONIC_EXTRA", &SUPERSONIC_EXTRA_TEXLIST });

	Sonic_Exec_t.Hook(Sonic_Exec_r);
	Sonic_Display_t.Hook(Sonic_Display_r);
	Sonic_Delete_t.Hook(Sonic_Delete_r);
	SuperSonicNAct_t.Hook(SuperSonicNAct_r); // fix winning position

	if (UseAdvancedSuperSonic())
	{
		SonicNAct_t.Hook(SonicNAct_r);
		WriteCall((void*)0x4949ED, njSetTextureHack);
	}
	
	// Always initialize Super Sonic weld data
	WriteData<2>(reinterpret_cast<Uint8*>(0x0049AC6A), 0x90i8);
}