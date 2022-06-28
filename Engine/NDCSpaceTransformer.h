#pragma once
#include "Vec3.h"
#include "Graphics.h"

class NDCSpaceTransformer {
public:
	NDCSpaceTransformer() :
		xFactor(float(Graphics::ScreenWidth) / 2.0f),
		yFactor(float(Graphics::ScreenHeight) / 2.0f) {

	}

	Vec3& Transform(Vec3& v)const {
		v.x = (v.x + 1.f)  * xFactor;
		v.y = (-v.y + 1.f) * yFactor;

		return v;
	}
	
	Vec3 GetTransformed(const Vec3& v)const {
		return Transform(Vec3(v));
	}

private:
	float xFactor;
	float yFactor;

};