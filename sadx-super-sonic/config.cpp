#include "pch.h"
#include "SADXModLoader.h"
#include "config.h"

Buttons TransformButton = Buttons_B;
AuraOptions DisableAura = AuraOptions::False;
bool EnableTransformationVoice = true;
bool EnableTikalUnusedVoice = true;
bool RemoveLimitations = false;
bool DetransformButton = true;
bool AlwaysSuperSonic = false;
bool ExtendedGamePlay = true;
bool CustomPhysics = true;
bool ChangeMusic = true;
bool CustomAnims = true;
bool EyeTracking = true;
bool WalkOnWater = true;
bool HUDIcons = true;

bool PatchPhysics = true;
bool PatchWater = false;

static const Buttons ButtonsList[]
{
	Buttons_B,
	Buttons_X,
	Buttons_Y,
	Buttons_C,
	Buttons_D,
	Buttons_Z
};

void ReadConfig(const char* path)
{
	const IniFile* config = new IniFile(std::string(path) + "\\config.ini");

	TransformButton = ButtonsList[config->getInt("General", "TransformButton", 0)];
	EnableTransformationVoice = config->getBool("Sounds", "EnableTransformationVoice", true);
	EnableTikalUnusedVoice = config->getBool("Sounds", "EnableTikalUnusedVoice", true);
	DetransformButton = config->getBool("General", "DetransformButton", true);
	AlwaysSuperSonic = config->getBool("General", "AlwaysSuperSonic", false);
	ExtendedGamePlay = config->getBool("General", "ExtendedGameplay", true);
	CustomPhysics = config->getBool("General", "CustomPhysics", true);
	CustomAnims = config->getBool("General", "CustomAnims", true);
	ChangeMusic = config->getBool("Sounds", "ChangeMusic", true);
	EyeTracking = config->getBool("General", "EyeTracking", true);
	WalkOnWater = config->getBool("General", "WalkOnWater", true);
	HUDIcons = config->getBool("General", "HUDIcons", true);

	PatchPhysics = config->getBool("Patches", "Physics", true);
	PatchWater = config->getBool("Patches", "Water", true);

	std::string aurastr = config->getString("General", "DisableAura", "False");

	if (!aurastr.compare("True"))
	{
		DisableAura = AuraOptions::True;
	}
	else if (!aurastr.compare("ExceptBoss"))
	{
		DisableAura = AuraOptions::ExceptBoss;
	}

	if (AlwaysSuperSonic == true || config->getBool("General", "RemoveLimitations", false))
	{
		RemoveLimitations = true;
	}

	delete config;
}
