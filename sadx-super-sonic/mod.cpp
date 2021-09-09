#include "pch.h"

// Entry point of the mod

HelperFunctions HelperFunctionsGlobal;

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		HelperFunctionsGlobal = helperFunctions;

		ReadConfig(path);

		SuperSonic_Init(); // Main code to load Super Sonic
		Objects_Init(); // Edit external objects like SpinDash Trail, SuperPhysics...
		WaterHack_Init(); // Hack to allow the player to float on water when Super Sonic
		Animations_Init(); // Allow Sonic actions for Super Sonic
		Physics_Init(path);
		SSTikalHint_Init();
		HudInit();
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}