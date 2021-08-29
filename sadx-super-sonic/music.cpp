#include "pch.h"

/*
* Changes to force the last boss music if the option enabled, and to perform transformation voices
*/

static short last_level = 0;
static short last_act = 0;
static int   level_song = 0;

static const int clips[] = {
	402,
	508,
	874,
	1427,
	1461
};

void RestoreMusic()
{
	if (ChangeMusic == true && CurrentSong == MusicIDs_ThemeOfSuperSonic)
	{
		if (!Music_Enabled)
		{
			return;
		}

		LastSong = CurrentSong = level_song;
	}
}

void RunSuperMusic()
{
	if (!Music_Enabled || CurrentSong == MusicIDs_ThemeOfSuperSonic || ChangeMusic == false)
	{
		return;
	}

	last_level = CurrentLevel;
	last_act = CurrentAct;

	level_song = LastSong;
	LastSong = CurrentSong = MusicIDs_ThemeOfSuperSonic;
}

void TransformMusicAndSound()
{
	if (EnableTransformationVoice == true)
	{
		PlayVoice(396);
	}
	
	RunSuperMusic();
}

void DetransformMusicAndSound()
{
	if (EnableTransformationVoice == true)
	{
		PlayVoice(clips[rand() % LengthOfArray(clips)]);
	}

	RestoreMusic();
}