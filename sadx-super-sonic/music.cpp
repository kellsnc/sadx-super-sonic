#include "pch.h"

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
		if (song == MusicIDs::MusicIDs_ThemeOfSuperSonic && CurrentSong != song) {
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

void TransformMusicAndSound() {
	PlayVoice(396);

	if (ChangeMusic == true) {
		PlayMusic_r(MusicIDs::MusicIDs_ThemeOfSuperSonic);
	}
}

void DetransformMusicAndSound() {
	PlayVoice(clips[rand() % LengthOfArray(clips)]);
	RestoreMusic();
}

void Music_Init() {
	if (ChangeMusic) {
		WriteJump(PlayMusic, PlayMusic_r);
	}
}