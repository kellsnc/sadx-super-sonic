#include "pch.h"

void DrawObject_Queue(NJS_OBJECT* object, QueuedModelFlagsB flags) {
	njPushMatrixEx();
	njTranslate(nullptr, Pos3(object->pos));
	njRotateXYZ(nullptr, Pos3(object->ang));
	DrawModel_Queue(object->basicdxmodel, QueuedModelFlagsB_EnableZWrite);
	njPopMatrixEx();
}

void DrawObjectRoot(NJS_OBJECT* object) {
	njPushMatrixEx();
	njTranslate(nullptr, Pos3(object->pos));
	njRotateXYZ(nullptr, Pos3(object->ang));
	njScale(nullptr, Pos3(object->scl));
	njDrawModel_SADX(object->basicdxmodel);
	njPopMatrixEx();
}

void njTranslateX(float f) {
	njTranslate(nullptr, f, 0, 0);
}

void njTranslateY(float f) {
	njTranslate(nullptr, 0, f, 0);
}

void njTranslateZ(float f) {
	njTranslate(nullptr, 0, 0, f);
}

void njRotateZXY(Rotation3* rot) {
	njRotateZ(0, rot->z);
	njRotateX(0, rot->x);
	njRotateY(0, rot->y);
}

void njRotateYXZ(Rotation3* rot) {
	njRotateY(0, rot->y);
	njRotateX(0, rot->x);
	njRotateZ(0, rot->z);
}

void njScaleX(float f) {
	njScale(nullptr, f, 1.0f, 1.0f);
}

void njScaleY(float f) {
	njScale(nullptr, 1.0f, f, 1.0f);
}

void njScaleZ(float f) {
	njScale(nullptr, 1.0f, 1.0f, f);
}

void njScalef(float f) {
	njScale(nullptr, f, f, f);
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