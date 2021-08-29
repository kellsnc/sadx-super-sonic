#pragma once

#define TARGET_DYNAMIC(name) ((decltype(name##_r)*)name##_t->Target())

extern HelperFunctions HelperFunctionsGlobal;

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
extern bool HUDIcons;

void SuperSonic_Init();
void Objects_Init();
void WaterHack_Init();
void Physics_Init(const char* path);
void Animations_Init();
void SSTikalHint_Init();
void HudInit();

bool IsPerfectChaosLevel();
bool IsSuperSonic(CharObj2* co2);
bool IsStoryFinished();
void DetransformSuperSonic(EntityData1* data, CharObj2* co2);
bool UseAdvancedSuperSonic();
void SuperSonic_HackDisplay(CharObj2* co2);
void RestoreMusic();
void TransformMusicAndSound();
void DetransformMusicAndSound();
void SuperSonic_Actions(EntityData1* data, motionwk* mwp, CharObj2* co2);
void SetSuperAnims(CharObj2* co2);
void UnsetSuperAnims(CharObj2* co2);
void InitSuperSonicAnims();
void InitSuperSonicEyes(int player);
void LoadIconTextures();