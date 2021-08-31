#include "pch.h"

// Code to adapt some objects to Super Sonic

DataArray(CollisionData, JumpPanel_Collision_, 0x97DF68, 4);

static Trampoline* Sonic_DisplayLightDashModel_t = nullptr;

static void LoadSonicDashTrail_r(EntityData1* player)
{
	ObjectMaster* obj = nullptr;
	CharObj2* co2 = CharObj2Ptrs[player->CharIndex];

	if (IsSuperSonic(co2) == true)
	{
		obj = LoadObject(LoadObj_Data1, 6, (ObjectFuncPtr)0x55FB80);
	}
	else
	{
		obj = LoadObject(LoadObj_Data1, 6, SonicDashTrail_Init);
	}

	if (obj)
	{
		obj->Data1->CharIndex = player->CharIndex;
	}
}

static void LoadSonicDashEffect_r(EntityData1* player)
{
	ObjectMaster* obj = nullptr;
	CharObj2* co2 = CharObj2Ptrs[player->CharIndex];

	if (IsSuperSonic(co2) == true)
	{
		obj = LoadObject(LoadObj_Data1, 5, (ObjectFuncPtr)0x55FB20);
	}
	else
	{
		obj = LoadObject(LoadObj_Data1, 5, (ObjectFuncPtr)0x4A2A40);
	}

	if (obj)
	{
		obj->Data1->CharIndex = player->CharIndex;
	}
}

static void SonicChargeSpindashEffect_r(ObjectMaster* obj)
{
	CharObj2* co2 = CharObj2Ptrs[obj->Data1->CharIndex];

	if (IsSuperSonic(co2) == false)
	{
		SonicChargeSpindashEffect(obj);
	}
}

static void Sonic_SuperAura_Load_r(ObjectMaster* obj)
{
	// Disable in menus or option enabled
	if (GameState == 21 || !(DisableAura == AuraOptions::False || (DisableAura == AuraOptions::ExceptBoss && IsPerfectChaosLevel())))
	{
		obj->MainSub = DeleteObject_;
		return;
	}

	obj->MainSub = Sonic_SuperAura_Main;
	obj->DisplaySub = Sonic_SuperAura_Display;
	obj->DeleteSub = Sonic_SuperAura_Delete;
	*(char*)0x3C69DD0 = 0;
}

static void __cdecl Sonic_DisplayLightDashModel_r(EntityData1* data, EntityData2* data2, CharObj2* co2)
{
	if (IsSuperSonic(co2) == false)
	{
		TARGET_DYNAMIC(Sonic_DisplayLightDashModel)(data, data2, co2);
	}
}

void Objects_Init()
{
	if (ExtendedGamePlay == true)
	{
		WriteJump(LoadSonicDashTrail, LoadSonicDashTrail_r);
		WriteJump((void*)0x4940B0, LoadSonicDashEffect_r);
		WriteData((ObjectFuncPtr*)0x49AE58, SonicChargeSpindashEffect_r);

		Sonic_DisplayLightDashModel_t = new Trampoline((int)Sonic_DisplayLightDashModel, (int)Sonic_DisplayLightDashModel + 0x5, Sonic_DisplayLightDashModel_r);
		WriteCall((void*)0x494B27, Sonic_DisplayLightDashModel_r); // DC Conversion compatibility
	}

	WriteJump(Sonic_SuperAura_Load, Sonic_SuperAura_Load_r);

	// Fix Jump Panel collision placement
	JumpPanel_Collision_[1].center.y = 4.0f;
	JumpPanel_Collision_[2].center.y = 4.0f;
	JumpPanel_Collision_[3].center.y = 2.0f;
	JumpPanel_Collision_[3].a = 7.0f;
}