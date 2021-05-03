#include "pch.h"

// Changes to force the last boss music if the option enabled, and to perform transformation voices

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

void TransformMusicAndSound() {
	if (EnableTransformationVoice == true) {
		PlayVoice(396);
	}
	
	if (ChangeMusic == true) {
		LastSong = CurrentSong;
		CurrentSong = MusicIDs::MusicIDs_ThemeOfSuperSonic;
	}
}

void DetransformMusicAndSound() {
	if (EnableTransformationVoice == true) {
		PlayVoice(clips[rand() % LengthOfArray(clips)]);
	}

	RestoreMusic();
}