#include <forward_list>
#include "../../include/onions/matrix.h"

namespace onion
{

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
			g_MatrixStack.push_front(g_MatrixStack.front());
		}
	}

	void mat_pop()
	{
		g_MatrixStack.pop_front();
	}


	void mat_identity()
	{
		g_MatrixStack.front() = mat4x4f();
	}

	void mat_custom_transform(const mat4x4f& transform)
	{
		g_MatrixStack.front() *= transform;
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
		trans.set(0, 3, dx);
		trans.set(1, 3, dy);
		trans.set(2, 3, dz);

		mat_custom_transform(trans);
	}

	void mat_scale(float sx, float sy, float sz)
	{
		mat4x4f& top = mat_get();
		for (int k = 2; k >= 0; --k)
		{
			top(k, 0) *= sx;
			top(k, 1) *= sy;
			top(k, 2) *= sz;
		}
	}

	void mat_rotatex(float angle)
	{
		float s = sinf(angle);
		float c = cosf(angle);

		mat4x4f rot;
		rot.set(1, 1, c);
		rot.set(1, 2, -s);
		rot.set(2, 1, s);
		rot.set(2, 2, c);

		mat_custom_transform(rot);
	}

	void mat_rotatey(float angle)
	{
		float s = sinf(angle);
		float c = cosf(angle);

		mat4x4f rot;
		rot.set(0, 0, c);
		rot.set(0, 2, s);
		rot.set(2, 0, -s);
		rot.set(2, 2, c);

		mat_custom_transform(rot);
	}

	void mat_rotatez(float angle)
	{
		float s = sinf(angle);
		float c = cosf(angle);

		mat4x4f rot;
		rot.set(0, 0, c);
		rot.set(0, 1, -s);
		rot.set(1, 0, s);
		rot.set(1, 1, c);

		mat_custom_transform(rot);
	}

}