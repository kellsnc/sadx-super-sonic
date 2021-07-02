#include "pch.h"

// Custom Tikal orb for the unused voice, spawns near Sonic at stage start.

static Trampoline* LoadLevel_t = nullptr;

static const char* const tikal_message_en[] = {
	"Gather fifty rings and press the action\nbutton while you jump.",
	"You'll transform into Super Sonic!\nBut watch out for your ring consumption!",
	NULL,
};

static const char* const tikal_message_fr[] = {
	"Collectez cinquante anneaux et appuyez sur le\nbouton d'action pendant un saut.",
	"Vous vous transformerez en Super Sonic !\nMais attention à vos anneaux !",
	NULL,
};

static const char* const tikal_message_es[] = {
	"Reúne 50 anillos y presiona el botón\nde acción mientras saltas.",
	"¡Te transformarás en Súper Sonic!\n¡Sólo cuida el consumo de anillos!",
	NULL,
};

static const char* const tikal_message_ru[] = {
	"Собрав 50 колец, нажми кнопку\nдействия в прыжке.",
	"Ты превратишься в Супер Соника!\nНо следи за числом колец!",
	NULL,
};

static const char* const* tikal_messages[] = {
	tikal_message_en, //jp
	tikal_message_en,
	tikal_message_fr,
	tikal_message_es,
	tikal_message_en, //ge
	tikal_message_ru // extra
};

enum SSTikalHintActs
{
	SSTikalHint_GetToPosition,
	SSTikalHint_AtPosition,
	SSTikalHint_RunCheck,
	SSTikalHint_FollowPlayer,
	SSTikalHint_Vanish
};

static void SSTikalHintCheck(ObjectMaster* obj, EntityData1* data, EntityData1* player)
{
	if (IsPlayerInsideSphere(&data->Position, 7.0f) == 1)
	{
		if (player->CharIndex == Characters_Sonic)
		{
			PlayVoice(1676);
			DisplayHintText(tikal_messages[RussianMod == true && TextLanguage == Languages_English ? 5 : TextLanguage], 260);
			NullifyVelocity(EntityData2Ptrs[0], CharObj2Ptrs[0]);
			SetHintChildData(obj->Child, 4.0f, 1.0f);
			PlaySound(6, 0, 0, 0);
			data->Action = SSTikalHint_FollowPlayer;
		}
		else
		{
			data->Scale = data->Position;
			PlaySound3D(760, obj, 1, 0, -1, data);
			data->Action = SSTikalHint_Vanish;
		}
		
	}
}

static void GetTikalPosition(ObjectMaster* obj, EntityData1* data, EntityData1* player)
{
	float y = GetGroundYPosition(data->Position.x, data->Position.y, data->Position.z, &data->Rotation);

	if (y < -999999.0f)
	{
		// Move orb to player if out of bounds, wait a bit for sky deck.
		if (TimeMinutes < 1 && TimeSeconds > 2)
		{
			NJS_VECTOR pos;

			GetBufferedPositionAndRotation(0, 0, &pos, 0);

			pos.x = njSin(FrameCounterUnpaused * 600) * 15.0f + pos.x;
			pos.z = njCos(FrameCounterUnpaused * 600) * 15.0f + pos.z;
			pos.y = (njSin(FrameCounterUnpaused * 600) * 0.5f + 1.0f) * 5.0f + pos.y;

			SetHintPos(&data->Position, &pos, 1.0f);
		}
	}
	else
	{
		if (data->Position.y > y + 15.0f)
		{
			data->Position.y -= 0.75f;
			SetPlayerEyeTarget(0, obj, &data->Position);
		}
		else
		{
			data->Action = SSTikalHint_AtPosition;
		}
	}
}

static void __cdecl SSTikalHint_Main(ObjectMaster* obj)
{
	EntityData1* player = EntityData1Ptrs[0];
	
	if (!ClipSetObject(obj))
	{
		EntityData1* data = obj->Data1;
		NJS_VECTOR pos = {};

		switch (data->Action)
		{
		case SSTikalHint_GetToPosition:
			GetTikalPosition(obj, data, player);
			SSTikalHintCheck(obj, data, player);

			break;
		case SSTikalHint_AtPosition:
			SetPlayerEyeTarget(0, obj, &data->Position);
			data->Action = SSTikalHint_RunCheck;
			break;
		case SSTikalHint_RunCheck:
			SSTikalHintCheck(obj, data, player);
			break;
		case SSTikalHint_FollowPlayer:
			data->Rotation.x += 695;

			GetBufferedPositionAndRotation(0, 0, &pos, 0);
			
			pos.x = njSin(data->Rotation.x) * 15.0f + pos.x;
			pos.z = njCos(data->Rotation.x) * 15.0f + pos.z;
			pos.y = (njSin(data->Rotation.x) * 0.5f + 1.0f) * 5.0f + pos.y;

			SetHintPos(&data->Position, &pos, 1.0f);

			SetPlayerEyeTarget(0, obj, &data->Position);

			if (data->Rotation.x > 420000)
			{
				StopPlayerEyeTarget(0);
				data->Action = SSTikalHint_Vanish;
				data->Rotation.x = 0;
				data->Scale = data->Position;
				PlaySound3D(760, obj, 1, 0, -1, data);
			}

			break;
		case SSTikalHint_Vanish:
			data->Rotation.x += 895;

			data->Position.x = data->Scale.x - njCos(data->Rotation.x) * 5.0f;
			data->Position.z = data->Scale.z - njSin(data->Rotation.x) * 5.0f;
			data->Position.y += 0.75f;

			if (++data->InvulnerableTime > 240)
			{
				DeleteObject_(obj);
			}

			break;
		}

		RunObjectChildren(obj);
	}
}

static void LoadLevel_r()
{
	TARGET_DYNAMIC(LoadLevel)();

	if (EnableTikalUnusedVoice == true && CurrentCharacter == Characters_Sonic && IsStoryFinished() == true &&
		RemoveLimitations == false && TimeMinutes < 1 && TimeSeconds < 10 && CurrentLevel < LevelIDs_Chaos0 && GameMode == GameModes_Adventure_ActionStg)
	{
		ObjectMaster* obj = LoadObject(LoadObj_Data1, 5, SSTikalHint_Main);

		if (obj)
		{
			EntityData1* data = obj->Data1;

			if (data)
			{
				LoadTikalChild(obj, &data->CharIndex, &data->Position);

				obj->DeleteSub = TikalHint_Delete;

				MovePlayerToStartPoint(data);

				data->Position.x = data->Position.x - njCos(data->Rotation.y + 0xE000) * 15.0f;
				data->Position.y = data->Position.y + 15.0f;
				data->Position.z = data->Position.z - njSin(data->Rotation.y + 0xE000) * 15.0f;
			}
		}
	}
}

void SSTikalHint_Init()
{
	LoadLevel_t = new Trampoline(0x4159A0, 0x4159A7, LoadLevel_r, false);
}