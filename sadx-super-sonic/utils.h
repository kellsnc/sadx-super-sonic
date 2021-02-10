#pragma once

extern HelperFunctions HelperFunctionsGlobal;

inline void LoadAnimationFile(AnimationFile** info, const char* name) {
	PrintDebug("[Super Sonic] Loading animation: %s... ", name);

	std::string fullPath = "system\\";
	fullPath = fullPath + name + ".saanim";

	AnimationFile* anm = new AnimationFile(HelperFunctionsGlobal.GetReplaceablePath(fullPath.c_str()));

	if (anm->getmodelcount() == 0) {
		PrintDebug("Failed!\n");
		delete anm;
		*info = nullptr;
	}
	else {
		PrintDebug("Done.\n");
		*info = anm;
	}
}