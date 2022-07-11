#pragma once

template<class Vertex>
class DefaultVertexShader {
public:
	typedef Vertex Output;
public:
	void BindRotation(const Mat3& rotation_in) {
		rotation = rotation_in;
	}
	void BindTranslation(const Vec3& translation_in) {
		translation = translation_in;
	}
	Output operator()(const Vertex& v_in)const {
		return { v_in.pos * rotation + translation, v_in };
	}

private:
	Mat3 rotation;
	Vec3 translation;
};