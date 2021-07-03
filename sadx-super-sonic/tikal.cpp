#include "pch.h"

// Custom Tikal orb for the unused voice, spawns near Sonic at stage start.

static Trampoline* LoadLevel_t = nullptr;

static const char* const tikal_message_jp[] = {
	"\a\203\212\203\223\203\117\202\36050\214\302\217\127\202\337\202\304\201\101\n\203\127\203\203\203\223\203\166\222\206\202\311\203\101\203\116\203\126\203\207\203\223\203\173\203\136\203\223\202\360\211\237\202\271\202\316\201\102" /* "\aリングを50個集めて、\nジャンプ中にアクションボタンを押せば。" */,
	"\a\203\130\201\133\203\160\201\133\203\134\203\152\203\142\203\116\202\311\225\317\220\147\202\305\202\253\202\351\202\315\202\270\202\346\201\111\n\203\212\203\223\203\117\202\314\217\301\224\357\202\311\213\103\202\360\202\302\202\257\202\304\201\111" /* "\aスーパーソニックに変身できるはずよ！\nリングの消費に気をつけて！" */,
	NULL
};

static const char* const tikal_message_en[] = {
	"Gather 50 rings and press the action\nbutton while you jump.",
	"You'll transform into Super Sonic!\nBut watch out for your ring consumption!",
	NULL
};

static const char* const tikal_message_fr[] = {
	"Collectez 50 anneaux et appuyez sur le\nbouton d'action pendant un saut.",
	"Vous vous transformerez en Super Sonic !\nMais attention à vos anneaux !",
	NULL
};

static const char* const tikal_message_es[] = {
	"Reúne 50 anillos y presiona el botón\nde acción mientras saltas.",
	"¡Te transformarás en Súper Sonic!\n¡Sólo cuida el consumo de anillos!",
	NULL
};

static const char* const tikal_message_ge[] = {
	"Sammle 50 Ringe und drücke die Aktionstaste,\nwährend du springst.",
	"Du wirst dich in Super Sonic verwandeln!\nAber achten Sie auf Ihren Ringverbrauch!",
	NULL
};

__declspec(dllexport) const char* const* tikal_messages[] = {
	tikal_message_jp,
	tikal_message_en,
	tikal_message_fr,
	tikal_message_es,
	tikal_message_ge
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
		if (player->CharIndex == Characters_Sonic && IsSuperSonic(CharObj2Ptrs[0]) == false)
		{
			PlayVoice(1676);
			DisplayHintText(tikal_messages[TextLanguage], 260);
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