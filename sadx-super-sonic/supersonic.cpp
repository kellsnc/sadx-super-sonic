#include "pch.h"

static Trampoline* Sonic_Exec_t = nullptr;
static Trampoline* Sonic_Display_t = nullptr;
static Trampoline* Sonic_Delete_t = nullptr;

void Sonic_Display_r(task* tsk) {
	CharObj2* co2 = (CharObj2*)tsk->mwp->work.ptr;

	// SuperSonicFlag is just a flag to change sonic's lighting, no other use
	if (co2->Upgrades & Upgrades_SuperSonic) {
		SuperSonicFlag = 1;
	}
	else {
		SuperSonicFlag = 0;
	}

	GamePlay_HackDisplay((EntityData1*)tsk->twp, co2);

	TARGET_DYNAMIC(Sonic_Display)(tsk);
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
	if (PressedButtons[data->CharIndex] & TransformButton) {
		// If Super Sonic story is finished & more than 50 rings
		if (RemoveLimitations == false && (!GetEventFlag(EventFlags_SuperSonicAdventureComplete) || (data->CharIndex == 0 && Rings < 50))) {
			return;
		}

		GamePlay_SetSuperAnims();

		ForcePlayerAction(data->CharIndex, NextAction_SuperSonic);

		// If it's player 1, play sound & update music
		if (data->CharIndex == 0) {
			TransformMusicAndSound();
		}
	}
}

void DetransformSuperSonic(EntityData1* data, CharObj2* co2) {
	ForcePlayerAction(data->CharIndex, NextAction_SuperSonicStop);
	co2->Powerups &= ~Powerups_Invincibility;

	GamePlay_UnsetSuperAnims();

	// If it's player 1, play sound & reset music
	if (data->CharIndex == 0) {
		DetransformMusicAndSound();
	}
}

void CheckSuperSonicDetransform(EntityData1* data, CharObj2* co2) {
	if (co2->Upgrades & Upgrades_SuperSonic && DetransformButton == true && PressedButtons[data->CharIndex] & TransformButton) {
		DetransformSuperSonic(data, co2);
	}
}

void SuperSonicAct(EntityData1* data, CharObj2* co2) {
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
		if (co2->Upgrades & Upgrades_SuperSonic) {
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

	// Series of hack to allow Sonic's actions for Super Sonic
	GamePlay_HackActions(data, mwp, co2);
}

void Sonic_Exec_r(task* tsk) {
	EntityData1* data = (EntityData1*)tsk->twp;
	motionwk* mwp = tsk->mwp;
	CharObj2* co2 = (CharObj2*)mwp->work.ptr;

	if (LastStoryFlag == false && MetalSonicFlag == false) {
		if (data->Action == Act_Sonic_Init) {

			// Force load Super Sonic if option enabled, otherwise check for Tikal's unused voice
			if (AlwaysSuperSonic == true) {
				ForcePlayerAction(data->CharIndex, NextAction_SuperSonic);
				LoadPVM("SUPERSONIC", &SUPERSONIC_TEXLIST);
				GamePlay_SetSuperAnims();
			}
			else {
				CheckTikalVoice(data, co2);
			}
		}
		else {

			// Only transform/detransform or decrease rings if AlwaysSuperSonic disabled.
			if (AlwaysSuperSonic == false) {
				Sonic_NewActions(data, mwp, co2);

				if (co2->Upgrades & Upgrades_SuperSonic) {
					SuperSonicAct(data, co2); // decrease rings, stuff
				}
			}
		}
	}

	TARGET_DYNAMIC(Sonic_Exec)(tsk);
}

void Sonic_Delete_r(task* tsk) {
	EntityData1* data = (EntityData1*)tsk->twp;
	CharObj2* co2 = (CharObj2*)tsk->mwp->work.ptr;

	// Restore things if the player is deleted, useful for compatiblity with Character Select
	if (LastStoryFlag == false && co2->Upgrades & Upgrades_SuperSonic && data->CharIndex == 0) {
		RestoreMusic();
	}

	// Just in case
	GamePlay_UnsetSuperAnims();

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