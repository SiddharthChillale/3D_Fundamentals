#pragma once
#include "Vec2.h"

class MouseTracker {
public:
	void Engage(const Vei2& pos) {
		base = pos;
		engaged = true;
	}
	void Release() {
		engaged = false;
	}
	bool isEngaged()const {
		return engaged;
	}
	Vei2 move(const Vei2& pos) {
		const auto delta = pos - base;
		base = pos;
		return delta;
	}
private:
	Vei2 base = {0, 0};
	bool engaged = false;
};