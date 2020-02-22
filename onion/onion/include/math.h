#pragma once

#define MAT_INDEX(r, c) (r) + (_Rows * (c))

template <typename T, int _Rows, int _Columns>
struct matrix
{
	T mat[_Rows * _Columns];

	matrix()
	{
		for (int r = _Rows - 1; r >= 0; --r)
		{
			for (int c = _Columns - 1; c >= 0; --c)
			{
				if (r == c)
					set(r, c, 1);
				else
					set(r, c, 0);
			}
		}
	}

	T get(int row, int column)
	{
		return mat[MAT_INDEX(row, column)];
	}

	void set(int row, int column, T value)
	{
		mat[MAT_INDEX(row, column)] = value;
	}
};

template <typename T, int _Rows, int _Middle, int _Columns>
matrix<T, _Rows, _Columns> matmul(matrix<T, _Rows, _Middle>& lhs, matrix<T, _Middle, _Columns>& rhs)
{
	matrix<T, _Rows, _Columns> res;

	for (int r = _Rows - 1; r >= 0; --r)
	{
		for (int c = _Columns - 1; c >= 0; --c)
		{
			T val = default;
			for (int k = _Middle - 1; k >= 0; --k)
				val += lhs.get(r, k) * rhs.get(k, c);
			res.set(r, c) = val;
		}
	}

	return res;
}

typedef matrix<float, 4, 4> mat4x4f;
typedef matrix<float, 4, 1> vec4f;


/// <summary>Pushes a copy of the current matrix to the top of the stack.</summary>
void mat_push();

/// <summary>Pops the current matrix from the top of the stack.</summary>
void mat_pop();

/// <summary>Gets the current matrix transform.</summary>
mat4x4f& mat_get();

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