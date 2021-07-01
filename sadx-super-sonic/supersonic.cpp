#include "pch.h"

// Main code for transforming, detransforming, losing rings, etc.

static Trampoline* Sonic_Exec_t = nullptr;
static Trampoline* Sonic_Display_t = nullptr;
static Trampoline* Sonic_Delete_t = nullptr;

static const char* const tikal_message_en[] = {
	"Gather fifty rings and press the action\nbutton while you jump.",
	"You'll transform in Super Sonic!\nBut watch out for your ring consumption!",
	NULL,
};

static const char* const tikal_message_fr[] = {
	"Collectez cinquante anneaux et appuyez sur le\nbouton d'action pendant un saut.",
	"Vous vous transformerez en Super Sonic !\nMais attention à vos anneaux !",
	NULL,
};

static const char* const tikal_message_ru[] = {
	"Собрав 50 колец, нажми кнопку\nдействия в прыжке.",
	"Ты превратишься в Супер Соника!\nНо следи за числом колец!",
	NULL,
};

static const char* const* tikal_messages[] = {
	tikal_message_en, //jp
	tikal_message_en, //en
	tikal_message_fr, //fr
	tikal_message_en, //sp
	tikal_message_en, //ge
	tikal_message_ru // extra
};

bool IsSuperSonic(CharObj2* co2)
{
	return (co2->Upgrades & Upgrades_SuperSonic);
}

static bool IsStoryFinished()
{
	return GetEventFlag(EventFlags_SuperSonicAdventureComplete);
}

static void Sonic_Display_r(task* tsk)
{
	CharObj2* co2 = (CharObj2*)tsk->mwp->work.ptr;

	// Just a flag to change sonic's lighting, no other use
	SuperSonicFlag = IsSuperSonic(co2) == true ? 1 : 0;

	// If Advanced Super Sonic is enabled, do trickery to play any animation with SS.
	SuperSonic_HackDisplay(co2);

	TARGET_DYNAMIC(Sonic_Display)(tsk);
}

static void CheckTikalVoice(EntityData1* data, CharObj2* co2)
{
	if (EnableTikalUnusedVoice == true && IsStoryFinished() == true &&
	GetEventFlag((EventFlags)0x39) == false && RemoveLimitations == false &&
	GameState == 15 && CurrentLevel < LevelIDs_Chaos0)
	{
		PlayVoice(1676);
		SetEventFlag((EventFlags)0x39);
		DisplayHintText(tikal_messages[RussianMod == true && TextLanguage == Languages_English ? 5 : TextLanguage], 260);
		SaveSave();
	}
}

static void TransformSuperSonic(EntityData1* data, CharObj2* co2)
{
	ForcePlayerAction(data->CharIndex, NextAction_SuperSonic);

	SetSuperAnims(co2);

	// If it's player 1, play sound & update music
	if (data->CharIndex == 0) {
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

void DetransformSuperSonic(EntityData1* data, CharObj2* co2)
{
	if (AlwaysSuperSonic == false)
	{
		ForcePlayerAction(data->CharIndex, NextAction_SuperSonicStop);

		co2->Upgrades &= ~(Upgrades_SuperSonic | Powerups_Invincibility | Status_OnPath);
		DoSonicGroundAnimation(co2, data);

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
			if (FrameCounterUnpaused % 60 == 0)
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

static void Sonic_Exec_r(task* tsk)
{
	EntityData1* data = (EntityData1*)tsk->twp;
	motionwk* mwp = tsk->mwp;
	CharObj2* co2 = (CharObj2*)mwp->work.ptr;

	if (LastStoryFlag == false && MetalSonicFlag == false)
	{
		if (data->Action == Act_Sonic_Init)
		{
			InitSuperAnims(data);
			CheckTikalVoice(data, co2);
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
				LoadPVM("SUPERSONIC", &SUPERSONIC_TEXLIST); // just in case
			}
		}
	}

	TARGET_DYNAMIC(Sonic_Exec)(tsk);
}

void Sonic_Delete_r(task* tsk) {
	EntityData1* data = (EntityData1*)tsk->twp;
	
	// Restore things if the player is deleted, useful for compatiblity with Character Select
	if (data->CharIndex == 0)
	{
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