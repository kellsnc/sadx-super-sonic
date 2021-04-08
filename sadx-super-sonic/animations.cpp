#include "pch.h"

FastcallFunctionPointer(void, SuperSonic_WalkAni, (CharObj2* co2, EntityData2* data2), 0x491820);

Trampoline* Sonic_WalkAni_t = nullptr;
Trampoline* Sonic_GroundAnim_t = nullptr;
Trampoline* njAction_t = nullptr;
Trampoline* njAction_Queue_t = nullptr;

NJS_ACTION new_action;
AnimationFile* customAnims[4] = {};

extern bool DrawSuperSonic;

NJS_MOTION* SuperSonicMotionFixes(NJS_MOTION* motion) {
	if (motion == SONIC_ACTIONS[11]->motion) {
		return customAnims[0]->getmotion(); // Landing
	}
	else if (motion == SONIC_ACTIONS[51]->motion) {
		return customAnims[1]->getmotion(); // Monkey climb
	}
	else if (motion == SONIC_ACTIONS[52]->motion) {
		return customAnims[3]->getmotion(); // Monkey climb 2
	}
	else if (motion == SONIC_ACTIONS[53]->motion) {
		return customAnims[2]->getmotion(); // Monkey climb 3
	}
	else {
		return motion;
	}
}

NJS_OBJECT* SonicObjectToSuperSonic(NJS_OBJECT* object) {
	if (object == SONIC_OBJECTS[0]) {
		return SONIC_OBJECTS[22];
	}
	else if (object == SONIC_OBJECTS[56] || object == SONIC_OBJECTS[70] || object == SONIC_OBJECTS[66]) {
		return SONIC_ACTIONS[142]->object;
	}
	else {
		return object;
	}
}

NJS_ACTION* njAction_Hack(NJS_ACTION* action) {
	if (DrawSuperSonic == true) {
		njSetTexture(&SUPERSONIC_TEXLIST);

		new_action = { SonicObjectToSuperSonic(action->object), SuperSonicMotionFixes(action->motion) };

		DrawSuperSonic = false;

		return &new_action;
	}
	else {
		return action;
	}
}

void __cdecl njAction_r(NJS_ACTION* action, Float frame) {
	TARGET_DYNAMIC(njAction)(njAction_Hack(action), frame);
}

void __cdecl njAction_Queue_r(NJS_ACTION* action, float frame, QueuedModelFlagsB flags) {
	TARGET_DYNAMIC(njAction_Queue)(njAction_Hack(action), frame, flags);
}

void __cdecl Sonic_WalkAni_r(EntityData1* data, EntityData2* data2, CharObj2* co2) {
	if (co2->Upgrades & Upgrades_SuperSonic) {
		SuperSonic_WalkAni(co2, data2);
	}
	else {
		const auto original = Sonic_WalkAni_t->Target();

		__asm {
			mov eax, co2
			mov edi, data2
			mov ecx, data
			
			call original
		}
	}
}

static void __declspec(naked) Sonic_WalkAni_asm() {
	__asm {
		push eax
		push edi
		push ecx
		call Sonic_WalkAni_r
		pop ecx
		pop edi
		pop eax
		retn
	}
}

void __cdecl Sonic_GroundAnim_r(EntityData1* data, CharObj2* co2) {
	const auto original = Sonic_GroundAnim_t->Target();

	__asm {
		mov esi, co2
		mov ebx, data

		call original
	}

	if (co2->Upgrades & Upgrades_SuperSonic) {
		co2->AnimationThing.Index = 134;
	}
}

static void __declspec(naked) Sonic_GroundAnim_asm() {
	__asm {
		push esi
		push ebx
		call Sonic_GroundAnim_r
		pop ebx
		pop esi
		retn
	}
}

void Animations_Init() {
	if (ExtendedGamePlay == true) {
		Sonic_WalkAni_t = new Trampoline(0x495CD0, 0x495CD5, Sonic_WalkAni_asm);
		Sonic_GroundAnim_t = new Trampoline(0x491660, 0x49166B, Sonic_GroundAnim_asm);
		njAction_t = new Trampoline((int)njAction, (int)njAction + 0x9, njAction_r);
		njAction_Queue_t = new Trampoline((int)njAction_Queue, (int)njAction_Queue + 0x8, njAction_Queue_r);

		if (CustomAnims == true) {
			LoadAnimationFile(&customAnims[0], "SS_011");
			LoadAnimationFile(&customAnims[1], "SS_051");
			LoadAnimationFile(&customAnims[2], "SS_053");
			LoadAnimationFile(&customAnims[3], "SS_052");
		}
	}
}