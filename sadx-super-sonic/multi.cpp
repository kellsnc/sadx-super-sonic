#include "pch.h"
#include "SADXModLoader.h"
#include "multiapi.h"
#include "multi.h"

HMODULE HMULTI = NULL;
bool check = false;

bool CheckAPI()
{
	if (!check)
	{
		HMULTI = GetModuleHandle(L"sadx-multiplayer");
		check = true;
	}
	return HMULTI != NULL;
}

void AddNumRingM(Sint32 pnum, Sint16 amount)
{
	if (CheckAPI() && multi_is_battle())
		multi_rings_add(pnum, amount);
	else
		AddNumRing(amount);
}

Sint16 GetNumRingM(Sint32 pnum)
{
	if (CheckAPI() && multi_is_battle())
		return multi_rings_get(pnum);
	else
		return GetNumRing();
}
