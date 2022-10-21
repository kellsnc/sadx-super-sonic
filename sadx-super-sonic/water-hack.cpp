#include "pch.h"
#include "SADXModLoader.h"
#include "config.h"

// By SonicFreak94, prevents Sonic from going through water planes
static int __stdcall SuperWaterCheck_C(taskwk* twp, playerwk* pwp)
{
	auto pad = ControllerPointers[TWP_PNUM(twp)];

	if (/*TWP_CHAR(twp) == Characters_Sonic &&*/ IsSuperSonic(pwp))
	{
		return pad && !(pad->HeldButtons & Buttons_Y);
	}

	return false;
}

static const void* surface_solid = reinterpret_cast<void*>(0x004496E7);
static const void* surface_water = reinterpret_cast<void*>(0x004497B6);

static void __declspec(naked) SuperWaterCheck()
{
	__asm
	{
		// If Gamma, treat surface as solid
		jnz not_gamma
		jmp surface_solid

		not_gamma :
		// Save whatever's in EAX
		push eax

		push[esp + 6A8h + 4h + 0Ch]	// CharObj2
		push ebx						// EntityData1
		call SuperWaterCheck_C

		test eax, eax

		jnz is_true

		// Restore EAX
		pop eax

		jmp surface_water

		is_true :
		// Restore EAX
		pop eax
		jmp surface_solid
	}
}

void WaterHack_Init()
{
	if (WalkOnWater == true)
	{
		WriteJump(reinterpret_cast<void*>(0x004496E1), SuperWaterCheck);

		if (PatchWater == true)
		{
			// Fixes upside down water plane in Emerald Coast 2
			if (GeoLists[LevelIDs_EmeraldCoast * 8 + 1] == &Geo_ECoast2 && Geo_ECoast2.Col[1].Flags & ColFlags_Water)
			{
				NJS_OBJECT* obj = Geo_ECoast2.Col[1].Model;
				obj->ang[0] = 32768;
				obj->pos[1] = -3.0f;
				obj->pos[2] = -5850.0f;
			}
		}
	}
}
