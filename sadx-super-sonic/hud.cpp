#include "pch.h"
#include "SADXModLoader.h"
#include "config.h"

enum SUPERSONIC_EXTRA
{
	SUPERSONIC_EXTRA_LIFE,
	SUPERSONIC_EXTRA_1UP
};

static Uint32 backupLifeIcon = 0;
static Uint32 backupOneUpIcon = 0;
static const uint8_t SonicSlotConRegular = 12;
static const uint8_t SonicSlotObjRegular = 31;

void __cdecl BackupLifeIcon()
{
	backupLifeIcon = CON_REGULAR_TEXLIST.textures[SonicSlotConRegular].texaddr;
	backupOneUpIcon = OBJ_REGULAR_TEXLIST.textures[SonicSlotObjRegular].texaddr;
}

void UpdateSSIcons(bool toggle)
{
	if (HUDIcons == false)
		return;

	CON_REGULAR_TEXLIST.textures[SonicSlotConRegular].texaddr = toggle ? SUPERSONIC_EXTRA_TEXLIST.textures[SUPERSONIC_EXTRA_LIFE].texaddr : backupLifeIcon;
	OBJ_REGULAR_TEXLIST.textures[SonicSlotObjRegular].texaddr = toggle ? SUPERSONIC_EXTRA_TEXLIST.textures[SUPERSONIC_EXTRA_1UP].texaddr : backupOneUpIcon;
}