#pragma once

#define TARGET_DYNAMIC(name) ((decltype(name##_r)*)name##_t->Target())

extern HelperFunctions HelperFunctionsGlobal;

extern Buttons TransformButton;
extern bool EnableTransformationVoice;
extern bool EnableTikalUnusedVoice;
extern bool RemoveLimitations;
extern bool DetransformButton;
extern bool AlwaysSuperSonic;
extern bool ExtendedGamePlay;
extern bool CustomPhysics;
extern bool ChangeMusic;
extern bool CustomAnims;

void SuperSonic_Init();
void Objects_Init();
void WaterHack_Init();
void Music_Init();
void Physics_Init(const char* path);
void Animations_Init();

void njAction_SuperSonic(NJS_ACTION* action, Float frame);
bool UseAdvancedSuperSonic();
void RestoreMusic();
void TransformMusicAndSound();
void DetransformMusicAndSound();
void SuperSonic_Display(EntityData1* data, CharObj2* co2);
void SuperSonic_Actions(EntityData1* data, motionwk* mwp, CharObj2* co2);