#include "pch.h"

Trampoline* Sonic_SuperPhysics_Load_t = nullptr;
Trampoline* Sonic_DisplayLightDashModel_t = nullptr;

static bool CustomPhysics = true;

static float SuperSonicDecel = *(float*)0x49439D; // -0.001f
static float SuperSonicAirDecel = *(float*)0x4943A7; // -0.002f
static float SuperSonicAirAccel = *(float*)0x4943B1; // 0.05f
static float SuperSonicYOff = *(float*)0x4943BB; // 6.4f

void ResetSuperPhysics(PhysicsData* physdata, int character) {
	physdata->RollDecel = PhysicsArray[character].RollDecel;
	physdata->AirDecel = PhysicsArray[character].AirDecel;
	physdata->AirAccel = PhysicsArray[character].AirAccel;
	physdata->YOff = PhysicsArray[character].YOff;
}

void SetSuperPhysics(PhysicsData* physdata) {
	physdata->RollDecel = SuperSonicDecel;
	physdata->AirDecel = SuperSonicAirDecel;
	physdata->AirAccel = SuperSonicAirAccel;
	physdata->YOff = SuperSonicYOff;
}

void __cdecl Sonic_SuperPhysicsLevel_Delete(task* tsk) {
	CharObj2* co2 = CharObj2Ptrs[tsk->awp->work.sl[0]];
	
	if (co2 != nullptr) {
		ResetSuperPhysics(&co2->PhysicsData, EntityData1Ptrs[tsk->awp->work.sl[0]]->CharID);
	}

	// Restore Super Sonic's win height
	WriteData((float*)0x494E16, 10.0f);
}

void __cdecl Sonic_SuperPhysicsLevel_Main(task* tsk) {
	CharObj2* co2 = CharObj2Ptrs[tsk->awp->work.sl[0]];
	
	// If an event occurs, remove the new physics temporarily
	if (tsk->awp->work.sl[1] == 0) {
		if (IsEventPerforming() == true) {
			ResetSuperPhysics(&co2->PhysicsData, Characters_Sonic);
			tsk->awp->work.sl[1] = 1;
		}
	}
	else {
		if (IsEventPerforming() == false) {
			SetSuperPhysics(&co2->PhysicsData);
			tsk->awp->work.sl[1] = 0;
		}
	}
	

	if (co2 == nullptr || (co2->Upgrades & Upgrades_SuperSonic) != Upgrades_SuperSonic) {
		FreeTask(tsk);
	}
}

void __cdecl Sonic_SuperPhysics_Load_r(task* tsk) {
	if (CurrentLevel == LevelIDs_PerfectChaos) {
		TARGET_DYNAMIC(Sonic_SuperPhysics_Load);
	}
	else {
		CharObj2* co2 = CharObj2Ptrs[tsk->awp->work.sl[0]];

		tsk->exec = Sonic_SuperPhysicsLevel_Main;
		tsk->dest = Sonic_SuperPhysicsLevel_Delete;

		SetSuperPhysics(&co2->PhysicsData);

		// Remove Super Sonic's win height in levels
		WriteData((float*)0x494E16, 0.0f);
		
		tsk->exec(tsk);
	}
}

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
		NonStaticFunctionPointer(void, original, (EntityData1 * data, EntityData2 * data2, CharObj2 * co2), Sonic_DisplayLightDashModel_t->Target());
		original(data, data2, co2);
	}
}

void Objects_Init(const IniFile* config, const IniFile* physics) {
	Sonic_SuperPhysics_Load_t = new Trampoline((int)Sonic_SuperPhysics_Load, (int)Sonic_SuperPhysics_Load + 0x8, Sonic_SuperPhysics_Load_r);
	
	// Fixes upside down water plane in Emerald Coast 2
	if (GeoLists[LevelIDs_EmeraldCoast * 8 + 1] == &Geo_ECoast2 && Geo_ECoast2.Col[1].Flags & ColFlags_Water) {
		NJS_OBJECT* obj = Geo_ECoast2.Col[1].Model;
		obj->ang[0] = 32768;
		obj->pos[1] = -3.0f;
		obj->pos[2] = -5850.0f;
	}

	if (config->getBool("General", "ExtendedGameplay", true)) {
		WriteJump(LoadSonicDashTrail, LoadSonicDashTrail_r);
		WriteJump((void*)0x4940B0, LoadSonicDashEffect_r);
		WriteData((ObjectFuncPtr*)0x49AE58, SonicChargeSpindashEffect_r);
		Sonic_DisplayLightDashModel_t = new Trampoline((int)Sonic_DisplayLightDashModel, (int)Sonic_DisplayLightDashModel + 0x5, Sonic_DisplayLightDashModel_r);
	}

	if (config->getBool("General", "CustomPhysics", true)) {
		SuperSonicDecel = physics->getFloat("SuperSonic", "SuperSonicDecel", -0.006f);
		SuperSonicAirDecel = physics->getFloat("SuperSonic", "SuperSonicAirDecel", -0.02f);
		SuperSonicAirAccel = physics->getFloat("SuperSonic", "SuperSonicAirAccel", 0.035f);
	}

	if (config->getBool("General", "AlwaysSuperSonic", false)) {
		WriteJump(Sonic_SuperAura_Load, Sonic_SuperAura_Load_r); // disable the aura in the Character Select screen
	}
}