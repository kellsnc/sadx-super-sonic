#include "pch.h"
#include "SADXModLoader.h"
#include "config.h"

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
	if (CurrentLevel != LevelIDs_PerfectChaos && ChangeMusic == true && CurrentSong == MusicIDs_ThemeOfSuperSonic)
	{
		if (!Music_Enabled)
		{
			return;
		}

		LastSong = CurrentSong = level_song;
	}
}

void SetSuperMusic()
{
	if (CurrentLevel == LevelIDs_PerfectChaos || !Music_Enabled || CurrentSong == MusicIDs_ThemeOfSuperSonic || EV_MainThread_ptr)
	{
		return;
	}

	last_level = CurrentLevel;
	last_act = CurrentAct;

	level_song = LastSong;
	LastSong = CurrentSong = MusicIDs_ThemeOfSuperSonic;
}

void RunSuperMusic()
{
	if (CurrentLevel != LevelIDs_PerfectChaos && ChangeMusic == true)
	{
		if (CurrentSong != -1 && (CurrentLevel != last_level || CurrentAct != last_act))
		{
			SetSuperMusic();
		}
	}
}

void TransformMusicAndSound()
{
	if (EnableTransformationVoice == true)
	{
		PlayVoice(396);
	}
	
	if (ChangeMusic == true)
	{
		SetSuperMusic();
	}
}

void DetransformMusicAndSound()
{
	if (EnableTransformationVoice == true)
	{
		PlayVoice(clips[rand() % LengthOfArray(clips)]);
	}

	RestoreMusic();
}
