#include "pch.h"

/*
* Custom animation list for Super Sonic
*/

static Trampoline* Sonic_WalkAni_t = nullptr;
static Trampoline* Sonic_GroundAnim_t = nullptr;

static NJS_OBJECT* SuperSonicEyeList[3];
static AnimationFile* customAnims[4] = {};
static AnimData_t SuperSonicAnimData[SonicAnimData_Length];

static bool animationsLoaded = false;

void SetSuperAnims(CharObj2* co2)
{
	if (UseAdvancedSuperSonic() == true)
	{
		co2->AnimationThing.AnimData = SuperSonicAnimData;
	}
}

void UnsetSuperAnims(CharObj2* co2)
{
	if (UseAdvancedSuperSonic() == true)
	{
		co2->AnimationThing.AnimData = SonicAnimData;
	}
}

void InitSuperSonicAnims()
{
	if (animationsLoaded == false)
	{
		if (UseAdvancedSuperSonic() == true)
		{
			// Fill the animation table at init in case mods edit animations
			for (int i = 0; i < SonicAnimData_Length; ++i)
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
					SuperSonicAnimData[i] = SonicAnimData[i];
					break;
				case Anm_Sonic_JumpOrSpin:
				case Anm_Sonic_Bowling:
				case Anm_Sonic_Roll:
				case Anm_Sonic_PinBall:
					SuperSonicAnimData[i] = SonicAnimData[Anm_SuperSonic_Jump];
					SuperSonicAnimData[i].Property = SonicAnimData[i].Property;
					break;
				default:
					SuperSonicAnimData[i] = SonicAnimData[i];
					SuperSonicAnimData[i].Animation = new NJS_ACTION();
					SuperSonicAnimData[i].Animation->object = SONIC_OBJECTS[22];
					SuperSonicAnimData[i].Animation->motion = SonicAnimData[i].Animation->motion;
					break;
				}
			}

			if (CustomAnims == true)
			{
				SuperSonicAnimData[19].Animation->motion = customAnims[0]->getmotion();
				SuperSonicAnimData[66].Animation->motion = customAnims[1]->getmotion();
				SuperSonicAnimData[67].Animation->motion = customAnims[2]->getmotion();
				SuperSonicAnimData[68].Animation->motion = customAnims[3]->getmotion();
			}

			if (AlwaysSuperSonic == true)
			{
				CharSelDataList[0].anonymous_1[0] = SonicAnimData[Anm_SuperSonic_Stand].Animation;
				CharSelDataList[0].anonymous_1[1]->object = SONIC_OBJECTS[22];
				CharSelDataList[0].anonymous_1[2]->object = SONIC_OBJECTS[22];
				CharSelDataList[0].TextureList = &SUPERSONIC_TEXLIST;
			}
		}
		
		animationsLoaded = true;
	}
}

void InitSuperSonicEyes(int player)
{
	if (EyeTracking == true)
	{
		CollisionCrashThing_Load(SuperSonicEyeList, player);
	}
}

static void __cdecl Sonic_WalkAni_r(EntityData1* data, EntityData2* data2, CharObj2* co2)
{
	if (IsSuperSonic(co2))
	{
		SuperSonic_WalkAni(co2, data2);
	}
	else
	{
		const auto original = Sonic_WalkAni_t->Target();

		__asm
		{
			mov eax, co2
			mov edi, data2
			mov ecx, data
			
			call original
		}
	}
}

static void __declspec(naked) Sonic_WalkAni_asm()
{
	__asm
	{
		push eax
		push edi
		push ecx
		call Sonic_WalkAni_r
		pop ecx
		pop edi
		pop eax
		retn
	}
}

static void __cdecl Sonic_GroundAnim_r(EntityData1* data, CharObj2* co2)
{
	if (IsSuperSonic(co2))
	{
		co2->AnimationThing.Index = Anm_SuperSonic_Stand;
	}
	else
	{
		const auto original = Sonic_GroundAnim_t->Target();

		__asm
		{
			mov esi, co2
			mov ebx, data

			call original
		}
	}
}

static void __declspec(naked) Sonic_GroundAnim_asm()
{
	__asm
	{
		push esi
		push ebx
		call Sonic_GroundAnim_r
		pop ebx
		pop esi
		retn
	}
}

void Animations_Init()
{
	if (UseAdvancedSuperSonic() == true)
	{
		Sonic_WalkAni_t = new Trampoline(0x495CD0, 0x495CD5, Sonic_WalkAni_asm);
		Sonic_GroundAnim_t = new Trampoline(0x491660, 0x49166B, Sonic_GroundAnim_asm);
		
		if (CustomAnims == true)
		{
			LoadAnimationFile(&customAnims[0], "SS_011");
			LoadAnimationFile(&customAnims[1], "SS_051");
			LoadAnimationFile(&customAnims[2], "SS_053");
			LoadAnimationFile(&customAnims[3], "SS_052");
		}
	}

	if (EyeTracking == true)
	{
		SuperSonicEyeList[0] = SONIC_OBJECTS[22]->child->child->sibling->sibling->sibling->sibling->sibling;
		SuperSonicEyeList[1] = SONIC_OBJECTS[22]->child->child->sibling->sibling->sibling->sibling->sibling->child->child->sibling->sibling->child->child->sibling;
		SuperSonicEyeList[2] = SONIC_OBJECTS[22]->child->child->sibling->sibling->sibling->sibling->sibling->child->child->sibling->child->child->sibling;
	}
}