#include "pch.h"

void njTranslateY(float y) {
	njTranslate(nullptr, 0, y, 0);
}

void njRotateX_(Angle x) {
	if (x) {
		njRotateX(_nj_current_matrix_ptr_, x);
	}
}

void njRotateY_(Angle y) {
	if (y) {
		njRotateY(_nj_current_matrix_ptr_, y);
	}
}

void njRotateZ_(Angle z) {
	if (z) {
		njRotateZ(_nj_current_matrix_ptr_, z);
	}
}

void LoadAnimationFile(AnimationFile** info, const char* name) {
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