#include "pch.h"
#include "SADXModLoader.h"
#include "FunctionHook.h"
#include "UsercallFunctionHandler.h"
#include "multi.h"
#include "config.h"

// Main code for transforming, detransforming, losing rings, etc.

TaskHook Sonic_Exec_t(SonicTheHedgehog);
TaskHook Sonic_Display_t(SonicDisplay);
TaskHook Sonic_Delete_t(SonicDestruct);
UsercallFunc(signed int, SuperSonicNAct_t, (motionwk2* a1, playerwk* a2, taskwk* a3), (a1, a2, a3), 0x494CD0, rEAX, rEAX, rECX, rESI);
UsercallFunc(signed int, SonicNAct_t, (taskwk* a1, playerwk* a2, motionwk2* a3), (a1, a2, a3), 0x496880, rEAX, rECX, rEAX, stack4);

static NJS_TEXNAME SUPERSONIC_EXTRA_TEXNAME[6];
NJS_TEXLIST SUPERSONIC_EXTRA_TEXLIST = { arrayptrandlength(SUPERSONIC_EXTRA_TEXNAME) };

static int RingTimer[8] = {};

bool IsSuperSonic(playerwk* pwp)
{
	return (pwp->equipment & Upgrades_SuperSonic);
}

bool IsSuperSonic(int pnum)
{
	return playerpwp[pnum] && IsSuperSonic(playerpwp[pnum]);
}

bool IsStoryFinished()
{
	return SeqCheckFlag(FLAG_SUPERSONIC_COMPLETE) != 0;
}

bool IsPerfectChaosLevel()
{
	return ssStageNumber == LevelIDs_PerfectChaos && ssActNumber == 0;
}

bool UseAdvancedSuperSonic()
{
	return ExtendedGamePlay == true && (AlwaysSuperSonic == true || IsEventPerforming() == false);
}

static void __cdecl njSetTextureHack(NJS_TEXLIST* texlist)
{
	if (flagSuperSonicMode)
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
	auto pwp = (playerwk*)tsk->mwp->work.ptr;

	auto previous_flag = flagSuperSonicMode;
	flagSuperSonicMode = IsSuperSonic(pwp) ? 1 : 0;
	Sonic_Display_t.Original(tsk);
	flagSuperSonicMode = previous_flag;
}

static void TransformSuperSonic(taskwk* twp, playerwk* pwp)
{
	auto pnum = TWP_PNUM(twp);
	SetInputP(pnum, PL_OP_SUPER);
	RingTimer[pnum] = 0;

	SetSuperAnims(pwp);

	// If it's player 1, play sound & update music
	if (pnum == 0)
	{
		TransformMusicAndSound();
		UpdateSSIcons(true);
	}
}

static void CheckSuperSonicTransform(taskwk* twp, playerwk* pwp)
{
	if (PressedButtons[TWP_PNUM(twp)] & TransformButton && IsEventPerforming() == false)
	{
		// If Super Sonic story is finished & more than 50 rings
		if (RemoveLimitations == false && (IsStoryFinished() == false || GetNumRingM(TWP_PNUM(twp)) < 50)) {
			return;
		}

		TransformSuperSonic(twp, pwp);
	}
}

static void DetransformSuperSonic(taskwk* twp, playerwk* pwp)
{
	if (AlwaysSuperSonic == false)
	{
		pwp->equipment &= ~Upgrades_SuperSonic;
		pwp->item &= ~Powerups_Invincibility;
		
		if (pwp->mj.reqaction >= 134 && pwp->mj.reqaction <= 145)
		{
			DoSonicGroundAnimation((CharObj2*)pwp, (EntityData1*)twp);
		}

		if (twp->flag & Status_Ball)
		{
			twp->flag &= ~(Status_Attack | Status_Ball);
		}

		UnsetSuperAnims(pwp);

		// If it's player 1, play sound & reset music
		if (TWP_PNUM(twp) == 0)
		{
			DetransformMusicAndSound();
			UpdateSSIcons(false);
		}
	}
}

static void CheckSuperSonicDetransform(taskwk* twp, playerwk* pwp)
{
	if (IsSuperSonic(pwp) && ((DetransformButton && PressedButtons[TWP_PNUM(twp)] & TransformButton) || IsEventPerforming()))
	{
		SetInputP(TWP_PNUM(twp), PL_OP_NORMAL);
		DetransformSuperSonic(twp, pwp);
	}
}

static void SuperSonic_Rings(taskwk* twp, playerwk* pwp)
{
	// Consume rings:
	if (RemoveLimitations == false && TimeThing == 1)
	{
		auto pnum = TWP_PNUM(twp);

		if (GetNumRingM(pnum) > 0)
		{
			++RingTimer[pnum] %= 60;
			
			if (RingTimer[pnum] == 0)
			{
				AddNumRingM(pnum, -1);
			}
		}
		else
		{
			DetransformSuperSonic(twp, pwp);
		}
	}
}

static void Sonic_NewActions(taskwk* twp, motionwk2* mwp, playerwk* pwp)
{
	switch (twp->mode)
	{
	case Act_Sonic_Jump:
		if (IsSuperSonic(pwp)) {
			CheckSuperSonicDetransform(twp, pwp);
		}
		else {
			CheckSuperSonicTransform(twp, pwp);
		}

		break;
	case Act_SuperSonic_Jump:
		CheckSuperSonicDetransform(twp, pwp);
		break;
	}
}

