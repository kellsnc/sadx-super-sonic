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