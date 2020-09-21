#include <forward_list>
#include "../../include/onions/matrix.h"

namespace onion
{

	MatrixStack::MatrixStack()
	{
		reset();
	}
	
	TRANSFORM_MATRIX& MatrixStack::get()
	{
		return m_Stack.top();
	}

	const TRANSFORM_MATRIX& MatrixStack::get() const
	{
		return m_Stack.top();
	}

	const float* MatrixStack::get_values() const
	{
		return m_Stack.top().matrix_values();
	}
	
	void MatrixStack::push()
	{
		if (m_Stack.empty())
			m_Stack.emplace();
		else
			m_Stack.push(get());
	}

	void MatrixStack::pop()
	{
		if (!m_Stack.empty())
			m_Stack.pop();
	}

	void MatrixStack::translate(float tx)
	{
		TRANSFORM_MATRIX& m = get();
		for (int k = 2; k >= 0; --k)
			m(k, 3) += (tx * m.get(k, 0));
	}

	void MatrixStack::translate(float tx, float ty)
	{
		TRANSFORM_MATRIX& m = get();
		for (int k = 2; k >= 0; --k)
			m(k, 3) += (tx * m.get(k, 0)) + (ty * m.get(k, 1));
	}
	
	void MatrixStack::translate(float tx, float ty, float tz)
	{
		TRANSFORM_MATRIX& m = get();
		for (int k = 2; k >= 0; --k)
			m(k, 3) += (tx * m.get(k, 0)) + (ty * m.get(k, 1)) + (tz * m.get(k, 2));
	}

	void MatrixStack::scale(float sx)
	{
		TRANSFORM_MATRIX& m = get();
		for (int k = 2; k >= 0; --k)
			m(k, 0) *= sx;
	}

	void MatrixStack::scale(float sx, float sy)
	{
		TRANSFORM_MATRIX& m = get();
		for (int k = 2; k >= 0; --k)
		{
			m(k, 0) *= sx;
			m(k, 1) *= sy;
		}
	}

	void MatrixStack::scale(float sx, float sy, float sz)
	{
		TRANSFORM_MATRIX& m = get();
		for (int k = 2; k >= 0; --k)
		{
			m(k, 0) *= sx;
			m(k, 1) *= sy;
			m(k, 2) *= sz;
		}
	}
	
	void MatrixStack::rotatex(float angle)
	{
		TRANSFORM_MATRIX& m = get();

		float s = sinf(angle);
		float c = cosf(angle);

		for (int k = 2; k >= 0; --k)
		{
			float& r1 = m(k, 1);
			float& r2 = m(k, 2);

			float a1 = (r2 * s) + (r1 * c);
			r2 = (r2 * c) - (r1 * s);
			r1 = a1;
		}
	}

	void MatrixStack::rotatey(float angle)
	{
		TRANSFORM_MATRIX& m = get();

		float s = sinf(angle);
		float c = cosf(angle);

		for (int k = 2; k >= 0; --k)
		{
			float& r0 = m(k, 0);
			float& r2 = m(k, 2);

			float a0 = (r0 * c) - (r2 * s);
			r2 = (r0 * s) + (r2 * c);
			r0 = a0;
		}
	}

	void MatrixStack::rotatez(float angle)
	{
		TRANSFORM_MATRIX& m = get();

		float s = sinf(angle);
		float c = cosf(angle);

		for (int k = 2; k >= 0; --k)
		{
			float& r0 = m(k, 0);
			float& r1 = m(k, 1);

			float a0 = (r1 * s) + (r0 * c);
			r1 = (r1 * c) - (r0 * s);
			r0 = a0;
		}
	}

	void MatrixStack::reset()
	{
		// Dump the stack
		while (!m_Stack.empty())
			m_Stack.pop();

		// Push the identity onto the stack
		m_Stack.push(
			TRANSFORM_MATRIX(
				1.f, 0.f, 0.f, 0.f,
				0.f, 1.f, 0.f, 0.f,
				0.f, 0.f, 1.f, 0.f
			)
		);
	}

	void MatrixStack::identity()
	{
		m_Stack.top() = TRANSFORM_MATRIX(
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f
		);
	}
	
	void MatrixStack::ortho(float left, float right, float bottom, float top, float near, float far)
	{
		TRANSFORM_MATRIX& m = get();

		float a00 = 2.f / (right - left);
		float a03 = (left + right) / (left - right);
		float a11 = 2.f / (top - bottom);
		float a13 = (bottom + top) / (bottom - top);
		float a22 = 2.f / (far - near);
		float a23 = (near + far) / (near - far);

		for (int k = 2; k >= 0; --k)
		{
			m(k, 3) += (m.get(k, 0) * a03) + (m.get(k, 1) * a13) + (m.get(k, 2) * a23);
			m(k, 0) *= a00;
			m(k, 1) *= a11;
			m(k, 2) *= a22;
		}
	}

	void MatrixStack::custom(const TRANSFORM_MATRIX& matrix)
	{
		get() *= matrix;
	}


	MatrixStack& model()
	{
		static MatrixStack m;
		return m;
	}

	MatrixStack& projection()
	{
		static MatrixStack p;
		return p;
	}

}