#include "pch.h"
#include "Characters/Common.h"
#include "Characters/Sonic.h"

void Objects_Init();
void WaterHack_Init();

Trampoline* Sonic_Exec_t = nullptr;
Trampoline* Sonic_Display_t = nullptr;
Trampoline* Sonic_Delete_t = nullptr;

static const int clips[] = {
	402,
	508,
	874,
	1427,
	1461
};

inline void RestoreMusic() {
	if (CurrentSong == MusicIDs_ThemeOfSuperSonic) {
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
	if (LastStoryFlag == false && PressedButtons[data->CharIndex] & Buttons_B) {
		ForcePlayerAction(data->CharIndex, NextAction_SuperSonic);

		// If it's player 1, play sound & update music
		if (data->CharIndex == 0) {
			PlayVoice(396);
			PlayMusic_r(MusicIDs::MusicIDs_ThemeOfSuperSonic);
		}
	}
}

void CheckSuperSonicDetransform(EntityData1* data, motionwk* mwp, CharObj2* co2) {
	if (co2->Upgrades & Upgrades_SuperSonic && PressedButtons[data->CharIndex] & Buttons_B) {
		ForcePlayerAction(data->CharIndex, NextAction_SuperSonicStop);
		
		if (data->CharIndex == 0) {
			PlayVoice(clips[rand() % LengthOfArray(clips)]);
			RestoreMusic();
		}
	}
}

void Sonic_NewActions(EntityData1* data, motionwk* mwp, CharObj2* co2) {
	switch (data->Action) {
	case Act_Sonic_Jump:
		CheckSuperSonicTransform(data, mwp, co2);
		break;
	case Act_SuperSonic_Jump:
		CheckSuperSonicDetransform(data, mwp, co2);
		break;
	}
}

void Sonic_Display_r(task* tsk) {
	EntityData1* data = (EntityData1*)tsk->twp;
	CharObj2* co2 = (CharObj2*)tsk->mwp->work.ptr;

	// SuperSonicFlag is just a flag to change sonic's lighting, no other use
	if (co2->Upgrades & Upgrades_SuperSonic) {
		SuperSonicFlag = 1;
	}
	else {
		SuperSonicFlag = 0;
	}

	TARGET_DYNAMIC(Sonic_Display)(tsk);
}

void Sonic_Exec_r(task* tsk) {
	EntityData1* data = (EntityData1*)tsk->twp; // main task containing position, rotation, scale
	motionwk* mwp = tsk->mwp; // task containing movement information
	CharObj2* co2 = (CharObj2*)mwp->work.ptr; // physics, animation info, and countless other things

	Sonic_NewActions(data, mwp, co2);

	TARGET_DYNAMIC(Sonic_Exec)(tsk);
}

void Sonic_Delete_r(task* tsk) {
	EntityData1* data = (EntityData1*)tsk->twp;
	CharObj2* co2 = (CharObj2*)tsk->mwp->work.ptr;

	// Restore things if the player is deleted, useful for compatiblity with Character Select
	if (co2->Upgrades & Upgrades_SuperSonic && data->CharIndex == 0) {
		RestoreMusic();
	}

	TARGET_DYNAMIC(Sonic_Delete)(tsk);
}

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions) {
		Sonic_Exec_t = new Trampoline((int)Sonic_Main, (int)Sonic_Main + 0x7, Sonic_Exec_r);
		Sonic_Display_t = new Trampoline((int)Sonic_Display, (int)Sonic_Display + 0x7, Sonic_Display_r);
		Sonic_Delete_t = new Trampoline((int)Sonic_Delete, (int)Sonic_Delete + 0x5, Sonic_Delete_r);
		WriteJump(PlayMusic, PlayMusic_r);

		Objects_Init();
		WaterHack_Init();
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}