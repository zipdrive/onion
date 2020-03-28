#pragma once

// A matrix of numeric values
template <typename T, int _Rows, int _Columns>
class matrix
{
protected:
	T mat[_Rows * _Columns];

public:
	/// <summary>Retrieves a pointer to the matrix value array.</summary>
	/// <returns>A pointer to the matrix value array.</returns>
	const T* matrix_values() const
	{
		// DEBUG: Set breakpoint on this line to view 
		return mat;
	}


	/// <summary>Retrieves a reference to the value at the specified index of the matrix value array.</summary>
	/// <param name="index">The index of the value.</param>
	T& operator()(int index)
	{
		return mat[index];
	}

	/// <summary>Retrieves a reference to the value at the specified row and column.</summary>
	/// <param name="row">The row of the value.</param>
	/// <param name="column">The column of the value.</param>
	T& operator()(int row, int column)
	{
		return mat[row + (_Rows * column)]; // Row-major order
	}


	/// <summary>Retrieves the value at the specified index of the matrix value array.</summary>
	/// <param name="index">The index of the value.</param>
	T get(int index) const
	{
		return mat[index];
	}

	/// <summary>Retrieves the value at the specified row and column.</summary>
	/// <param name="row">The row of the value.</param>
	/// <param name="column">The column of the value.</param>
	T get(int row, int column) const
	{
		return mat[row + (_Rows * column)]; // Row-major order
	}


	/// <summary>Sets the value at the specified row and column.</summary>
	/// <param name="row">The row of the value.</param>
	/// <param name="column">The column of the value.</param>
	void set(int row, int column, T value)
	{
		mat[row + (_Rows * column)] = value;
	}



	/// <summary>Adds another matrix to this one and returns the result.</summary>
	/// <param name="other">The other matrix.</param>
	/// <returns>The addition of this matrix and the other one.</returns>
	matrix<T, _Rows, _Columns> operator+(const matrix<T, _Rows, _Columns>& other) const
	{
		matrix<T, _Rows, _Columns> result;

		for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
		{
			result.mat[k] = mat[k] + other.get(k);
		}

		return result;
	}

	/// <summary>Subtracts another matrix from this one and returns the result.</summary>
	/// <param name="other">The other matrix.</param>
	/// <returns>The subtraction of this matrix and the other one.</returns>
	matrix<T, _Rows, _Columns> operator-(const matrix<T, _Rows, _Columns>& other) const
	{
		matrix<T, _Rows, _Columns> result;

		for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
		{
			result.mat[k] = mat[k] - other.get(k);
		}

		return result;
	}

	/// <summary>Multiplies another matrix to this one and returns the result.</summary>
	/// <param name="other">The other matrix.</param>
	/// <returns>The multiplication of this matrix and the other one.</returns>
	template <int _ColumnsRHS>
	matrix<T, _Rows, _ColumnsRHS> operator*(const matrix<T, _Columns, _ColumnsRHS>& other) const
	{
		matrix<T, _Rows, _ColumnsRHS> result;

		for (int r = _Rows - 1; r >= 0; --r)
		{
			for (int c = _ColumnsRHS - 1; c >= 0; --c)
			{
				T val = 0;
				for (int k = _Columns - 1; k >= 0; --k)
					val += get(r, k) * other.get(k, c);
				result.set(r, c, val);
			}
		}

		return result;
	}



	/// <summary>Adds another matrix to this one in-place.</summary>
	/// <param name="other">The other matrix.</param>
	/// <returns>A reference to this matrix.</returns>
	matrix<T, _Rows, _Columns>& operator+=(const matrix<T, _Rows, _Columns>& other)
	{
		for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
		{
			mat[k] += other.get(k);
		}

		return *this;
	}

	/// <summary>Subtracts another matrix from this one in-place.</summary>
	/// <param name="other">The other matrix.</param>
	/// <returns>A reference to this matrix.</returns>
	matrix<T, _Rows, _Columns>& operator-=(const matrix<T, _Rows, _Columns>& other)
	{
		for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
		{
			mat[k] -= other.get(k);
		}

		return *this;
	}


