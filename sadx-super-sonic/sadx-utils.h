#pragma once

static constexpr int MaxPlayers = 4;

FunctionPointer(void, DrawCharacterShadow, (EntityData1* a1, struct_a3* a2), 0x49F0B0);
FunctionPointer(Bool, IsEventPerforming, (), 0x42FB00);
ObjectFunc(SonicChargeSpindashEffect, 0x4A2A10);
FunctionPointer(float, GetGroundYPosition, (float x, float y, float z, Rotation3* rotation), 0x49E920);
FunctionPointer(void, SetPlayerEyeTarget, (unsigned __int8 player, ObjectMaster* obj, NJS_VECTOR* pos), 0x440FC0);
FunctionPointer(void, SetHintChildData, (ObjectMaster* obj, float, float), 0x4A3490);
FunctionPointer(void, SetHintPos, (NJS_VECTOR*, NJS_VECTOR*, float), 0x4A34B0);
FunctionPointer(void, StopPlayerEyeTarget, (unsigned __int8 player), 0x441080);
FunctionPointer(void, PlaySound3D, (int ID, void* entity, int a3, int volume, int length, EntityData1* data), 0x424880);
FunctionPointer(ObjectMaster*, LoadTikalChild, (ObjectMaster* obj, char*, NJS_VECTOR*), 0x4A3B70);
FunctionPointer(void, StopPlayerLookAt, (uint8_t id), 0x441080);
FastcallFunctionPointer(void, SuperSonic_WalkAni, (CharObj2* co2, EntityData2* data2), 0x491820);
DataArray(CollisionData, JumpPanel_Collision_, 0x97DF68, 4);
DataPointer(AnimData*, pSonicAnimData, 0x49AB4D);

// Symbols:
#define TWP_PNUM(twp) twp->counter.b[0]
FunctionPointer(void, PSetDashEffect, (taskwk* twp), 0x494050);
FunctionPointer(void, PSetSpinDSEffect, (taskwk* twp), 0x4940B0);
TaskFunc(EffectSpin, 0x4A2A10);
TaskFunc(EffectSpinDS, 0x4A2A40);
TaskFunc(EffectDash, 0x4A2AA0);
TaskFunc(EffectSuperSonicAura, 0x55FAF0);
TaskFunc(EffectSSSpinDS, 0x55FB20);
TaskFunc(EffectSSDash, 0x55FB80);
TaskFunc(EffectSpinRibbon, 0x4A1E90);
TaskFunc(EffectRibbon, 0x4A1BE0);
TaskFunc(EffectSpinRibbonDisplay, 0x4A1DB0);

//signed int __usercall Sonic_HoldingObject_NAct@<eax>(EntityData1* data@<ecx>, int a2@<edx>, CharObj2* co2@<eax>, void* data2)
static const void* const Sonic_HoldingObject_NActPtr = (void*)0x496880;
static inline signed int Sonic_HoldingObject_NAct(EntityData1* data, CharObj2* co2, EntityData2* data2)
{
	signed int result;
	__asm
	{
		push[data2]
		mov eax, [co2]
		mov ecx, [data]
		call Sonic_HoldingObject_NActPtr
		add esp, 4
		mov result, eax
	}
	return result;
}

enum CollisionShapes {
    CollisionShape_Sphere,
    CollisionShape_Cylinder,
    CollisionShape_PushCylinder,
    CollisionShape_Cube,
    CollisionShape_Cube2,
    CollisionShape_Capsule,
    CollisionShape_Capsule2
};