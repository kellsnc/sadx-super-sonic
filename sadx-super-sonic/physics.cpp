#include "pch.h"
#include "SADXModLoader.h"
#include "FunctionHook.h"
#include "config.h"

// Code to replace Super Sonic physics with custom, softer ones

TaskHook SuperSonicStatusManager_h(0x494350);
FunctionHook<void, taskwk*, motionwk2*, playerwk*> PResetAngle_h(0x443AD0);
TaskHook ObjectSpring_h(0x7A4E50);

static float SuperSonicDecel = *(float*)0x49439D; // -0.001f
static float SuperSonicAirDecel = *(float*)0x4943A7; // -0.002f
static float SuperSonicAirAccel = *(float*)0x4943B1; // 0.05f
static float SuperSonicYOff = *(float*)0x4943BB; // 6.4f

static bool SpringTaken[8]{};

static void ResetSuperPhysics(player_parameter* physdata, int character)
{
	physdata->air_resist = PhysicsArray[character].RollDecel;
	physdata->air_resist_air = PhysicsArray[character].AirDecel;
	physdata->air_accel = PhysicsArray[character].AirAccel;
	physdata->center_height = PhysicsArray[character].YOff;
}

static void SetSuperPhysics(player_parameter* physdata)
{
	physdata->air_resist = SuperSonicDecel;
	physdata->air_resist_air = SuperSonicAirDecel;
	physdata->air_accel = SuperSonicAirAccel;
	physdata->center_height = SuperSonicYOff;
}

static void __cdecl SuperSonicStatusManagerDestructor_r(task* tp)
{
	auto* pwp = playerpwp[tp->awp->work.sl[0]];

	if (pwp != nullptr)
	{
		ResetSuperPhysics(&pwp->p, TWP_PNUM(playertwp[tp->awp->work.sl[0]]));
	}
}

static void __cdecl SuperSonicStatusManagerExecutor_r(task* tp)
{
	auto playerid = tp->awp->work.sl[0];
	auto ptwp = playertwp[playerid];
	auto ppwp = playerpwp[playerid];

	if (ppwp == nullptr || IsSuperSonic((playerwk*)ppwp) == false)
	{
		FreeTask(tp);
		return;
	}

	if (tp->awp->work.sl[1] == 0)
	{
		if (IsEventPerforming() == true)
		{
			// If an event occurs, remove the new physics temporarily
			ResetSuperPhysics(&ppwp->p, Characters_Sonic);
			tp->awp->work.sl[1] = 1;
		}
		else
		{
			// HACK: If a spring has been taken, restore air decel physics
			if (SpringTaken[TWP_CHAR(ptwp)] == true && ptwp->flag & Status_Ground)
			{
				ppwp->p.air_resist_air = SuperSonicAirDecel;
				SpringTaken[TWP_CHAR(ptwp)] = false;
			}
		}
	}
	else
	{
		if (IsEventPerforming() == false)
		{
			SetSuperPhysics(&ppwp->p);
			tp->awp->work.sl[1] = 0;
		}
	}
}

static void __cdecl SuperSonicStatusManager_r(task* tp)
{
	if (IsPerfectChaosLevel())
	{
		SuperSonicStatusManager_h.Original(tp);
	}
	else
	{
		auto pwp = playerpwp[tp->awp->work.sl[0]];

		tp->exec = SuperSonicStatusManagerExecutor_r;
		tp->dest = SuperSonicStatusManagerDestructor_r;

		SetSuperPhysics(&pwp->p);

		tp->exec(tp);
	}
}

// Fix Super Sonic hardcoded behaviour in PResetAngle
static void __cdecl PResetAngle_r(taskwk* twp, motionwk2* mwp, playerwk* pwp)
{
	if (IsPerfectChaosLevel() == false && IsSuperSonic(pwp))
	{
		pwp->equipment &= ~Upgrades_SuperSonic;
		PResetAngle_h.Original(twp, mwp, pwp);
		pwp->equipment |= Upgrades_SuperSonic;
	}
	else
	{
		PResetAngle_h.Original(twp, mwp, pwp);
	}
}

// HACK: Reset air_resist_air when taking a spring. The value is restored in Sonic_SuperPhysics_Main.
static void __cdecl ObjectSpring_r(task* tp)
{
	auto twp = tp->twp;

	if (twp->mode == 2)
	{
		for (auto i = 0; i < MaxPlayers; i++)
		{
			auto ptwp = playertwp[i];
			auto ppwp = playerpwp[i];

			if (ptwp && ppwp)
			{
				if (IsSuperSonic(ppwp))
				{
					ppwp->p.air_resist_air = PhysicsArray[TWP_CHAR(ptwp)].AirDecel;
					SpringTaken[TWP_CHAR(ptwp)] = true;
				}
			}
		}
	}

	ObjectSpring_h.Original(tp);
}

void Physics_Init(const char* path)
{
	SuperSonicStatusManager_h.Hook(SuperSonicStatusManager_r);
	
	if (PatchPhysics == true)
	{
		PResetAngle_h.Hook(PResetAngle_r);
		ObjectSpring_h.Hook(ObjectSpring_r);
	}
	
	if (CustomPhysics == true)
	{
		const IniFile* physics = new IniFile(std::string(path) + "\\physics.ini");

		SuperSonicDecel = physics->getFloat("SuperSonic", "SuperSonicDecel", -0.006f);
		SuperSonicAirDecel = physics->getFloat("SuperSonic", "SuperSonicAirDecel", -0.02f);
		SuperSonicAirAccel = physics->getFloat("SuperSonic", "SuperSonicAirAccel", 0.035f);

		delete physics;
	}
}
