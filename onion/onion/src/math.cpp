#include <forward_list>

#include "../include/math.h"

mat4x4f::mat4x4f() {}

mat4x4f::mat4x4f(float a11, float a12, float a13, float a14,
	float a21, float a22, float a23, float a24,
	float a31, float a32, float a33, float a34)
{
	get(0, 0) = a11;
	get(0, 1) = a12;
	get(0, 2) = a13;
	get(0, 3) = a14;

	get(1, 0) = a21;
	get(1, 1) = a22;
	get(1, 2) = a23;
	get(1, 3) = a24;

	get(2, 0) = a31;
	get(2, 1) = a32;
	get(2, 2) = a33;
	get(2, 3) = a34;
}


// The matrix stack.
std::forward_list<mat4x4f> g_MatrixStack;

mat4x4f& mat_get()
{
	if (g_MatrixStack.empty())
	{
		g_MatrixStack.emplace_front();
	}

	return g_MatrixStack.front();
}

const float* mat_get_values()
{
	return mat_get().matrix_values();
}


void mat_push()
{
	if (g_MatrixStack.empty())
	{
		g_MatrixStack.emplace_front();
	}
	else
	{
		mat4x4f top = g_MatrixStack.front();
		g_MatrixStack.push_front(top);
	}
}

void mat_pop()
{
	g_MatrixStack.pop_front();
}


void mat_custom_transform(mat4x4f& transform)
{
	mat4x4f top = mat_get();
	mat_mul(top, transform, mat_get());
}

void mat_ortho(float left, float right, float bottom, float top, float near, float far)
{
	g_MatrixStack.clear();

	mat4x4f mat;
	mat.set(0, 0, 2.f / (right - left));
	mat.set(1, 1, 2.f / (top - bottom));
	mat.set(2, 2, 2.f / (far - near));
	mat.set(0, 3, (left + right) / (left - right));
	mat.set(1, 3, (bottom + top) / (bottom - top));
	mat.set(2, 3, (near + far) / (near - far));

	g_MatrixStack.push_front(mat);
}

void mat_translate(float dx, float dy, float dz)
{
	mat4x4f trans;
	trans.get(0, 3) = dx;
	trans.get(1, 3) = dy;
	trans.get(2, 3) = dz;

	mat_custom_transform(trans);
}

void mat_scale(float sx, float sy, float sz)
{
	mat4x4f& top = mat_get();
	for (int k = 3; k >= 0; --k)
	{
		top.get(0, k) *= sx;
		top.get(1, k) *= sy;
		top.get(2, k) *= sz;
	}
}

void mat_rotatex(float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	mat4x4f rot;
	rot.get(1, 1) = c;
	rot.get(1, 2) = -s;
	rot.get(2, 1) = s;
	rot.get(2, 2) = c;

	mat_custom_transform(rot);
}

void mat_rotatey(float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	mat4x4f rot;
	rot.get(0, 0) = c;
	rot.get(0, 2) = s;
	rot.get(2, 0) = -s;
	rot.get(2, 2) = c;

	mat_custom_transform(rot);
}

void mat_rotatez(float angle)
{
	float s = sinf(angle);
	float c = cosf(angle);

	mat4x4f rot;
	rot.get(0, 0) = c;
	rot.get(0, 1) = -s;
	rot.get(1, 0) = s;
	rot.get(1, 1) = c;

	mat_custom_transform(rot);
}

void mat_rotatex_scaley(float angle, float sy)
{
	float s = sinf(angle);
	float c = cosf(angle);

	mat4x4f rot;
	rot.get(1, 1) = c * sy;
	rot.get(1, 2) = -s;
	rot.get(2, 1) = s * sy;
	rot.get(2, 2) = c;

	mat_custom_transform(rot);
}