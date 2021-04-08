#include "pch.h"

Trampoline* Sonic_DisplayLightDashModel_t = nullptr;

void LoadSonicDashTrail_r(EntityData1* player) {
	ObjectMaster* obj = nullptr;
	CharObj2* co2 = CharObj2Ptrs[player->CharIndex];

	if (co2->Upgrades & Upgrades_SuperSonic) {
		obj = LoadObject(LoadObj_Data1, 6, (ObjectFuncPtr)0x55FB80);
	}
	else {
		obj = LoadObject(LoadObj_Data1, 6, SonicDashTrail_Init);
	}
	
	if (obj) {
		obj->Data1->CharIndex = player->CharIndex;
	}
}

void LoadSonicDashEffect_r(EntityData1* player) {
	ObjectMaster* obj = nullptr;
	CharObj2* co2 = CharObj2Ptrs[player->CharIndex];

	if (co2->Upgrades & Upgrades_SuperSonic) {
		obj = LoadObject(LoadObj_Data1, 5, (ObjectFuncPtr)0x55FB20);
	}
	else {
		obj = LoadObject(LoadObj_Data1, 5, (ObjectFuncPtr)0x4A2A40);
	}
	
	if (obj) {
		obj->Data1->CharIndex = player->CharIndex;
	}
}

void SonicChargeSpindashEffect_r(ObjectMaster* obj) {
	CharObj2* co2 = CharObj2Ptrs[obj->Data1->CharIndex];

	if ((co2->Upgrades & Upgrades_SuperSonic) != Upgrades_SuperSonic) {
		SonicChargeSpindashEffect(obj);
	}
}

void Sonic_SuperAura_Load_r(ObjectMaster* obj) {
	if (IsIngame()) {
		obj->MainSub = Sonic_SuperAura_Main;
		obj->DisplaySub = Sonic_SuperAura_Display;
		obj->DeleteSub = Sonic_SuperAura_Delete;
		*(char*)0x3C69DD0 = 0;
	}
}

void __cdecl Sonic_DisplayLightDashModel_r(EntityData1* data, EntityData2* data2, CharObj2* co2) {
	if ((co2->Upgrades & Upgrades_SuperSonic) != Upgrades_SuperSonic) {
		TARGET_DYNAMIC(Sonic_DisplayLightDashModel)(data, data2, co2);
	}
}

void Objects_Init() {	
	if (ExtendedGamePlay == true) {
		WriteJump(LoadSonicDashTrail, LoadSonicDashTrail_r);
		WriteJump((void*)0x4940B0, LoadSonicDashEffect_r);
		WriteData((ObjectFuncPtr*)0x49AE58, SonicChargeSpindashEffect_r);
		Sonic_DisplayLightDashModel_t = new Trampoline((int)Sonic_DisplayLightDashModel, (int)Sonic_DisplayLightDashModel + 0x5, Sonic_DisplayLightDashModel_r);
	}

	if (AlwaysSuperSonic == true) {
		WriteJump(Sonic_SuperAura_Load, Sonic_SuperAura_Load_r); // disable the aura in the Character Select screen
	}
}