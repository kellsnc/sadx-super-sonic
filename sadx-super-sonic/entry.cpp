#include "pch.h"

void SuperSonic_Init(const HelperFunctions& helperFunctions, const IniFile* config);
void Objects_Init(const IniFile* config);
void WaterHack_Init(const IniFile* config);
void GamePlay_Init(const IniFile* config);
void SuperSonic_InitAnimTable();

static bool Initialized = false;

extern "C" {
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions) {
		const IniFile* config = new IniFile(std::string(path) + "\\config.ini");

		SuperSonic_Init(helperFunctions, config); // Main code to load Super Sonic
		Objects_Init(config); // Edit external objects like SpinDash Trail, SuperPhysics...
		WaterHack_Init(config); // Hack to allow the player to float on water when Super Sonic
		GamePlay_Init(config); // Allow Sonic actions for Super Sonic

		delete config;
	}

	__declspec(dllexport) void __cdecl OnFrame() {
		if (Initialized == false && SonicAnimData->Animation->motion != nullptr) {
			SuperSonic_InitAnimTable();
			Initialized = true;
		}
	}

	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
}