#include "pch.h"

// Code to replace Super Sonic physics with custom, softer ones

static Trampoline* Sonic_SuperPhysics_Load_t = nullptr;
static Trampoline* ResetAngle_t = nullptr;
static Trampoline* SpringB_Main_t = nullptr;

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
	}
	else
	{
		CharObj2* co2 = CharObj2Ptrs[tsk->awp->work.sl[0]];

		tsk->exec = Sonic_SuperPhysicsLevel_Main;
		tsk->dest = Sonic_SuperPhysicsLevel_Delete;

		SetSuperPhysics(&co2->PhysicsData);

		tsk->exec(tsk);
	}
}

// Fix Super Sonic hardcoded behaviour in ResetAngle
static void __cdecl ResetAngle_r(EntityData1* data1, EntityData2* data2, CharObj2* co2)
{
	if (IsPerfectChaosLevel() == false && co2->Upgrades & Upgrades_SuperSonic)
	{
		co2->Upgrades &= ~Upgrades_SuperSonic;
		TARGET_DYNAMIC(ResetAngle)(data1, data2, co2);
		co2->Upgrades |= Upgrades_SuperSonic;
	}
	else
	{
		TARGET_DYNAMIC(ResetAngle)(data1, data2, co2);
	}
}

void __cdecl SpringB_Main_r(ObjectMaster* obj) {

	EntityData1* data = obj->Data1;

	for (uint8_t i = 0; i < MaxPlayers; i++) {

		if (!EntityData1Ptrs[i])
			continue;

		CharObj2* co2 = CharObj2Ptrs[i];

		if (data->Action == 2) {
			if (IsSuperSonic(co2)) {
				ResetSuperPhysics(&co2->PhysicsData, Characters_Sonic);
			}
		}
	}

	TARGET_DYNAMIC(SpringB_Main)(obj);
}

void Physics_Init(const char* path)
{
	Sonic_SuperPhysics_Load_t = new Trampoline((int)Sonic_SuperPhysics_Load, (int)Sonic_SuperPhysics_Load + 0x6, Sonic_SuperPhysics_Load_r);
	ResetAngle_t = new Trampoline(0x443AD0, 0x443AD7, ResetAngle_r);
	SpringB_Main_t = new Trampoline((int)SpringB_Main, (int)SpringB_Main + 0x5, SpringB_Main_r);

	if (CustomPhysics == true)
	{
		const IniFile* physics = new IniFile(std::string(path) + "\\physics.ini");

		SuperSonicDecel = physics->getFloat("SuperSonic", "SuperSonicDecel", -0.006f);
		SuperSonicAirDecel = physics->getFloat("SuperSonic", "SuperSonicAirDecel", -0.02f);
		SuperSonicAirAccel = physics->getFloat("SuperSonic", "SuperSonicAirAccel", 0.035f);

		delete physics;
	}
}