# sadx-super-sonic
Improved super sonic mod, based on SonicFreak94's [original mod](https://github.com/sonicretro/sadx-super-sonic).

## Features

* Transform into Super Sonic anywhere (the 50 rings & story completion are optional)
* All of Sonic's actions!
* Transformation in the character's code directly, allowing much better compatibility with Character Select.
* If you've completed Super Sonic story, an unused hint from Tikal will appear at stage starts in adventure mode (optional)
* Adds eye tracking to Super Sonic thanks to ItsEasyActually. (optional)
* Adds life & 1up icons for Super Sonic. (optional, can be replaced by mods)
* Patches many vanilla Super Sonic issues like springs, jump panels, downward slopes, etc.
* Option to change the transformation button.
* Optional custom physics to stop you flying over the whole level every time you jump.
* Option to remove the 50 rings & completed story limitations.
* Option to remove the music change or grunt transformation voices.
* Option to always be Super Sonic.
* Option to change the transformation button and to disable the detransform button.
* Option to disable the aura (permanently or only in levels/fields).

## Install

1. Download the mod from the [GitHub releases page](https://github.com/kellsnc/sadx-super-sonic/releases) or from the [GameBanana mod page](https://gamebanana.com/mods/49986).
2. Extract the folder contained in the archive into the "mods" folder at the root of your game. <br> If you do not have a "mods" folder, download the [mod loader](https://github.com/x-hax/sadx-mod-loader).
3. Launch the Mod Manager and enable "Super Sonic".
4. (optional) Use the "configure" button in the Mod Manager to tweak the mod's configuration.

## How tu use

As Sonic, jump then press the action button to transform into Super Sonic. The action button is configurable.

If the "Remove Limitations" option is disabled, you will need to have completed Super Sonic's story and to collect 50 rings.

## Configure

The Mod Manager provides an easy to use configuration form, but you can write the config.ini yourself. Here is an exemple with all options at their default value.

```
[General]
ExtendedGameplay=True ; Whether Super Sonic can perform Sonic's actions.
RemoveLimitations=False ; Removes the completed story and 50 rings limitations.
CustomPhysics=True ; Use nerfed physics value.
AlwaysSuperSonic=False
TransformButton=0 ; 0 = B, 1 = X, 2 = Y, 3 = C, 4 = D, 5 = Z
EyeTracking=True ; Super Sonic head can turn toward entities.
HUDIcons=True ; Replace Sonic life icon with a custom Super Sonic one upon transformation.
CustomAnims=True ; Use custom animations to fix some buggy animations.
DisableAura=False ; Expects "False", "True" or "ExceptBoss" (to keep the aura during Perfect Chaos.)
[Sounds]
ChangeMusic=True
EnableTransformationVoice=True
EnableTikalUnusedVoice=True ; Whether a Tikal orb playing the unused Super Sonic line should appear
```

## For mod developers
* You can replace the life & 1up icon from your mod by replacing the SUPERSONIC_EXTRA texture archive.
* The texlist for the SUPERSONIC_EXTRA texture archive is exported by the dll as "SUPERSONIC_EXTRA_TEXLIST"
