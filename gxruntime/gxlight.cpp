#include "std.h"
#include "gxlight.h"
#include "gxscene.h"
#include "gxgraphics.h"
#include <cmath>

gxLight::gxLight(gxScene* s, int type) :
	scene(s) {

	memset(&d3d_light, 0, sizeof(d3d_light));

	switch (type) {
	case LIGHT_POINT:
		d3d_light.dltType = D3DLIGHT_POINT;
		break;
	case LIGHT_SPOT:
		d3d_light.dltType = D3DLIGHT_SPOT;
		break;
	default:
		d3d_light.dltType = D3DLIGHT_DIRECTIONAL;
	}

	d3d_light.dcvDiffuse.a = 1.0f;
	d3d_light.dcvDiffuse.r = 1.0f;
	d3d_light.dcvDiffuse.g = 1.0f;
	d3d_light.dcvDiffuse.b = 1.0f;
	d3d_light.dcvSpecular.r = 1.0f;
	d3d_light.dcvSpecular.g = 1.0f;
	d3d_light.dcvSpecular.b = 1.0f;
	d3d_light.dvTheta = 0.0f;
	d3d_light.dvPhi = 1.57079632679f;
	d3d_light.dvFalloff = 1.0f;
	d3d_light.dvDirection.x = 0.0f;
	d3d_light.dvDirection.y = 0.0f;
	d3d_light.dvDirection.z = 1.0f;

	setRange(1000.0f);
}

gxLight::~gxLight() {
}

void gxLight::setRange(float r) {
	if (r < 0.00001f) {
		r = 0.00001f;
	}
	d3d_light.dvRange = r;
	d3d_light.dvAttenuation0 = 1.0f;
	d3d_light.dvAttenuation1 = 0.0f;
	d3d_light.dvAttenuation2 = 1.0f / (r * r);
}

void gxLight::setPosition(const float pos[3]) {
	d3d_light.dvPosition.x = pos[0];
	d3d_light.dvPosition.y = pos[1];
	d3d_light.dvPosition.z = pos[2];
}

void gxLight::setDirection(const float dir[3]) {
	float len = std::sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
	if (len > 0.00001f) {
		d3d_light.dvDirection.x = dir[0] / len;
		d3d_light.dvDirection.y = dir[1] / len;
		d3d_light.dvDirection.z = dir[2] / len;
	}
	else {
		d3d_light.dvDirection.x = 0.0f;
		d3d_light.dvDirection.y = 0.0f;
		d3d_light.dvDirection.z = 1.0f;
	}
}

void gxLight::setConeAngles(float inner, float outer) {
	d3d_light.dvTheta = inner;
	d3d_light.dvPhi = outer;
}
