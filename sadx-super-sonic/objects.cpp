#include "pch.h"
#include "SADXModLoader.h"
#include "FunctionHook.h"
#include "config.h"

// Code to adapt some tasks to Super Sonic

FunctionHook<void, taskwk*, motionwk2*, playerwk*> SonicChargeBodyDisplay_h(0x4A1630);
TaskHook EffectSuperSonicAura_h(0x55FAF0);
FunctionHook<void, taskwk*> PSetSpinDSEffect_h(0x4940B0);
FunctionHook<void, taskwk*> PSetDashEffect_h(0x494050);
TaskHook EffectSpin_h(0x4A2A10); // PSetSpinEffect is inlined
TaskHook EffectSpinRibbonDisplay_h(0x4A1DB0);
TaskHook EffectDrawRibbon_h(0x4A17B0);

static void __cdecl SonicChargeBodyDisplay_r(taskwk* twp, motionwk2* mwp, playerwk* pwp)
{
	if (!IsSuperSonic(pwp))
	{
		SonicChargeBodyDisplay_h.Original(twp, mwp, pwp);
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

	EffectSuperSonicAura_h.Original(tp);
}

static void __cdecl PSetSpinDSEffect_r(taskwk* twp)
{
	auto pnum = TWP_PNUM(twp);
	auto pwp = playerpwp[pnum];

	if (IsSuperSonic(pwp))
	{
		auto tp = CreateElementalTask(2u, LEV_6, EffectSSSpinDS);
		if (tp)
			TWP_PNUM(tp->twp) = pnum;
	}
	else
	{
		PSetSpinDSEffect_h.Original(twp);
	}
}

static void __cdecl PSetDashEffect_r(taskwk* twp)
{
	auto pnum = TWP_PNUM(twp);
	auto pwp = playerpwp[pnum];

	if (IsSuperSonic(pwp))
	{
		auto tp = CreateElementalTask(2u, LEV_6, EffectSSDash);
		if (tp)
			TWP_PNUM(tp->twp) = pnum;
	}
	else
	{
		PSetDashEffect_h.Original(twp);
	}
}

static void __cdecl EffectSpin_r(task* tp)
{
	auto pwp = playerpwp[TWP_PNUM(tp->twp)];

	if (!IsSuperSonic(pwp))
	{
		EffectSpin_h.Original(tp);
	}
}

static NJS_TEXANIM anim_hitodama_ss[]{
	{ 0x40, 0x40, 0x20, 0x20, 0, 0, 0xFF, 0xFF, 3, 0 },
	{ 0x40, 0x40, 0x20, 0x20, 0, 0, 0xFF, 0xFF, 5, 0 },
};

static NJS_SPRITE sprite_hitodama_ss{ {}, 0.2f, 0.2f, 0, &SUPERSONIC_EXTRA_TEXLIST, anim_hitodama_ss };

static void __cdecl EffectSpinRibbonDisplay_r(task* tp)
{
	auto twp = tp->twp;

	if (IsSuperSonic(twp->counter.b[0]))
	{
		if (!loop_count)
		{
			auto twp = tp->twp;

			njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
			njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
			njPushMatrixEx();
			njTranslateEx(&twp->pos);

			auto cam_twp = camera_twp;
			if (cam_twp->ang.y)
				njRotateY(0, cam_twp->ang.y);
			if (cam_twp->ang.x)
				njRotateX(0, cam_twp->ang.x);

			auto scl = twp->timer.l * 0.03125f;
			njScale(0, scl, scl, scl);

			___njFogDisable();
			late_z_ofs___ = 1000.0f;
			njDrawSprite3D_Queue(&sprite_hitodama_ss, twp->btimer, 0x20u, (QueuedModelFlagsB)4);
			late_z_ofs___ = 0.0f;
			___njFogEnable();

			njPopMatrixEx();
			njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
			njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
		}
	}
	else
	{
		EffectSpinRibbonDisplay_h.Original(tp);
	}
}

static void __cdecl EffectDrawRibbon_r(task* tp)
{
	if (IsSuperSonic(tp->twp->smode))
	{
		auto p3col = (NJS_POINT3COL*)tp->mwp;
		
		if (!MissedFrames && p3col->num >= 3)
		{
			njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
			njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
			njSetTexture(&SUPERSONIC_EXTRA_TEXLIST);
			njSetTextureNum(2 + tp->twp->btimer * 2);
			late_z_ofs___ = 1000.0f;
			Draw3DLinesMaybe_Queue(p3col, p3col->num - 2, NJD_TRANSPARENT | NJD_FILL | NJD_DRAW_CONNECTED | 0x80000000, QueuedModelFlagsB_SomeTextureThing);
			late_z_ofs___ = 0.0f;
			njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
			njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
		}
	}
	else
	{
		EffectDrawRibbon_h.Original(tp);
	}
}

static void __cdecl CreateEffectSpinRibbon_r(taskwk* ptwp, int count)
{
	auto ppwp = playerpwp[ptwp->counter.b[0]];
	auto type = ppwp->equipment & Upgrades_CrystalRing ? 1 : 0;

	auto spinribbon_tp = CreateElementalTask(2, 2, EffectSpinRibbon);
	if (spinribbon_tp)
	{
		auto spinribbon_twp = spinribbon_tp->twp;
		spinribbon_twp->counter.b[0] = ptwp->counter.b[0];

		auto cinfo = ptwp->cwp->info;
		spinribbon_twp->scl.x = cinfo->center.x;
		spinribbon_twp->scl.y = cinfo->center.y;
		spinribbon_twp->scl.z = cinfo->center.z;

		float dst = njRandom() * 15.0f + 25.0f;
		Angle ang1 = -2048 - (Angle)(njRandom() * -12288.0f);
		Angle ang2 = (Angle)(njRandom() * 65536.0f);
		
		spinribbon_twp->pos.x = njSin(ang2) * njCos(ang1) * dst + spinribbon_twp->scl.x;
		spinribbon_twp->pos.y = njSin(ang1) * dst + spinribbon_twp->scl.y;
		spinribbon_twp->pos.z = njCos(ang2) * njCos(ang1) * dst + spinribbon_twp->scl.z;

		spinribbon_twp->ang.y = 0x8000 - ptwp->ang.y;
		spinribbon_twp->timer.l = count + 1;
		spinribbon_twp->btimer = type;

		auto ribbon_tp = CreateElementalTask(2, 3, EffectRibbon);
		if (ribbon_tp)
		{
			auto ribbon_twp = ribbon_tp->twp;
			ribbon_twp->smode = ptwp->counter.b[0]; // <- rewrote everything just for that
			ribbon_twp->value.ptr = &spinribbon_tp->twp->pos;
			ribbon_twp->counter.l = count;
			ribbon_twp->timer.l = -1;
			ribbon_twp->pos = spinribbon_tp->twp->pos;
			ribbon_twp->btimer = type;

			CCL_Init(spinribbon_tp, (CCL_INFO*)0x927D18, 1, 1u);
			spinribbon_tp->disp = EffectSpinRibbonDisplay;
		}
		else
		{
			FreeTask(spinribbon_tp);
		}
	}
}

void Objects_Init()
{
	if (ExtendedGamePlay == true)
	{
		SonicChargeBodyDisplay_h.Hook(SonicChargeBodyDisplay_r);
		PSetDashEffect_h.Hook(PSetDashEffect_r);
		PSetSpinDSEffect_h.Hook(PSetSpinDSEffect_r);
		EffectSpin_h.Hook(EffectSpin_r);
		EffectSpinRibbonDisplay_h.Hook(EffectSpinRibbonDisplay_r);
		EffectDrawRibbon_h.Hook(EffectDrawRibbon_r);
		WriteJump((void*)0x4A2040, CreateEffectSpinRibbon_r);
	}

	EffectSuperSonicAura_h.Hook(EffectSuperSonicAura_r);

	//Fix Jump Panel collision placement
	JumpPanel_Collision_[1].center.y = 2.0f;
	JumpPanel_Collision_[2].center.y = 2.0f;
	JumpPanel_Collision_[3].center.y = 2.0f;
	JumpPanel_Collision_[3].a = 7.0f;
}
