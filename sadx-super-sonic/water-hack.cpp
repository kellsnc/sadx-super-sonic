#include "pch.h"

// Code by SonicFreak94

static int __stdcall SuperWaterCheck_C(EntityData1* data1, CharObj2* data2)
{
	auto pad = ControllerPointers[static_cast<int>(data1->CharIndex)];

	if (data1->CharID == Characters_Sonic && (data2->Upgrades & Upgrades_SuperSonic) != 0)
	{
		return pad && !(pad->HeldButtons & AttackButtons);
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

void WaterHack_Init(const IniFile* config) {
	WriteJump(reinterpret_cast<void*>(0x004496E1), SuperWaterCheck);
}