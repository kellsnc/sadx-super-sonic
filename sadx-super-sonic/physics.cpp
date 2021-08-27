#include "pch.h"

// Code to replace Super Sonic physics with custom, softer ones

static Trampoline* Sonic_SuperPhysics_Load_t = nullptr;

static float SuperSonicDecel = *(float*)0x49439D; // -0.001f
static float SuperSonicAirDecel = *(float*)0x4943A7; // -0.002f
static float SuperSonicAirAccel = *(float*)0x4943B1; // 0.05f
static float SuperSonicYOff = *(float*)0x4943BB; // 6.4f

static void ResetSuperPhysics(PhysicsData* physdata, int character)
{
	physdata->RollDecel = PhysicsArray[character].RollDecel;
	physdata->AirDecel = PhysicsArray[character].AirDecel;
	physdata->AirAccel = PhysicsArray[character].AirAccel;
	physdata->YOff = PhysicsArray[character].YOff;
}

static void SetSuperPhysics(PhysicsData* physdata)
{
	physdata->RollDecel = SuperSonicDecel;
	physdata->AirDecel = SuperSonicAirDecel;
	physdata->AirAccel = SuperSonicAirAccel;
	physdata->YOff = SuperSonicYOff;
}

static void __cdecl Sonic_SuperPhysicsLevel_Delete(task* tsk)
{
	CharObj2* co2 = CharObj2Ptrs[tsk->awp->work.sl[0]];

	if (co2 != nullptr)
	{
		ResetSuperPhysics(&co2->PhysicsData, EntityData1Ptrs[tsk->awp->work.sl[0]]->CharID);
	}
}

static void __cdecl Sonic_SuperPhysicsLevel_Main(task* tsk)
{
	CharObj2* co2 = CharObj2Ptrs[tsk->awp->work.sl[0]];

	// If an event occurs, remove the new physics temporarily
	if (tsk->awp->work.sl[1] == 0)
	{
		if (IsEventPerforming() == true)
		{
			ResetSuperPhysics(&co2->PhysicsData, Characters_Sonic);
			tsk->awp->work.sl[1] = 1;
		}
	}
	else
	{
		if (IsEventPerforming() == false)
		{
			SetSuperPhysics(&co2->PhysicsData);
			tsk->awp->work.sl[1] = 0;
		}
	}

	if (co2 == nullptr || IsSuperSonic(co2) == false)
	{
		FreeTask(tsk);
	}
}

static void __cdecl Sonic_SuperPhysics_Load_r(task* tsk)
{
	if (IsPerfectChaosLevel())
	{
		TARGET_DYNAMIC(Sonic_SuperPhysics_Load)(tsk);

		// Restore Super Sonic's win height hack
		WriteData(reinterpret_cast<int*>(0x00494E13), 0x002446C7);
		WriteData(reinterpret_cast<float*>(0x494E16), 10.0f);
	}
	else
	{
		CharObj2* co2 = CharObj2Ptrs[tsk->awp->work.sl[0]];

		tsk->exec = Sonic_SuperPhysicsLevel_Main;
		tsk->dest = Sonic_SuperPhysicsLevel_Delete;

		SetSuperPhysics(&co2->PhysicsData);

		// Remove Super Sonic's win height hack
		WriteData<7>(reinterpret_cast<Uint8*>(0x00494E13), 0x90i8);

		tsk->exec(tsk);
	}
}

void Physics_Init(const char* path)
{
	Sonic_SuperPhysics_Load_t = new Trampoline((int)Sonic_SuperPhysics_Load, (int)Sonic_SuperPhysics_Load + 0x6, Sonic_SuperPhysics_Load_r);

	if (CustomPhysics == true)
	{
		const IniFile* physics = new IniFile(std::string(path) + "\\physics.ini");

		SuperSonicDecel = physics->getFloat("SuperSonic", "SuperSonicDecel", -0.006f);
		SuperSonicAirDecel = physics->getFloat("SuperSonic", "SuperSonicAirDecel", -0.02f);
		SuperSonicAirAccel = physics->getFloat("SuperSonic", "SuperSonicAirAccel", 0.035f);

		delete physics;
	}
}