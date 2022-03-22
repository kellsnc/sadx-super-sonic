#pragma once

#define TARGET_DYNAMIC(name) ((decltype(name##_r)*)name##_t->Target())

extern HelperFunctions HelperFunctionsGlobal;
extern NJS_TEXLIST SUPERSONIC_EXTRA_TEXLIST;

void SuperSonic_Init();
void Objects_Init();
void WaterHack_Init();
void Physics_Init(const char* path);
void Animations_Init();
void SSTikalHint_Init();
void HudInit();

bool IsPerfectChaosLevel();
bool IsSuperSonic(CharObj2* co2);
bool IsSuperSonic(int pnum);
bool IsStoryFinished();
bool UseAdvancedSuperSonic();
void RestoreMusic();
void RunSuperMusic();
void TransformMusicAndSound();
void DetransformMusicAndSound();
void SuperSonic_Actions(EntityData1* data, motionwk* mwp, CharObj2* co2);
void SetSuperAnims(CharObj2* co2);
void UnsetSuperAnims(CharObj2* co2);
void InitSuperSonicEyes(int player);