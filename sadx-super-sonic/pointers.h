#pragma once

#define TARGET_DYNAMIC(name) ((decltype(name##_r)*)name##_t->Target())

static constexpr int MaxPlayers = 4;

FunctionPointer(void, DrawObject, (NJS_OBJECT*), 0x408530);
FunctionPointer(Bool, IsEventPerforming, (), 0x42FB00);
ObjectFunc(UpdateSetDataAndDelete, 0x46C150);

enum CollisionShapes {
    CollisionShape_Sphere,
    CollisionShape_Cylinder,
    CollisionShape_PushCylinder,
    CollisionShape_Cube,
    CollisionShape_Cube2,
    CollisionShape_Capsule,
    CollisionShape_Capsule2
};