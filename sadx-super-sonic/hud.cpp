#include "pch.h"

enum SUPERSONIC_EXTRA
{
	SUPERSONIC_EXTRA_LIFE,
	SUPERSONIC_EXTRA_1UP
};

static NJS_TEXANIM SUPERSONIC_EXTRA_TEXANIM[2] = {
	{ 0x20, 0x20, 0, 0, 0, 0, 0x100, 0x100, SUPERSONIC_EXTRA_LIFE, 0x20 },
	{ 0x20, 0x20, 0x10, 0x10, 0, 0, 0xFF, 0xFF, SUPERSONIC_EXTRA_1UP, 0x20 }
};

static NJS_SPRITE SUPERSONIC_EXTRA_SPRITE = { {}, 1.0f, 1.0f, 0, &SUPERSONIC_EXTRA_TEXLIST, SUPERSONIC_EXTRA_TEXANIM };

#pragma pack(push, 1)
struct AirBoxWK
{
	int thing;
	int id;
	uint8_t gap8[12];
	int dword14;
};
#pragma pack(pop)

DataPointer(AirBoxWK*, airBoxData, 0x3C5A9D4);

static void __cdecl upIconHack1(NJS_MODEL_SADX* model)
{
	EntityData1* player = EntityData1Ptrs[0];

	if (player && player->CharID == Characters_Sonic && ItemBox_CurrentItem == 6 && !VerifyTexList(&SUPERSONIC_EXTRA_TEXLIST))
	{
		CharObj2* co2 = CharObj2Ptrs[0];

		if (co2 && IsSuperSonic(co2))
		{
			NJS_TEXLIST* tex = CurrentTexList;
			njSetTexture(&SUPERSONIC_EXTRA_TEXLIST);
			model->mats->attr_texId = SUPERSONIC_EXTRA_1UP;
			DrawModel(model);
			njSetTexture(tex);
			return;
		}
	}

	DrawModel(model);
}

static void __cdecl upIconHack2(NJS_MODEL_SADX* model)
{
	EntityData1* player = EntityData1Ptrs[0];

	if (player && player->CharID == Characters_Sonic && airBoxData->id == 6 && !VerifyTexList(&SUPERSONIC_EXTRA_TEXLIST))
	{
		CharObj2* co2 = CharObj2Ptrs[0];

		if (co2 && IsSuperSonic(co2))
		{
			NJS_TEXLIST* tex = CurrentTexList;
			njSetTexture(&SUPERSONIC_EXTRA_TEXLIST);
			model->mats->attr_texId = SUPERSONIC_EXTRA_1UP;
			DrawModel(model);
			njSetTexture(tex);
			return;
		}
	}

	DrawModel(model);
}

static void __cdecl upIconHack3(NJS_SPRITE* sp, Int n, Float pri, NJD_SPRITE attr, QueuedModelFlagsB queue_flags)
{
	EntityData1* player = EntityData1Ptrs[0];

	if (player && player->CharID == Characters_Sonic && sp->tanim->texid == LifeTextures[Characters_Sonic] && !VerifyTexList(&SUPERSONIC_EXTRA_TEXLIST))
	{
		CharObj2* co2 = CharObj2Ptrs[0];

		if (co2 && IsSuperSonic(co2))
		{
			sp->tlist = &SUPERSONIC_EXTRA_TEXLIST;
			sp->tanim = SUPERSONIC_EXTRA_TEXANIM;
			njDrawSprite2D_ForcePriority(sp, SUPERSONIC_EXTRA_1UP, pri, attr);
			return;
		}
	}

	njDrawSprite2D_ForcePriority(sp, n, pri, attr);
}

// Hack the icon sprite drawing function, if player 1 is super sonic, swap for another sprite.
static void __cdecl lifeIconHack(NJS_SPRITE* sp, Int n, Float pri, NJD_SPRITE attr)
{
	EntityData1* player = EntityData1Ptrs[0];

	if (player && player->CharID == Characters_Sonic && !VerifyTexList(&SUPERSONIC_EXTRA_TEXLIST))
	{
		CharObj2* co2 = CharObj2Ptrs[0];

		if (co2 && IsSuperSonic(co2))
		{
			SUPERSONIC_EXTRA_SPRITE.p.x = Hud_RingTimeLife.p.x;
			SUPERSONIC_EXTRA_SPRITE.p.y = Hud_RingTimeLife.p.y;
			njDrawSprite2D_ForcePriority(&SUPERSONIC_EXTRA_SPRITE, SUPERSONIC_EXTRA_LIFE, pri, attr);
			return;
		}
	}

	njDrawSprite2D_ForcePriority(sp, n, pri, attr);
}

void HudInit()
{
	if (HUDIcons == true)
	{
		WriteCall((void*)0x425E74, lifeIconHack);
		WriteCall((void*)0x4D6944, upIconHack1);
		WriteCall((void*)0x4D6AC0, upIconHack1);
		WriteCall((void*)0x4C0066, upIconHack2);
		WriteCall((void*)0x4C03C0, upIconHack3);
	}
}