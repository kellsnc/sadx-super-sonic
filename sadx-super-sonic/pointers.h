#pragma once

#define TARGET_DYNAMIC(name) ((decltype(name##_r)*)name##_t->Target())

static constexpr int MaxPlayers = 4;

FunctionPointer(void, DrawObject, (NJS_OBJECT*), 0x408530);
FunctionPointer(Bool, IsEventPerforming, (), 0x42FB00);
ObjectFunc(SonicChargeSpindashEffect, 0x4A2A10);

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