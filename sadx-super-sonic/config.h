#pragma once

void ReadConfig(const char* path);

enum class AuraOptions
{
	False,
	True,
	ExceptBoss
};

extern Buttons TransformButton;
extern AuraOptions DisableAura;
extern bool EnableTransformationVoice;
extern bool EnableTikalUnusedVoice;
extern bool RemoveLimitations;
extern bool DetransformButton;
extern bool AlwaysSuperSonic;
extern bool ExtendedGamePlay;
extern bool CustomPhysics;
extern bool ChangeMusic;
extern bool CustomAnims;
extern bool EyeTracking;
extern bool WalkOnWater;
extern bool HUDIcons;

extern bool PatchPhysics;
extern bool PatchWater;