	/// <summary>Sets this matrix equal to another one.</summary>
	/// <param name="other">The other matrix.</param>
	/// <returns>A reference to this matrix.</returns>
	matrix<T, _Rows, _Columns>& operator=(const matrix<T, _Rows, _Columns>& other)
	{
		for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
		{
			mat[k] = other.get(k);
		}

		return *this;
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


/// <summary>Multiplies two matrices.</summary>
/// <param name="lhs">The matrix being multiplied on the left.</param>
/// <param name="rhs">The matrix being multiplied on the right.</param>
/// <param name="res">Outputs the resulting matrix.</param>
template <typename T, int _Rows, int _Middle, int _Columns>
void mat_mul(const matrix<T, _Rows, _Middle>& lhs, const matrix<T, _Middle, _Columns>& rhs, matrix<T, _Rows, _Columns>& res)
{
	for (int r = _Rows - 1; r >= 0; --r)
	{
		for (int c = _Columns - 1; c >= 0; --c)
		{
			T val = 0;
			for (int k = _Middle - 1; k >= 0; --k)
				val += lhs.get(r, k) * rhs.get(k, c);
			res.set(r, c, val);
		}
	}
}




template <typename T> class vec2;
template <typename T> class vec3;
template <typename T> class vec4;

// A 2-by-1 vector of numeric values
template <typename T>
class vec2 : public matrix<T, 2, 1>
{
public:
	vec2() {}

	vec2(T a1, T a2)
	{
		set(0, 0, a1);
		set(1, 0, a2);
	}

	template <typename U>
	vec2(const vec2<U>& v)
	{
		set(0, 0, v.get(0));
		set(1, 0, v.get(1));
	}
	
	template <typename U>
	vec2(const vec3<U>& v)
	{
		set(0, 0, v.get(0));
		set(1, 0, v.get(1));
	}

	template <typename U>
	vec2(const vec4<U>& v)
	{
		set(0, 0, v.get(0));
		set(1, 0, v.get(1));
	}


	vec2& operator=(const matrix<T, 2, 1>& other)
	{
		matrix<T, 2, 1>::operator=(other);
		return *this;
	}
};

// A 3-by-1 vector of numeric values
template <typename T>
class vec3 : public matrix<T, 3, 1>
{
public:
	vec3() {}

	vec3(T a1, T a2, T a3)
	{
		set(0, 0, a1);
		set(1, 0, a2);
		set(2, 0, a3);
	}

	template <typename U>
	vec3(const vec2<U>& v, T a3)
	{
		set(0, 0, v.get(0));
		set(1, 0, v.get(1));
		set(2, 0, a3);
	}

	template <typename U>
	vec3(const vec3<U>& v)
	{
		set(0, 0, v.get(0));
		set(1, 0, v.get(1));
		set(2, 0, v.get(2));
	}
	
	template <typename U>
	vec3(const vec4<U>& v)
	{
		set(0, 0, v.get(0));
		set(1, 0, v.get(1));
		set(2, 0, v.get(2));
	}


	vec3& operator=(const matrix<T, 3, 1>& other)
	{
		matrix<T, 3, 1>::operator=(other);
		return *this;
	}
};

// A 4-by-1 vector of numeric values
template <typename T>
class vec4 : public matrix<T, 4, 1>
{
public:
	vec4() {}

	vec4(T a1, T a2, T a3, T a4)
	{
		set(0, 0, a1);
		set(1, 0, a2);
		set(2, 0, a3);
		set(3, 0, a4);
	}

	template <typename U>
	vec4(const vec2<U>& v, T a3, T a4)
	{
		set(0, 0, v.get(0));
		set(1, 0, v.get(1));
		set(2, 0, a3);
		set(3, 0, a4);
	}

	template <typename U>
	vec4(const vec3<U>& v, T a4)
	{
		set(0, 0, v.get(0));
		set(1, 0, v.get(1));
		set(2, 0, v.get(2));
		set(3, 0, a4);
	}

	template <typename U>
	vec4(const vec4<U>& v)
	{
		set(0, 0, v.get(0));
		set(1, 0, v.get(1));
		set(2, 0, v.get(2));
		set(3, 0, v.get(3));
	}


	vec4& operator=(const matrix<T, 4, 1>& other)
	{
		matrix<T, 4, 1>::operator=(other);
		return *this;
	}
};


// A 2-by-2 matrix of numeric values
template <typename T>
class mat2x2 : public matrix<T, 2, 2>
{
public:
	mat2x2() {}

	mat2x2(T a11, T a12, T a21, T a22)
	{
		set(0, 0, a11);
		set(0, 1, a12);
		set(1, 0, a21);
		set(1, 1, a22);
	}

	template <typename U>
	mat2x2(const mat2x2<U>& m)
	{
		for (int k = 3; k >= 0; --k)
			mat[k] = m.get(k);
	}


	mat2x2& operator=(const matrix<T, 2, 2>& other)
	{
		matrix<T, 2, 2>::operator=(other);
		return *this;
	}

	mat2x2& operator*=(const matrix<T, 2, 2>& other)
	{
		return operator=(operator*(other));
	}
};

// A 3-by-3 matrix of numeric values
template <typename T>
class mat3x3 : public matrix<T, 3, 3>
{
public:
	mat3x3() {}

	mat3x3(T a11, T a12, T a13,
		T a21, T a22, T a23,
		T a31, T a32, T a33)
	{
		set(0, 0, a11);
		set(0, 1, a12);
		set(0, 2, a13);
		set(1, 0, a21);
		set(1, 1, a22);
		set(1, 2, a23);
		set(2, 0, a31);
		set(2, 1, a32);
		set(2, 2, a33);
	}

	template <typename U>
	mat3x3(const mat3x3<U>& m)
	{
		for (int k = 8; k >= 0; --k)
			mat[k] = m.get(k);
	}


	mat3x3& operator=(const matrix<T, 3, 3>& other)
	{
		matrix<T, 3, 3>::operator=(other);
		return *this;
	}

	mat3x3& operator*=(const matrix<T, 3, 3>& other)
	{
		return operator=(operator*(other));
	}
};

// A 4-by-4 matrix of float values
template <typename T>
class mat4x4 : public matrix<T, 4, 4>
{
public:
	mat4x4() {}

	mat4x4(T a11, T a12, T a13, T a14,
		T a21, T a22, T a23, T a24,
		T a31, T a32, T a33, T a34,
		T a41 = 0, T a42 = 0, T a43 = 0, T a44 = 1)
	{
		set(0, 0, a11);
		set(0, 1, a12);
		set(0, 2, a13);
		set(0, 3, a14);

		set(1, 0, a21);
		set(1, 1, a22);
		set(1, 2, a23);
		set(1, 3, a24);

		set(2, 0, a31);
		set(2, 1, a32);
		set(2, 2, a33);
		set(2, 3, a34);

		set(3, 0, a41);
		set(3, 1, a42);
		set(3, 2, a43);
		set(3, 3, a44);
	}

	template <typename U>
	mat4x4(const mat4x4<U>& m)
	{
		for (int k = 15; k >= 0; --k)
			mat[k] = m.get(k);
	}


	mat4x4& operator=(const matrix<T, 4, 4>& other)
	{
		matrix<T, 4, 4>::operator=(other);
		return *this;
	}

	mat4x4& operator*=(const matrix<T, 4, 4>& other)
	{
		return operator=(operator*(other));
	}
};


typedef mat2x2<int> mat2x2i;

typedef mat2x2<float> mat2x2f;
typedef mat3x3<float> mat3x3f;
typedef mat4x4<float> mat4x4f;

typedef vec2<int> vec2i;
typedef vec3<int> vec3i;
typedef vec4<int> vec4i;

typedef vec2<float> vec2f;
typedef vec3<float> vec3f;
typedef vec4<float> vec4f;


/// <summary>Pushes a copy of the current matrix to the top of the stack.</summary>
void mat_push();

/// <summary>Pops the current matrix from the top of the stack.</summary>
void mat_pop();


/// <summary>Gets the current matrix transform.</summary>
mat4x4f& mat_get();

/// <summary>Gets the value array of the current matrix transform.</summary>
/// <returns>The value array of the current matrix transform.</returns>
const float* mat_get_values();



/// <summary>Sets the current matrix transform to the identity.</summary>
void mat_identity();


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
void mat_rotatez(float angle);


/// <summary>Adds a custom transformation to the current transformation.</summary>
/// <param name="transform">The matrix of the transformation.</summary>
void mat_custom_transform(const mat4x4f& transform);