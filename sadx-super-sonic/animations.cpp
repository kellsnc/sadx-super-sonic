#include "pch.h"
#include "SADXModLoader.h"
#include "UsercallFunctionHandler.h"
#include "config.h"

/*
* Custom animation list for Super Sonic
*/

extern "C" __declspec(dllexport) unsigned int SSAnimCount = SonicAnimData.size();
extern "C" __declspec(dllexport) PL_ACTION * SSAnimData = sonic_action;

UsercallFuncVoid(SonicChangeRunningMotion_h, (taskwk* twp, motionwk2* mwp, playerwk* pwp), (twp, mwp, pwp), 0x495CD0, rECX, rEDI, rEAX);
UsercallFuncVoid(SonicChangeWaitingMotion_h, (taskwk* twp, playerwk* pwp), (twp, pwp), 0x491660, rEBX, rESI);

static NJS_OBJECT* SuperSonicEyeList[3];
static PL_ACTION* SuperSonicAnimData;

static bool animationsLoaded = false;

static void InitSuperSonicAnims()
{
	if (UseAdvancedSuperSonic() == true)
	{
		SuperSonicAnimData = new PL_ACTION[SSAnimCount];

		// Fill the animation table at init in case mods edit animations
		for (int i = 0; i < SSAnimCount; ++i)
		{
			switch (i)
			{
			case Anm_SuperSonic_Stand:
			case Anm_SuperSonic_StandToMove:
			case Anm_SuperSonic_Move1:
			case Anm_SuperSonic_Move2:
			case Anm_SuperSonic_Move3:
			case Anm_SuperSonic_Spring:
			case Anm_SuperSonic_SpringFall:
			case Anm_SuperSonic_Fall:
			case Anm_SuperSonic_Land:
			case Anm_SuperSonic_Win:
			case Anm_SuperSonic_WinToStand:
			case Anm_SuperSonic_Jump:
				SuperSonicAnimData[i] = SSAnimData[i];
				break;
			case Anm_Sonic_JumpOrSpin:
			case Anm_Sonic_Bowling:
			case Anm_Sonic_Roll:
			case Anm_Sonic_PinBall:
			case Anm_Sonic_SpinBall:
				SuperSonicAnimData[i] = SSAnimData[Anm_SuperSonic_Jump];
				SuperSonicAnimData[i].mtnmode = SSAnimData[i].mtnmode;
				break;
			default:
				SuperSonicAnimData[i] = SSAnimData[i];
				if (SSAnimData[i].actptr)
				{
					SuperSonicAnimData[i].actptr = new NJS_ACTION();
					SuperSonicAnimData[i].actptr->object = SSAnimData[134].actptr->object; //take model from SS list to allow mods to replace model
					SuperSonicAnimData[i].actptr->motion = SSAnimData[i].actptr->motion;
				}
				break;
			}
		}

		if (AlwaysSuperSonic == true)
		{
			CharSelDataList[0].anonymous_1[0] = SSAnimData[Anm_SuperSonic_Stand].actptr;
			CharSelDataList[0].anonymous_1[1]->object = SONIC_OBJECTS[22];
			CharSelDataList[0].anonymous_1[2]->object = SONIC_OBJECTS[22];
			CharSelDataList[0].TextureList = &SUPERSONIC_TEXLIST;
		}
	}
}

void SetSuperAnims(playerwk* pwp)
{
	if (animationsLoaded == false)
	{
		InitSuperSonicAnims();
		animationsLoaded = true;
	}

	if (UseAdvancedSuperSonic() == true)
	{
		pwp->mj.plactptr = SuperSonicAnimData;
	}
}

void UnsetSuperAnims(playerwk* pwp)
{
	if (UseAdvancedSuperSonic() == true)
	{
		pwp->mj.plactptr = pSonicAnimData;
	}
}

void InitSuperSonicEyes(int player)
{
	if (EyeTracking == true)
	{
		CollisionCrashThing_Load(SuperSonicEyeList, player);
	}
}

static void __cdecl SonicChangeRunningMotion_r(taskwk* twp, motionwk2* mwp, playerwk* pwp)
{
	if (IsSuperSonic(pwp))
	{
		SonicChangeRunningMotionS(pwp, mwp);
	}
	else
	{
		SonicChangeRunningMotion_h.Original(twp, mwp, pwp);
	}
}

static void __cdecl SonicChangeWaitingMotion_r(taskwk* twp, playerwk* pwp)
{
	if (IsSuperSonic(pwp))
	{
		pwp->mj.reqaction = Anm_SuperSonic_Stand;
	}
	else
	{
		SonicChangeWaitingMotion_h.Original(twp, pwp);
	}
}


void Animations_Init()
{
	if (UseAdvancedSuperSonic() == true)
	{
		SonicChangeRunningMotion_h.Hook(SonicChangeRunningMotion_r);
		SonicChangeWaitingMotion_h.Hook(SonicChangeWaitingMotion_r);

		if (CustomAnims == true)
		{

			auto anim = (NJS_MDATA3*)SONIC_ACTIONS[11]->motion->mdata;

			if (anim && &anim[22])
			{
				*anim[22].p = nullptr;
			}

			for (uint8_t i = 51; i < 54; i++)
			{
				auto d = (NJS_MDATA3*)SONIC_ACTIONS[i]->motion->mdata;
				if (d && &d[22])
					*d[22].p = nullptr;
			}
		}
	}

	if (EyeTracking == true)
	{
		SuperSonicEyeList[0] = SONIC_OBJECTS[22]->child->child->sibling->sibling->sibling->sibling->sibling;
		SuperSonicEyeList[1] = SONIC_OBJECTS[22]->child->child->sibling->sibling->sibling->sibling->sibling->child->child->sibling->sibling->child->child->sibling;
		SuperSonicEyeList[2] = SONIC_OBJECTS[22]->child->child->sibling->sibling->sibling->sibling->sibling->child->child->sibling->child->child->sibling;
	}
}
