#include "pch.h"

void GamePlay_HackDisplay(EntityData1* data, CharObj2* co2);
void GamePlay_HackActions(EntityData1* data, motionwk* mwp, CharObj2* co2);

static const PVMEntry SuperSonicPVM = { "SUPERSONIC", &SUPERSONIC_TEXLIST };

static Trampoline* Sonic_Exec_t = nullptr;
static Trampoline* Sonic_Display_t = nullptr;
static Trampoline* Sonic_Delete_t = nullptr;

static bool ChangeMusic = true;
static bool RemoveLimitations = false;
static bool AlwaysSuperSonic = false;

static const int clips[] = {
	402,
	508,
	874,
	1427,
	1461
};

void RestoreMusic() {
	if (ChangeMusic == true && CurrentSong == MusicIDs_ThemeOfSuperSonic) {
		CurrentSong = LastSong;
	}
}

void __cdecl PlayMusic_r(MusicIDs song) {
	if (Music_Enabled) {

		// If the Super Sonic Theme is requested, keep the level song in LastSong
		if (song == MusicIDs::MusicIDs_ThemeOfSuperSonic) {
			LastSong = CurrentSong;
			CurrentSong = song;
		}

		// If the Super Sonic Theme theme is currently playing, save the new registered song for later
		else if (CurrentSong == MusicIDs::MusicIDs_ThemeOfSuperSonic) {
			LastSong = song;
		}

		// Else normal behaviour
		else {
			CurrentSong = song;
			LastSong = song;
		}
	}
}

void CheckSuperSonicTransform(EntityData1* data, motionwk* mwp, CharObj2* co2) {
	if (PressedButtons[data->CharIndex] & Buttons_B) {
		
		// If Super Sonic story is finished & more than 50 rings
		if (RemoveLimitations == false && (!GetEventFlag(EventFlags_SuperSonicAdventureComplete) || (data->CharIndex == 0 && Rings < 50))) {
			return;
		}

		ForcePlayerAction(data->CharIndex, NextAction_SuperSonic);

		// If it's player 1, play sound & update music
		if (data->CharIndex == 0) {
			PlayVoice(396);

			if (ChangeMusic == true) {
				PlayMusic_r(MusicIDs::MusicIDs_ThemeOfSuperSonic);
			}
		}
	}
}

void DetransformSuperSonic(EntityData1* data, CharObj2* co2) {
	ForcePlayerAction(data->CharIndex, NextAction_SuperSonicStop);
	co2->Powerups &= ~Powerups_Invincibility;

	if (data->CharIndex == 0) {
		PlayVoice(clips[rand() % LengthOfArray(clips)]);
		RestoreMusic();
	}
}

void CheckSuperSonicDetransform(EntityData1* data, motionwk* mwp, CharObj2* co2) {
	if (co2->Upgrades & Upgrades_SuperSonic && PressedButtons[data->CharIndex] & Buttons_B) {
		DetransformSuperSonic(data, co2);
	}
}

// Handle Super Sonic things common to every action (ring decrease, invincibility...)
bool HandleSuperSonicState(EntityData1* data, CharObj2* co2) {
	if (co2->Upgrades & Upgrades_SuperSonic) {
		co2->Powerups |= Powerups_Invincibility;

		// Consume rings:
		if (RemoveLimitations == false && data->CharIndex == 0) {
			if (Rings > 0) {
				if (FrameCounterUnpaused % 60 == 0) {
					AddRings(-1);
				}
			}
			else {
				DetransformSuperSonic(data, co2);
				return false;
			}
		}
	}

	// Always force Super Sonic if enabled
	else if (AlwaysSuperSonic == true) {
		ForcePlayerAction(data->CharIndex, NextAction_SuperSonic);
		LoadPVM(SuperSonicPVM.Name, SuperSonicPVM.TexList); // failsafe, it's not always loaded

		return false;
	}

	return true;
}

void Sonic_NewActions(EntityData1* data, motionwk* mwp, CharObj2* co2) {
	if (HandleSuperSonicState(data, co2)) {

		// Skip transformation actions if AlwaysSuperSonic is enabled
		if (AlwaysSuperSonic == false) {
			switch (data->Action) {
			case Act_Sonic_Jump:
				if (co2->Upgrades & Upgrades_SuperSonic) {
					CheckSuperSonicDetransform(data, mwp, co2);
				}
				else {
					CheckSuperSonicTransform(data, mwp, co2);
				}

				break;
			case Act_SuperSonic_Jump:
				if (CurrentLevel != LevelIDs_PerfectChaos) {
					CheckSuperSonicDetransform(data, mwp, co2);
				}

				break;
			}
		}
	}

	// Series of hack to allow Sonic's actions for Super Sonic
	GamePlay_HackActions(data, mwp, co2);
}

void Sonic_Display_r(task* tsk) {
	EntityData1* data = (EntityData1*)tsk->twp;
	CharObj2* co2 = (CharObj2*)tsk->mwp->work.ptr;

	// If in last story or metal sonic, don't run custom super sonic
	if (LastStoryFlag == false && MetalSonicFlag == false) {

		// SuperSonicFlag is just a flag to change sonic's lighting, no other use
		if (co2->Upgrades & Upgrades_SuperSonic) {
			SuperSonicFlag = 1;
		}
		else {
			SuperSonicFlag = 0;
		}

		// Series of hacks to allow Sonic's animations on Super Sonic
		GamePlay_HackDisplay(data, co2);
	}

	TARGET_DYNAMIC(Sonic_Display)(tsk);
}

void Sonic_Exec_r(task* tsk) {
	EntityData1* data = (EntityData1*)tsk->twp;
	motionwk* mwp = tsk->mwp;
	CharObj2* co2 = (CharObj2*)mwp->work.ptr;

	if (data->Action != Act_Sonic_Init && LastStoryFlag == false && MetalSonicFlag == false) {
		Sonic_NewActions(data, mwp, co2); // Add the transform and detransform actions
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

	TARGET_DYNAMIC(Sonic_Delete)(tsk);
}

void SuperSonic_Init(const HelperFunctions& helperFunctions, const IniFile* config) {
	helperFunctions.RegisterCharacterPVM(Characters_Sonic, SuperSonicPVM);

	Sonic_Exec_t = new Trampoline((int)Sonic_Main, (int)Sonic_Main + 0x7, Sonic_Exec_r);
	Sonic_Display_t = new Trampoline((int)Sonic_Display, (int)Sonic_Display + 0x7, Sonic_Display_r);
	Sonic_Delete_t = new Trampoline((int)Sonic_Delete, (int)Sonic_Delete + 0x5, Sonic_Delete_r);
	
	// Always initialize Super Sonic weld data
	WriteData<2>(reinterpret_cast<Uint8*>(0x0049AC6A), 0x90i8);

	// Read configuration
	ChangeMusic = config->getBool("General", "ChangeMusic", true);
	RemoveLimitations = config->getBool("General", "RemoveLimitations", false);
	AlwaysSuperSonic = config->getBool("General", "AlwaysSuperSonic", false);

	if (AlwaysSuperSonic == true) {
		RemoveLimitations = true;
	}

	// Apply music changes
	if (ChangeMusic) {
		WriteJump(PlayMusic, PlayMusic_r);
	}
}