static void SuperSonic_Actions(taskwk* twp, motionwk2* mwp, playerwk* pwp)
{
	// Skip if disabled
	if (UseAdvancedSuperSonic() == false || twp->mode == MD_SONIC_KILL)
	{
		return;
	}

	// Run next actions again, fixes weird things
	if (twp->mode != MD_SONIC_STND)
	{
		if (twp->flag & Status_HoldObject)
		{
			Sonic_HoldingObject_NAct((EntityData1*)twp, (CharObj2*)pwp, (EntityData2*)mwp);
		}
		else
		{
			Sonic_NAct((CharObj2*)pwp, (EntityData1*)twp, (EntityData2*)mwp);
		}
	}

	// Use Super Sonic actions when we can, force Sonic's when it's better
	if (IsSuperSonic(pwp))
	{
		switch (twp->mode)
		{
		case MD_SONIC_1DWK:
			twp->mode = MD_SUPER_1DWK;
			break;
		case MD_SUPER_STND:
			twp->mode = MD_SONIC_STND;
			break;
		case MD_SUPER_WALK:
			twp->mode = MD_SONIC_WALK;
			break;
		case MD_SUPER_LNCH:
			twp->mode = MD_SONIC_LNCH;
			break;
		case MD_SUPER_SPRG:
			twp->mode = MD_SONIC_SPRG;
			break;
		case MD_SUPER_SPAT:
			twp->mode = MD_SONIC_SPAT;
			break;
		case MD_SUPER_JUMP:
			twp->mode = MD_SONIC_JUMP;
			break;
		}
	}
}

static bool Blacklist_NormalSuperSonic(taskwk* twp, playerwk* pwp)
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

	if (pwp->mj.reqaction < Anm_SuperSonic_Stand || pwp->mj.reqaction > Anm_SuperSonic_Jump)
	{
		return true;
	}

	return twp->flag & Status_DoNextAction &&
		(twp->smode == PL_OP_PLACEON || (twp->smode == PL_OP_PLACEWITHSPIN && CurrentLevel == LevelIDs_TwinklePark));
}

static void __cdecl Sonic_Exec_r(task* tp)
{
	auto twp = tp->twp;
	auto mwp = (motionwk2*)tp->mwp;
	auto pwp = (playerwk*)mwp->work.ptr;

	if (IsPerfectChaosLevel() == false && MetalSonicFlag == false)
	{
		if (twp->mode == Act_Sonic_Init)
		{
			InitSuperSonicEyes(TWP_PNUM(twp));
			BackupLifeIcon();
			Sonic_Exec_t.Original(tp);
		}
		else
		{
			if (AlwaysSuperSonic == false)
			{
				Sonic_NewActions(twp, mwp, pwp);
			}

			// Super Sonic actions
			if (IsSuperSonic(pwp))
			{
				pwp->item |= Powerups_Invincibility;

				RunSuperMusic();

				SuperSonic_Actions(twp, mwp, pwp); // advanced actions if enabled
				SuperSonic_Rings(twp, pwp); // deplete rings if enabled
				
				// if advanced super sonic is disabled, detransform Super on invalid actions.
				if (UseAdvancedSuperSonic() == false && Blacklist_NormalSuperSonic(twp, pwp))
				{
					DetransformSuperSonic(twp, pwp); 
				}
			}
			else if (AlwaysSuperSonic == true)
			{
				TransformSuperSonic(twp, pwp);
				LoadPVM("SUPERSONIC", &SUPERSONIC_TEXLIST);
				LoadPVM("SUPERSONIC_EXTRA", &SUPERSONIC_EXTRA_TEXLIST);
			}
		}
	}

	Sonic_Exec_t.Original(tp);
	flagSuperSonicMode = IsSuperSonic(0) ? 1 : 0;
}

static void __cdecl Sonic_Delete_r(task* tp)
{
	auto twp = tp->twp;
	
	// Restore things if the player is deleted, useful for compatiblity with Character Select
	if (TWP_PNUM(twp) == 0)
	{
		RestoreMusic();
	}

	Sonic_Delete_t.Original(tp);
}

static Bool  __cdecl SonicNAct_r(taskwk* twp, playerwk* pwp, motionwk2* mwp)
{
	// In case an external mod sets Super Sonic
	if (IsPerfectChaosLevel() == false && twp->smode == PL_OP_SUPER && (twp->flag & Status_DoNextAction))
	{
		SetSuperAnims(pwp);
	}

	return SonicNAct_t.Original(twp, pwp, mwp);
}

static Bool __cdecl SuperSonicNAct_r(motionwk2* mwp, playerwk* pwp, taskwk* twp)
{
	if (IsPerfectChaosLevel() == false && twp->smode == PL_OP_PLACEWITHKIME && (twp->flag & Status_DoNextAction))
	{
		twp->mode = MD_SONIC_PLON;
		pwp->mj.reqaction = Anm_SuperSonic_Win;
		NullifyVelocity((EntityData2*)mwp, (CharObj2*)pwp);
		twp->ang.z = 0;
		twp->ang.x = 0;
		twp->flag &= ~(Status_OnPath | Status_Attack | Status_Ball);
		StopPlayerLookAt(0);
		UnsetSuperAnims(pwp);
		return true;
	}

	return SuperSonicNAct_t.Original(mwp, pwp, twp);
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
	
	// Always initialize Super Sonic welds
	WriteData<2>(reinterpret_cast<Uint8*>(0x0049AC6A), 0x90i8);
}
