#pragma once

template <typename T, int _Rows, int _Columns>
class matrix
{
private:
	T mat[_Rows * _Columns];

public:
	/// <summary>Retrieves a pointer to the matrix value array.</summary>
	/// <returns>A pointer to the matrix value array.</returns>
	const T* matrix_values()
	{
		return mat;
	}

	/// <summary>Retrieves a reference to the value at the specified index of the matrix value array.</summary>
	/// <param name="index">The index of the value.</param>
	T& get(int index)
	{
		return mat[index];
	}

	/// <summary>Retrieves a reference to the value at the specified row and column.</summary>
	/// <param name="row">The row of the value.</param>
	/// <param name="column">The column of the value.</param>
	T& get(int row, int column)
	{
		return mat[row + (_Rows * column)]; // Row-major order
		//return mat[(row * _Columns) + column]; // Column-major order
	}

	/// <summary>Sets the value at the specified row and column.</summary>
	/// <param name="row">The row of the value.</param>
	/// <param name="column">The column of the value.</param>
	void set(int row, int column, T value)
	{
		get(row, column) = value;
	}

	matrix()
	{
		for (int r = _Rows - 1; r >= 0; --r)
		{
			for (int c = _Columns - 1; c >= 0; --c)
			{
				if (r == c && _Rows == _Columns)
					set(r, c, 1);
				else
					set(r, c, 0);
			}
		}
	}
};

template <typename T, int _Rows, int _Middle, int _Columns>
void mat_mul(matrix<T, _Rows, _Middle>& lhs, matrix<T, _Middle, _Columns>& rhs, matrix<T, _Rows, _Columns>& res)
{
	for (int r = _Rows - 1; r >= 0; --r)
	{
		for (int c = _Columns - 1; c >= 0; --c)
		{
			T val = 0;
			for (int k = _Middle - 1; k >= 0; --k)
				val += lhs.get(r, k) * rhs.get(k, c);
			res.get(r, c) = val;
		}
	}
}


class mat4x4f : public matrix<float, 4, 4>
{
public:
	mat4x4f();

	mat4x4f(float a11, float a12, float a13, float a14,
		float a21, float a22, float a23, float a24,
		float a31, float a32, float a33, float a34);
};


typedef matrix<int, 2, 2> mat2x2i;

typedef matrix<float, 2, 1> vec2f;
typedef matrix<float, 4, 1> vec4f;


/// <summary>Pushes a copy of the current matrix to the top of the stack.</summary>
void mat_push();

/// <summary>Pops the current matrix from the top of the stack.</summary>
void mat_pop();

/// <summary>Gets the current matrix transform.</summary>
mat4x4f& mat_get();

/// <summary>Gets the value array of the current matrix transform.</summary>
/// <returns>The value array of the current matrix transform.</returns>
const float* mat_get_values();

/// <summary>Clears the stack and sets the projection to orthogonal.</summary>
/// <param name="left">The left side of the projection.</param>
/// <param name="right">The right side of the projection.</param>
/// <param name="bottom">The bottom side of the projection.<param>
/// <param name="top">The top side of the projection.</param>
void mat_ortho(float left, float right, float bottom, float top, float near, float far);

/// <summary>Adds a translation to the current transformation.</summary>
/// <param name="dx">The translation along the x-axis.</param>
/// <param name="dy">The translation along the y-axis.</param>
/// <param name="dz">The translation along the z-axis.</param>
void mat_translate(float dx, float dy, float dz);

/// <summary>Adds a scale transform to the current transformation.</summary>
/// <param name="sx">The scaling factor for the x-axis.</param>
/// <param name="sy">The scaling factor for the y-axis.</param>
/// <param name="sz">The scaling factor for the z-axis.</param>
void mat_scale(float sx, float sy, float sz);

/// <summary>Adds a rotation transform around the x-axis to the current transformation.</summary>
/// <param name="angle">The angle of rotation.</param>
void mat_rotatex(float angle);

/// <summary>Adds a rotation transform around the y-axis to the current transformation.</summary>
/// <param name="angle">The angle of rotation.</param>
void mat_rotatey(float angle);

/// <summary>Adds a rotation transform around the z-axis to the current transformation.</summary>
/// <param name="angle">The angle of rotation.</param>
void mat_rotatex(float angle);

/// <summary>Adds a rotation transform around the x-axis, followed by a scale transform of the y-axis, to the current transformation.</summary>
/// <param name="angle">The angle of rotation.</param>
/// <param name="sy">The scaling factor for the y-axis.</param>
void mat_rotatex_scaley(float angle, float sy);

/// <summary>Adds a custom transformation to the current transformation.</summary>
/// <param name="transform">The matrix of the transformation.</summary>
void mat_custom_transform(mat4x4f& transform);