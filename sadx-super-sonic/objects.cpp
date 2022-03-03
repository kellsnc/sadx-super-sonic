#include "pch.h"

// Code to adapt some tasks to Super Sonic

Trampoline* SonicChargeBodyDisplay_t = nullptr;
Trampoline* EffectSuperSonicAura_t   = nullptr;
Trampoline* PSetSpinDSEffect_t       = nullptr;
Trampoline* PSetDashEffect_t         = nullptr;
Trampoline* EffectSpin_t             = nullptr; // PSetSpinEffect is inlined

static void __cdecl SonicChargeBodyDisplay_r(taskwk* twp, motionwk2* mwp, playerwk* pwp)
{
	if (!IsSuperSonic((CharObj2*)pwp))
	{
		TARGET_DYNAMIC(SonicChargeBodyDisplay)(twp, mwp, pwp);
	}
}

static void __cdecl EffectSuperSonicAura_r(task* tp)
{
	// Disable in menus or option enabled
	if (GameState == 21 || !(DisableAura == AuraOptions::False || (DisableAura == AuraOptions::ExceptBoss && IsPerfectChaosLevel())))
	{
		tp->exec = FreeTask;
		return;
	}

	TARGET_DYNAMIC(EffectSuperSonicAura)(tp);
}

static void __cdecl PSetSpinDSEffect_r(taskwk* twp)
{
	auto pnum = TWP_PNUM(twp);
	auto pwp = playerpwp[pnum];

	if (IsSuperSonic((CharObj2*)pwp))
	{
		auto tp = CreateElementalTask(2u, LEV_6, EffectSSSpinDS);
		if (tp)
			TWP_PNUM(tp->twp) = pnum;
	}
	else
	{
		TARGET_DYNAMIC(PSetSpinDSEffect)(twp);
	}
}

static void __cdecl PSetDashEffect_r(taskwk* twp)
{
	auto pnum = TWP_PNUM(twp);
	auto pwp = playerpwp[pnum];

	if (IsSuperSonic((CharObj2*)pwp))
	{
		auto tp = CreateElementalTask(2u, LEV_6, EffectSSDash);
		if (tp)
			TWP_PNUM(tp->twp) = pnum;
	}
	else
	{
		TARGET_DYNAMIC(PSetDashEffect)(twp);
	}
}

static void __cdecl EffectSpin_r(task* tp)
{
	auto pwp = playerpwp[TWP_PNUM(tp->twp)];

	if (!IsSuperSonic((CharObj2*)pwp))
	{
		TARGET_DYNAMIC(EffectSpin)(tp);
	}
}

void Objects_Init()
{
	if (ExtendedGamePlay == true)
	{
		PSetDashEffect_t         = new Trampoline(0x494050, 0x494055, PSetDashEffect_r);
		PSetSpinDSEffect_t       = new Trampoline(0x4940B0, 0x4940B5, PSetSpinDSEffect_r);
		EffectSpin_t             = new Trampoline(0x4A2A10, 0x4A2A15, EffectSpin_r);

		SonicChargeBodyDisplay_t = new Trampoline(0x4A1630, 0x4A1635, SonicChargeBodyDisplay_r);
		WriteCall((void*)0x494B27, SonicChargeBodyDisplay_r); // DC Conversion compatibility
	}

	EffectSuperSonicAura_t = new Trampoline(0x55FAF0, 0x55FAF5, EffectSuperSonicAura_r);

	//Fix Jump Panel collision placement
	JumpPanel_Collision_[1].center.y = 2.0f;
	JumpPanel_Collision_[2].center.y = 2.0f;
	JumpPanel_Collision_[3].center.y = 2.0f;
	JumpPanel_Collision_[3].a = 7.0f;
}