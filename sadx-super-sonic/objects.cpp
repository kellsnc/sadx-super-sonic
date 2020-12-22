#include "pch.h"

Trampoline* Sonic_SuperPhysics_Load_t = nullptr;

void __cdecl Sonic_SuperPhysicsLevel_Delete(task* tsk) {
	CharObj2* co2 = CharObj2Ptrs[tsk->awp->work.sl[0]];
	EntityData1* data = EntityData1Ptrs[tsk->awp->work.sl[0]]; // Use the actual character index to be compatible with Character Select

	if (co2 != nullptr) {
		memcpy(&co2->PhysicsData, &PhysicsArray[data->CharID], sizeof(PhysicsData));	
	}

	// Restore Super Sonic's win height
	WriteData((float*)0x494E16, 10.0f);
}

void __cdecl Sonic_SuperPhysicsLevel_Main(task* tsk) {
	CharObj2* co2 = CharObj2Ptrs[tsk->awp->work.sl[0]];
	
	if (co2 != nullptr && co2->Upgrades & Upgrades_SuperSonic) {
		co2->PhysicsData.RollDecel = -0.006f;
		co2->PhysicsData.AirDecel = -0.02f;
		co2->PhysicsData.AirAccel = 0.035f;
		co2->PhysicsData.YOff = 6.4f;
	}
	else {
		FreeTask(tsk);
	}
}

void __cdecl Sonic_SuperPhysics_Load_r(task* tsk) {
	if (CurrentLevel == LevelIDs_PerfectChaos) {
		TARGET_DYNAMIC(Sonic_SuperPhysics_Load);
	}
	else {
		tsk->exec = Sonic_SuperPhysicsLevel_Main;
		tsk->dest = Sonic_SuperPhysicsLevel_Delete;

		// Remove Super Sonic's win height in levels
		WriteData((float*)0x494E16, 0.0f);
		
		tsk->exec(tsk);
	}
}

void Objects_Init() {
	Sonic_SuperPhysics_Load_t = new Trampoline((int)Sonic_SuperPhysics_Load, (int)Sonic_SuperPhysics_Load + 0x8, Sonic_SuperPhysics_Load_r);
	
	// Fixes upside down water plane in Emerald Coast 2
	if (GeoLists[LevelIDs_EmeraldCoast * 8 + 1] == &Geo_ECoast2 && Geo_ECoast2.Col[1].Flags & ColFlags_Water) {
		NJS_OBJECT* obj = Geo_ECoast2.Col[1].Model;
		obj->ang[0] = 32768;
		obj->pos[1] = -3.0f;
		obj->pos[2] = -5850.0f;
	}

	// Always initialize Super Sonic weld data.
	WriteData<2>(reinterpret_cast<Uint8*>(0x0049AC6A), 0x90i8);
}