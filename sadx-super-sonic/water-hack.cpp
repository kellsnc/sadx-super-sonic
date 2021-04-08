#include "pch.h"

// By SonicFreak94, prevents Sonic from going through water planes

static int __stdcall SuperWaterCheck_C(EntityData1* data1, CharObj2* data2)
{
	auto pad = ControllerPointers[static_cast<int>(data1->CharIndex)];

	if (data1->CharID == Characters_Sonic && (data2->Upgrades & Upgrades_SuperSonic) != 0)
	{
		return pad && !(pad->HeldButtons & Buttons_B);
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

void WaterHack_Init() {
	WriteJump(reinterpret_cast<void*>(0x004496E1), SuperWaterCheck);

	// Fixes upside down water plane in Emerald Coast 2
	if (GeoLists[LevelIDs_EmeraldCoast * 8 + 1] == &Geo_ECoast2 && Geo_ECoast2.Col[1].Flags & ColFlags_Water) {
		NJS_OBJECT* obj = Geo_ECoast2.Col[1].Model;
		obj->ang[0] = 32768;
		obj->pos[1] = -3.0f;
		obj->pos[2] = -5850.0f;
	}
}