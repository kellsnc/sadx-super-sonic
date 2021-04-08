#include "pch.h"

// Main code for transforming, detransforming, losing rings, etc.

static Trampoline* Sonic_Exec_t = nullptr;
static Trampoline* Sonic_Display_t = nullptr;
static Trampoline* Sonic_Delete_t = nullptr;

bool IsSuperSonic(CharObj2* co2) {
	return (co2->Upgrades & Upgrades_SuperSonic);
}

void Sonic_Display_r(task* tsk) {
	CharObj2* co2 = (CharObj2*)tsk->mwp->work.ptr;

	// Just a flag to change sonic's lighting, no other use
	SuperSonicFlag = IsSuperSonic(co2) == true ? 1 : 0;

	// If the advanced config is enabled, use a custom drawing function
	if (SuperSonicFlag && UseAdvancedSuperSonic()) {
		SuperSonic_Display((EntityData1*)tsk->twp, co2);
	}
	else {
		TARGET_DYNAMIC(Sonic_Display)(tsk);
	}
}

void CheckTikalVoice(EntityData1* data, CharObj2* co2) {
	if (EnableTikalUnusedVoice == true && GetEventFlag(EventFlags_SuperSonicAdventureComplete) &&
		GetEventFlag((EventFlags)0x39) == false && RemoveLimitations == false &&
		GameState == 15 && CurrentLevel < LevelIDs_Chaos0) {
		PlayVoice(1676);
		SetEventFlag((EventFlags)0x39);
		SaveSave();
	}
}

void CheckSuperSonicTransform(EntityData1* data, CharObj2* co2) {
	if (PressedButtons[data->CharIndex] & TransformButton && IsEventPerforming() == false) {
		
		// If Super Sonic story is finished & more than 50 rings
		if (RemoveLimitations == false && (!GetEventFlag(EventFlags_SuperSonicAdventureComplete) || (data->CharIndex == 0 && Rings < 50))) {
			return;
		}

		ForcePlayerAction(data->CharIndex, NextAction_SuperSonic);

		// If it's player 1, play sound & update music
		if (data->CharIndex == 0) {
			TransformMusicAndSound();
		}
	}
}

void DetransformSuperSonic(EntityData1* data, CharObj2* co2) {
	if (AlwaysSuperSonic == false) {
		ForcePlayerAction(data->CharIndex, NextAction_SuperSonicStop);
		co2->Powerups &= ~Powerups_Invincibility;

		// If it's player 1, play sound & reset music
		if (data->CharIndex == 0) {
			DetransformMusicAndSound();
		}
	}
}

void CheckSuperSonicDetransform(EntityData1* data, CharObj2* co2) {
	if (IsSuperSonic(co2) == true && ((DetransformButton == true && PressedButtons[data->CharIndex] & TransformButton) || IsEventPerforming() == true)) {
		DetransformSuperSonic(data, co2);
	}
}

void SuperSonic_Rings(EntityData1* data, CharObj2* co2) {
	co2->Powerups |= Powerups_Invincibility;

	// Consume rings:
	if (RemoveLimitations == false && data->CharIndex == 0 && TimeThing == 1) {
		if (Rings > 0) {
			if (FrameCounterUnpaused % 60 == 0) {
				AddRings(-1);
			}
		}
		else {
			DetransformSuperSonic(data, co2);
		}
	}
}

void Sonic_NewActions(EntityData1* data, motionwk* mwp, CharObj2* co2) {
	switch (data->Action) {
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

bool Blacklist_NormalSuperSonic(EntityData1* data, CharObj2* co2) {
	switch (CurrentLevel) {
	default:
		break;
	case LevelIDs_Casinopolis:
		return CurrentAct == 2 || CurrentAct == 3;
	case LevelIDs_SpeedHighway:
		return CurrentAct == 1;
	}

	if (co2->AnimationThing.Index < Anm_SuperSonic_Stand || co2->AnimationThing.Index > Anm_SuperSonic_Jump) {
		return true;
	}

	return data->Status & Status_DoNextAction &&
		(data->NextAction == 12 || (data->NextAction == 13 && CurrentLevel == LevelIDs_TwinklePark));
}

void Sonic_Exec_r(task* tsk) {
	EntityData1* data = (EntityData1*)tsk->twp;
	motionwk* mwp = tsk->mwp;
	CharObj2* co2 = (CharObj2*)mwp->work.ptr;

	if (LastStoryFlag == false && MetalSonicFlag == false) {
		if (data->Action == Act_Sonic_Init) {
			CheckTikalVoice(data, co2);
		}
		else {
			if (AlwaysSuperSonic == false) {
				Sonic_NewActions(data, mwp, co2);
			}

			// Super Sonic actions
			if (IsSuperSonic(co2) == true) {
				SuperSonic_Actions(data, mwp, co2);
				SuperSonic_Rings(data, co2);

				// if advanced super sonic is disabled, detransform Super on invalid actions.
				if (UseAdvancedSuperSonic() == false && Blacklist_NormalSuperSonic(data, co2)) {
					DetransformSuperSonic(data, co2); 
				}
			}

			// Always force Super Sonic if enabled
			if (AlwaysSuperSonic == true && IsSuperSonic(co2) == false) {
				ForcePlayerAction(data->CharIndex, NextAction_SuperSonic);
				LoadPVM("SUPERSONIC", &SUPERSONIC_TEXLIST);
			}
		}
	}

	TARGET_DYNAMIC(Sonic_Exec)(tsk);
}

void Sonic_Delete_r(task* tsk) {
	EntityData1* data = (EntityData1*)tsk->twp;
	CharObj2* co2 = (CharObj2*)tsk->mwp->work.ptr;

	// Restore things if the player is deleted, useful for compatiblity with Character Select
	if (LastStoryFlag == false && IsSuperSonic(co2) == true && data->CharIndex == 0) {
		RestoreMusic();
	}

	TARGET_DYNAMIC(Sonic_Delete)(tsk);
}

void SuperSonic_Init() {
	HelperFunctionsGlobal.RegisterCharacterPVM(Characters_Sonic, { "SUPERSONIC", &SUPERSONIC_TEXLIST });

	Sonic_Exec_t = new Trampoline((int)Sonic_Main, (int)Sonic_Main + 0x7, Sonic_Exec_r);
	Sonic_Display_t = new Trampoline((int)Sonic_Display, (int)Sonic_Display + 0x7, Sonic_Display_r);
	Sonic_Delete_t = new Trampoline((int)Sonic_Delete, (int)Sonic_Delete + 0x5, Sonic_Delete_r);
	
	// Always initialize Super Sonic weld data
	WriteData<2>(reinterpret_cast<Uint8*>(0x0049AC6A), 0x90i8);
}