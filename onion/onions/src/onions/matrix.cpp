#include <forward_list>
#include "../../include/onions/matrix.h"


namespace onion
{

	TransformMatrix::TransformMatrix(
		float a11, float a12, float a13, float a14,
		float a21, float a22, float a23, float a24,
		float a31, float a32, float a33, float a34,
		float a41, float a42, float a43, float a44
	) : mat4f(
		a11, a12, a13, a14,
		a21, a22, a23, a24,
		a31, a32, a33, a34,
		a41, a42, a43, a44
	) {}

	void TransformMatrix::translate(float tx)
	{
		for (int k = 2; k >= 0; --k)
			this->operator()(k, 3) += (tx * get(k, 0));
	}

	void TransformMatrix::translate(float tx, float ty)
	{
		for (int k = 2; k >= 0; --k)
			this->operator()(k, 3) += (tx * get(k, 0)) + (ty * get(k, 1));
	}

	void TransformMatrix::translate(float tx, float ty, float tz)
	{
		for (int k = 2; k >= 0; --k)
			this->operator()(k, 3) += (tx * get(k, 0)) + (ty * get(k, 1)) + (tz * get(k, 2));
	}

	void TransformMatrix::scale(float sx)
	{
		for (int k = 2; k >= 0; --k)
			this->operator()(k, 0) *= sx;
	}

	void TransformMatrix::scale(float sx, float sy)
	{
		for (int k = 2; k >= 0; --k)
		{
			this->operator()(k, 0) *= sx;
			this->operator()(k, 1) *= sy;
		}
	}

	void TransformMatrix::scale(float sx, float sy, float sz)
	{
		for (int k = 2; k >= 0; --k)
		{
			this->operator()(k, 0) *= sx;
			this->operator()(k, 1) *= sy;
			this->operator()(k, 2) *= sz;
		}
	}

	void TransformMatrix::rotatex(float angle)
	{
		float s = sinf(angle);
		float c = cosf(angle);

		for (int k = 2; k >= 0; --k)
		{
			float& r1 = this->operator()(k, 1);
			float& r2 = this->operator()(k, 2);

			float a1 = (r2 * s) + (r1 * c);
			r2 = (r2 * c) - (r1 * s);
			r1 = a1;
		}
	}

	void TransformMatrix::rotatey(float angle)
	{
		float s = sinf(angle);
		float c = cosf(angle);

		for (int k = 2; k >= 0; --k)
		{
			float& r0 = this->operator()(k, 0);
			float& r2 = this->operator()(k, 2);

			float a0 = (r0 * c) - (r2 * s);
			r2 = (r0 * s) + (r2 * c);
			r0 = a0;
		}
	}

	void TransformMatrix::rotatez(float angle)
	{
		float s = sinf(angle);
		float c = cosf(angle);

		for (int k = 2; k >= 0; --k)
		{
			float& r0 = this->operator()(k, 0);
			float& r1 = this->operator()(k, 1);

			float a0 = (r1 * s) + (r0 * c);
			r1 = (r1 * c) - (r0 * s);
			r0 = a0;
		}
	}

	void TransformMatrix::identity()
	{
		for (int i = 3; i >= 0; --i)
			for (int j = 3; j >= 0; --j)
				set(i, j, i == j ? 1.f : 0.f);
	}

	void TransformMatrix::ortho(float left, float right, float bottom, float top, float near, float far)
	{
		float a00 = 2.f / (right - left);
		float a03 = (left + right) / (left - right);
		float a11 = 2.f / (top - bottom);
		float a13 = (bottom + top) / (bottom - top);
		float a22 = 2.f / (far - near);
		float a23 = (near + far) / (near - far);

		for (int k = 2; k >= 0; --k)
		{
			this->operator()(k, 3) += (get(k, 0) * a03) + (get(k, 1) * a13) + (get(k, 2) * a23);
			this->operator()(k, 0) *= a00;
			this->operator()(k, 1) *= a11;
			this->operator()(k, 2) *= a22;
		}
	}

	TransformMatrix& TransformMatrix::operator=(const matrix<float, 4, 4>& other)
	{
		matrix<float, 4, 4>::operator=(other);
		return *this;
	}

	TransformMatrix& TransformMatrix::operator*=(const matrix<float, 4, 4>& other)
	{
		return operator=(operator*(other));
	}




	MatrixStack::MatrixStack() 
	{
		reset();
	}


	TransformMatrix& MatrixStack::get()
	{
		return m_Stack.top();
	}

	const TransformMatrix& MatrixStack::get() const
	{
		return m_Stack.top();
	}

	const float* MatrixStack::get_values() const
	{
		return m_Stack.top().matrix_values();
	}
	
	void MatrixStack::push()
	{
		m_Stack.push(get());
	}

	void MatrixStack::pop()
	{
		if (!m_Stack.empty())
			m_Stack.pop();
	}

	void MatrixStack::translate(float tx)
	{
		get().translate(tx);
	}

	void MatrixStack::translate(float tx, float ty)
	{
		get().translate(tx, ty);
	}
	
	void MatrixStack::translate(float tx, float ty, float tz)
	{
		get().translate(tx, ty, tz);
	}

	void MatrixStack::scale(float sx)
	{
		get().scale(sx);
	}

	void MatrixStack::scale(float sx, float sy)
	{
		get().scale(sx, sy);
	}

	void MatrixStack::scale(float sx, float sy, float sz)
	{
		get().scale(sx, sy, sz);
	}
	
	void MatrixStack::rotatex(float angle)
	{
		get().rotatex(angle);
	}

	void MatrixStack::rotatey(float angle)
	{
		get().rotatey(angle);
	}

	void MatrixStack::rotatez(float angle)
	{
		get().rotatez(angle);
	}

	void MatrixStack::reset()
	{
		// Dump the stack
		while (!m_Stack.empty())
			m_Stack.pop();

		// Push the identity onto the stack
		m_Stack.push(
			TransformMatrix()
		);
	}

	void MatrixStack::identity()
	{
		get().identity();
	}
	
	void MatrixStack::ortho(float left, float right, float bottom, float top, float near, float far)
	{
		get().ortho(left, right, bottom, top, near, far);
	}

	void MatrixStack::custom(const TransformMatrix& matrix)
	{
		get() *= matrix;
	}

}