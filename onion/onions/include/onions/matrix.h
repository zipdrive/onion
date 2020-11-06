#pragma once
#include <stack>
#include "math.h"


namespace onion
{

	// A matrix of numeric values
	template <typename T, int _Columns, int _Rows>
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
		matrix<T, _Columns, _Rows> operator+(const matrix<T, _Columns, _Rows>& other) const
		{
			matrix<T, _Columns, _Rows> result;

			for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
			{
				result.mat[k] = mat[k] + other.get(k);
			}

			return result;
		}

		/// <summary>Adds another matrix to this one in-place.</summary>
		/// <param name="other">The other matrix.</param>
		/// <returns>A reference to this matrix.</returns>
		matrix<T, _Columns, _Rows>& operator+=(const matrix<T, _Columns, _Rows>& other)
		{
			for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
			{
				mat[k] += other.get(k);
			}

			return *this;
		}


		/// <summary>Subtracts another matrix from this one and returns the result.</summary>
		/// <param name="other">The other matrix.</param>
		/// <returns>The subtraction of this matrix and the other one.</returns>
		matrix<T, _Columns, _Rows> operator-(const matrix<T, _Columns, _Rows>& other) const
		{
			matrix<T, _Columns, _Rows> result;

			for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
			{
				result.mat[k] = mat[k] - other.get(k);
			}

			return result;
		}

		/// <summary>Subtracts another matrix from this one in-place.</summary>
		/// <param name="other">The other matrix.</param>
		/// <returns>A reference to this matrix.</returns>
		matrix<T, _Columns, _Rows>& operator-=(const matrix<T, _Columns, _Rows>& other)
		{
			for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
			{
				mat[k] -= other.get(k);
			}

			return *this;
		}

		

		

		/// <summary>Multiplies a scalar value to this matrix in-place.</summary>
		/// <param name="scalar">The scalar value.</param>
		/// <returns>A reference to this matrix.</returns>
		matrix<T, _Columns, _Rows>& operator*=(Int scalar)
		{
			for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
			{
				mat[k] *= scalar;
			}

			return *this;
		}
	
		/// <summary>Multiplies a scalar value to this matrix in-place.</summary>
		/// <param name="scalar">The scalar value.</param>
		/// <returns>A reference to this matrix.</returns>
		matrix<T, _Columns, _Rows>& operator*=(Float scalar)
		{
			for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
			{
				mat[k] *= scalar;
			}

			return *this;
		}


		/// <summary>Sets this matrix equal to another one.</summary>
		/// <param name="other">The other matrix.</param>
		/// <returns>A reference to this matrix.</returns>
		matrix<T, _Columns, _Rows>& operator=(const matrix<T, _Columns, _Rows>& other)
		{
			for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
			{
				mat[k] = other.get(k);
			}

			return *this;
		}


		/// <summary>Checks whether this matrix is equal to another one.</summary>
		/// <param name="other">The other matrix.</param>
		/// <returns>True if both matrices are equal, false otherwise.</returns>
		bool operator==(const matrix<T, _Columns, _Rows>& other) const
		{
			for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
			{
				if (mat[k] != other.get(k))
					return false;
			}

			return true;
		}

		/// <summary>Checks whether this matrix is not equal to another one.</summary>
		/// <param name="other">The other matrix.</param>
		/// <returns>True if the matrices are not equal, false if they are.</returns>
		bool operator!=(const matrix<T, _Columns, _Rows>& other) const
		{
			for (int k = (_Rows * _Columns) - 1; k >= 0; --k)
			{
				if (mat[k] != other.get(k))
					return true;
			}

			return false;
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


	template <typename T, int _Columns, int _Rows>
	matrix<Float, _Columns, _Rows> operator*(const matrix<T, _Columns, _Rows>& lhs, Float rhs)
	{
		matrix<Float, _Columns, _Rows> res;
		for (int k = (_Columns * _Rows) - 1; k >= 0; --k)
			res(k) = lhs.get(k) * rhs;
		return res;
	}

	template <typename T, int _Columns, int _Rows>
	matrix<Float, _Columns, _Rows> operator*(Float lhs, const matrix<T, _Columns, _Rows>& rhs)
	{
		matrix<Float, _Columns, _Rows> res;
		for (int k = (_Columns * _Rows) - 1; k >= 0; --k)
			res(k) = lhs * rhs.get(k);
		return res;
	}

	template <typename T, int _Columns, int _Rows>
	matrix<primitive_multiplication<T, Int>, _Columns, _Rows> operator*(const matrix<T, _Columns, _Rows>& lhs, Int rhs)
	{
		matrix<primitive_multiplication<T, Int>, _Columns, _Rows> res;
		for (int k = (_Columns * _Rows) - 1; k >= 0; --k)
			res(k) = lhs.get(k) * rhs;
		return res;
	}

	template <typename T, int _Columns, int _Rows>
	matrix<primitive_multiplication<T, Int>, _Columns, _Rows> operator*(Int lhs, const matrix<T, _Columns, _Rows>& rhs)
	{
		matrix<primitive_multiplication<T, Int>, _Columns, _Rows> res;
		for (int k = (_Columns * _Rows) - 1; k >= 0; --k)
			res(k) = lhs * rhs.get(k);
		return res;
	}

	template <int _Columns, int _Rows>
	matrix<Int, _Columns, _Rows> operator*(const matrix<Int, _Columns, _Rows>& lhs, const Frac& rhs)
	{
		matrix<Int, _Columns, _Rows> res;
		for (int k = (_Columns * _Rows) - 1; k >= 0; --k)
			res(k) = lhs.get(k) * rhs;
		return res;
	}

	template <typename T, typename U, int _Columns, int _Rows>
	matrix<primitive_multiplication<T, U>, _Columns, _Rows> operator/(const matrix<T, _Columns, _Rows>& lhs, U rhs)
	{
		matrix<primitive_multiplication<T, U>, _Columns, _Rows> res;
		for (int k = (_Columns * _Rows) - 1; k >= 0; --k)
			res(k) = lhs.get(k) / rhs;
		return res;
	}



	template <typename T, typename U, int _Columns, int _Middle, int _Rows>
	matrix<primitive_multiplication<T, U>, _Columns, _Rows> operator*(const matrix<T, _Middle, _Rows>& lhs, const matrix<U, _Columns, _Middle>& rhs)
	{
		matrix<primitive_multiplication<T, U>, _Columns, _Rows> res;
		for (int i = _Rows - 1; i >= 0; --i)
		{
			for (int j = _Columns - 1; j >= 0; --j)
			{
				res(i, j) = 0;
				for (int k = _Middle - 1; k >= 0; --k)
				{
					res(i, j) += lhs.get(i, k) * rhs.get(k, j);
				}
			}
		}
		return res;
	}

	template <typename T, typename U, int _Rows>
	matrix<primitive_multiplication<T, U>, 1, _Rows> operator*(const matrix<T, 1, _Rows>& lhs, const matrix<U, 1, _Rows>& rhs)
	{
		matrix<primitive_multiplication<T, U>, 1, _Rows> res;
		for (int k = _Rows - 1; k >= 0; --k)
			res(k) = lhs.get(k) * rhs.get(k);
		return res;
	}


#define FLOAT_VEC2				matrix<Float, 1, 2>
#define FLOAT_VEC3				matrix<Float, 1, 3>
#define FLOAT_VEC4				matrix<Float, 1, 4>
#define FLOAT_MAT2				matrix<Float, 2, 2>
#define FLOAT_MAT3X2			matrix<Float, 3, 2>
#define FLOAT_MAT4X2			matrix<Float, 4, 2>
#define FLOAT_MAT2X3			matrix<Float, 2, 3>
#define FLOAT_MAT3				matrix<Float, 3, 3>
#define FLOAT_MAT4X3			matrix<Float, 4, 3>
#define FLOAT_MAT2X4			matrix<Float, 2, 4>
#define FLOAT_MAT3X4			matrix<Float, 3, 4>
#define FLOAT_MAT4				matrix<Float, 4, 4>

#define DOUBLE_VEC2				matrix<Double, 1, 2>
#define DOUBLE_VEC3				matrix<Double, 1, 3>
#define DOUBLE_VEC4				matrix<Double, 1, 4>
#define DOUBLE_MAT2				matrix<Double, 2, 2>
#define DOUBLE_MAT3X2			matrix<Double, 3, 2>
#define DOUBLE_MAT4X2			matrix<Double, 4, 2>
#define DOUBLE_MAT2X3			matrix<Double, 2, 3>
#define DOUBLE_MAT3				matrix<Double, 3, 3>
#define DOUBLE_MAT4X3			matrix<Double, 4, 3>
#define DOUBLE_MAT2X4			matrix<Double, 2, 4>
#define DOUBLE_MAT3X4			matrix<Double, 3, 4>
#define DOUBLE_MAT4				matrix<Double, 4, 4>

#define INT_VEC2				matrix<Int, 1, 2>
#define INT_VEC3				matrix<Int, 1, 3>
#define INT_VEC4				matrix<Int, 1, 4>
#define INT_MAT2				matrix<Int, 2, 2>
#define INT_MAT3X2				matrix<Int, 3, 2>
#define INT_MAT4X2				matrix<Int, 4, 2>
#define INT_MAT2X3				matrix<Int, 2, 3>
#define INT_MAT3				matrix<Int, 3, 3>
#define INT_MAT4X3				matrix<Int, 4, 3>
#define INT_MAT2X4				matrix<Int, 2, 4>
#define INT_MAT3X4				matrix<Int, 3, 4>
#define INT_MAT4				matrix<Int, 4, 4>

#define FRAC_VEC2				matrix<Frac, 1, 2>
#define FRAC_VEC3				matrix<Frac, 1, 3>
#define FRAC_VEC4				matrix<Frac, 1, 4>

#define UINT_VEC2				matrix<Uint, 1, 2>
#define UINT_VEC3				matrix<Uint, 1, 3>
#define UINT_VEC4				matrix<Uint, 1, 4>
#define UINT_MAT2				matrix<Uint, 2, 2>
#define UINT_MAT3X2				matrix<Uint, 3, 2>
#define UINT_MAT4X2				matrix<Uint, 4, 2>
#define UINT_MAT2X3				matrix<Uint, 2, 3>
#define UINT_MAT3				matrix<Uint, 3, 3>
#define UINT_MAT4X3				matrix<Uint, 4, 3>
#define UINT_MAT2X4				matrix<Uint, 2, 4>
#define UINT_MAT3X4				matrix<Uint, 3, 4>
#define UINT_MAT4				matrix<Uint, 4, 4>


	template <typename _Primitive, int _Columns, int _Rows>
	struct type_size<matrix<_Primitive, _Columns, _Rows>>
	{
		/// <summary>Retrieves the size of the primitives that compose the type.</summary>
		static constexpr std::size_t primitive = type_size<_Primitive>::primitive;

		/// <summary>Retrieves the total size of the type.</summary>
		static constexpr std::size_t whole = type_size<_Primitive>::whole * _Rows * _Columns;
	};




	// A vector of numeric values
	template <typename T, int N>
	class vec : public matrix<T, 1, N>
	{
	public:
		/// <summary>Computes the dot product of two vectors.</summary>
		/// <param name="other">The other vector.</param>
		/// <returns>The dot product of the two vectors.</returns>
		T dot(const matrix<T, 1, N>& other) const
		{
			T sum = 0;
			for (int k = N - 1; k >= 0; --k)
				sum += get(k) * other.get(k);
			return sum;
		}

		/// <summary>Calculates the sum of all squared elements in the vector.</summary>
		/// <returns>The square of the magnitude of the vector.</returns>
		T square_sum() const
		{
			T sum = 0;
			for (int k = N - 1; k >= 0; --k)
				sum += get(k) * get(k);
			return sum;
		}

		/// <summary>Normalizes the vector.</summary>
		/// <param name="normalized">Outputs the normalized vector.</param>
		void normalize(matrix<Float, 1, N>& normalized) const
		{
			Float mag = sqrt(square_sum());
			normalized = *this / mag;
		}
	};

	template <typename T> class vec2;
	template <typename T> class vec3;
	template <typename T> class vec4;

	// A 2-by-1 vector of numeric values
	template <typename T>
	class vec2 : public vec<T, 2>
	{
	public:
		vec2() {}

		vec2(T a1, T a2)
		{
			set(0, 0, a1);
			set(1, 0, a2);
		}

		template <typename U, int _Rows>
		vec2(const matrix<U, 1, _Rows>& m)
		{
			set(0, 0, m.get(0));
			set(1, 0, m.get(1));
		}


		vec2& operator=(const matrix<T, 1, 2>& other)
		{
			matrix<T, 1, 2>::operator=(other);
			return *this;
		}
	};

	// A 3-by-1 vector of numeric values
	template <typename T>
	class vec3 : public vec<T, 3>
	{
	public:
		vec3() {}

		vec3(T a1, T a2, T a3)
		{
			set(0, 0, a1);
			set(1, 0, a2);
			set(2, 0, a3);
		}

		template <typename U, int _Rows>
		vec3(const matrix<U, 1, _Rows>& m)
		{
			set(0, 0, m.get(0));
			set(1, 0, m.get(1));
			set(2, 0, m.get(2));
		}

		template <typename U>
		vec3(const matrix<U, 1, 2>& v, T a3)
		{
			set(0, 0, v.get(0));
			set(1, 0, v.get(1));
			set(2, 0, a3);
		}


		vec3& operator=(const matrix<T, 1, 3>& other)
		{
			matrix<T, 1, 3>::operator=(other);
			return *this;
		}


		/// <summary>Computes the cross product of this three-dimensional vector with another.</summary>
		/// <param name="other">The vector on the right-hand side of the cross product.</param>
		/// <param name="result">Outputs the cross product of the two vectors.</param>
		void cross(const matrix<T, 1, 3>& other, matrix<T, 1, 3>& result) const
		{
			result.set(0, 0, (get(1) * other.get(2)) - (get(2) * other.get(1)));
			result.set(1, 0, (get(2) * other.get(0)) - (get(0) * other.get(2)));
			result.set(2, 0, (get(0) * other.get(1)) - (get(1) * other.get(0)));
		}
	};

	// A 4-by-1 vector of numeric values
	template <typename T>
	class vec4 : public vec<T, 4>
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

		template <typename U, int _Rows>
		vec4(const matrix<U, 1, _Rows>& m)
		{
			set(0, 0, m.get(0));
			set(1, 0, m.get(1));
			set(2, 0, m.get(2));
			set(3, 0, m.get(3));
		}

		template <typename U>
		vec4(const matrix<U, 1, 2>& v, T a3, T a4)
		{
			set(0, 0, v.get(0));
			set(1, 0, v.get(1));
			set(2, 0, a3);
			set(3, 0, a4);
		}

		template <typename U>
		vec4(const matrix<U, 1, 3>& v, T a4)
		{
			set(0, 0, v.get(0));
			set(1, 0, v.get(1));
			set(2, 0, v.get(2));
			set(3, 0, a4);
		}


		vec4& operator=(const matrix<T, 1, 4>& other)
		{
			matrix<T, 1, 4>::operator=(other);
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



	// A matrix with 3 rows and 2 columns.
	template <typename T>
	class mat2x3 : public matrix<T, 2, 3>
	{
	public:
		mat2x3() {}

		mat2x3(T a11, T a12,
			T a21, T a22,
			T a31, T a32)
		{
			set(0, 0, a11);
			set(0, 1, a12);

			set(1, 0, a21);
			set(1, 1, a22);

			set(2, 0, a31);
			set(2, 1, a32);
		}

		mat2x3& operator=(const matrix<T, 2, 3>& other)
		{
			matrix<T, 2, 3>::operator=(other);
			return *this;
		}
	};

	// A matrix with 2 rows and 4 columns.
	template <typename T>
	class mat4x2 : public matrix<T, 4, 2>
	{
	public:
		mat4x2(T a11, T a12, T a13, T a14,
			T a21, T a22, T a23, T a24)
		{
			set(0, 0, a11);
			set(0, 1, a12);
			set(0, 2, a13);
			set(0, 3, a14);

			set(1, 0, a21);
			set(1, 1, a22);
			set(1, 2, a23);
			set(1, 3, a24);
		}

		mat4x2& operator=(const matrix<T, 4, 2>& other)
		{
			matrix<T, 4, 2>::operator=(other);
			return *this;
		}
	};

	// 
	template <typename T>
	class mat4x3 : public matrix<T, 4, 3>
	{
	public:
		mat4x3() {}

		mat4x3(T a11, T a12, T a13, T a14,
			T a21, T a22, T a23, T a24,
			T a31, T a32, T a33, T a34)
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
		}

		mat4x3& operator=(const matrix<T, 4, 3>& other)
		{
			matrix<T, 4, 3>::operator=(other);
			return *this;
		}
	};


	typedef mat2x2<Int> mat2x2i;
	#define mat2i mat2x2i
	typedef mat3x3<Int> mat3x3i;
	#define mat3i mat3x3i
	typedef mat4x4<Int> mat4x4i;
	#define mat4i mat4x4i

	typedef mat2x2<Float> mat2x2f;
	#define mat2f mat2x2f
	typedef mat3x3<Float> mat3x3f;
	#define mat3f mat3x3f
	typedef mat4x4<Float> mat4x4f;
	#define mat4f mat4x4f

	typedef mat2x3<Int> mat2x3i;

	typedef mat4x2<Float> mat4x2f;
	typedef mat4x3<Float> mat4x3f;

	typedef vec2<Int> vec2i;
	typedef vec3<Int> vec3i;
	typedef vec4<Int> vec4i;

	typedef vec2<Float> vec2f;
	typedef vec3<Float> vec3f;
	typedef vec4<Float> vec4f;



	class TransformMatrix : public mat4f
	{
	public:
		/// <summary>Constructs a matrix transformation.</summary>
		/// <param name="a11">The element in the first row and first column.</param>
		/// <param name="a12">The element in the first row and second column.</param>
		/// <param name="a13">The element in the first row and third column.</param>
		/// <param name="a14">The element in the first row and fourth column.</param>
		/// <param name="a21">The element in the second row and first column.</param>
		/// <param name="a22">The element in the second row and second column.</param>
		/// <param name="a23">The element in the second row and third column.</param>
		/// <param name="a24">The element in the second row and fourth column.</param>
		/// <param name="a31">The element in the third row and first column.</param>
		/// <param name="a32">The element in the third row and second column.</param>
		/// <param name="a33">The element in the third row and third column.</param>
		/// <param name="a34">The element in the third row and fourth column.</param>
		/// <param name="a41">The element in the fourth row and first column.</param>
		/// <param name="a42">The element in the fourth row and second column.</param>
		/// <param name="a43">The element in the fourth row and third column.</param>
		/// <param name="a44">The element in the fourth row and fourth column.</param>
		TransformMatrix(
			float a11 = 1.f, float a12 = 0.f, float a13 = 0.f, float a14 = 0.f,
			float a21 = 0.f, float a22 = 1.f, float a23 = 0.f, float a24 = 0.f,
			float a31 = 0.f, float a32 = 0.f, float a33 = 1.f, float a34 = 0.f,
			float a41 = 0.f, float a42 = 0.f, float a43 = 0.f, float a44 = 1.f
		);

		/// <summary>Adds a translation to the current transformation.</summary>
		/// <param name="dx">The translation along the x-axis.</param>
		void translate(float tx);

		/// <summary>Adds a translation to the current transformation.</summary>
		/// <param name="dx">The translation along the x-axis.</param>
		/// <param name="dy">The translation along the y-axis.</param>
		void translate(float tx, float ty);

		/// <summary>Adds a translation to the current transformation.</summary>
		/// <param name="dx">The translation along the x-axis.</param>
		/// <param name="dy">The translation along the y-axis.</param>
		/// <param name="dz">The translation along the z-axis.</param>
		void translate(float tx, float ty, float tz);


		/// <summary>Adds a scale transform to the current transformation.</summary>
		/// <param name="sx">The scaling factor for the x-axis.</param>
		void scale(float sx);

		/// <summary>Adds a scale transform to the current transformation.</summary>
		/// <param name="sx">The scaling factor for the x-axis.</param>
		/// <param name="sy">The scaling factor for the y-axis.</param>
		void scale(float sx, float sy);

		/// <summary>Adds a scale transform to the current transformation.</summary>
		/// <param name="sx">The scaling factor for the x-axis.</param>
		/// <param name="sy">The scaling factor for the y-axis.</param>
		/// <param name="sz">The scaling factor for the z-axis.</param>
		void scale(float sx, float sy, float sz);


		/// <summary>Adds a rotation transform around the x-axis to the current transformation.</summary>
		/// <param name="angle">The angle of rotation, in radians.</param>
		void rotatex(float angle);

		/// <summary>Adds a rotation transform around the y-axis to the current transformation.</summary>
		/// <param name="angle">The angle of rotation, in radians.</param>
		void rotatey(float angle);

		/// <summary>Adds a rotation transform around the z-axis to the current transformation.</summary>
		/// <param name="angle">The angle of rotation, in radians.</param>
		void rotatez(float angle);


		/// <summary>Sets the current transformation to the identity matrix.</summary>
		void identity();

		/// <summary>Creates an orthogonal projection.</summary>
		/// <param name="left">The left side of the projection.</param>
		/// <param name="right">The right side of the projection.</param>
		/// <param name="bottom">The bottom side of the projection.</param>
		/// <param name="top">The top side of the projection.</param>
		/// <param name="near">The near side of the projection.</param>
		/// <param name="far">The far side of the projection.</param>
		void ortho(float left, float right, float bottom, float top, float near, float far);


		/// <summary>Sets this matrix equal to the right-hand side of the operator.</summary>
		/// <param name="other">The matrix to set this one equal to.</param>
		/// <returns>A reference to this matrix.</returns>
		TransformMatrix& operator=(const FLOAT_MAT4& other);

		/// <summary>Multiplies this matrix by another one, in place.</summary>
		/// <param name="other">The matrix to multiply this one by.</param>
		/// <returns>A reference to this matrix.</returns>
		TransformMatrix& operator*=(const FLOAT_MAT4& other);
	};





	class MatrixStack
	{
	private:
		// A FIFO stack of matrix transformations.
		std::stack<TransformMatrix> m_Stack;

	public:
		/// <summary>Constructs a matrix stack consisting of a single identity matrix.</summary>
		MatrixStack();


		/// <summary>Retrieves the current matrix transformation.</summary>
		/// <returns>The current matrix transformation.</returns>
		TransformMatrix& get();

		/// <summary>Retrieves the current matrix transformation.</summary>
		/// <returns>The current matrix transformation.</returns>
		const TransformMatrix& get() const;


		/// <summary>Retrieves the current matrix transformation as a raw array.</summary>
		/// <returns>An array representing the current matrix transformation, in row-major order.</returns>
		const float* get_values() const;


		/// <summary>Pushes a copy of the current matrix to the top of the stack.</summary>
		void push();

		/// <summary>Pops the top matrix from the stack.</summary>
		void pop();


		/// <summary>Adds a translation to the current transformation.</summary>
		/// <param name="dx">The translation along the x-axis.</param>
		void translate(float tx);

		/// <summary>Adds a translation to the current transformation.</summary>
		/// <param name="dx">The translation along the x-axis.</param>
		/// <param name="dy">The translation along the y-axis.</param>
		void translate(float tx, float ty);

		/// <summary>Adds a translation to the current transformation.</summary>
		/// <param name="dx">The translation along the x-axis.</param>
		/// <param name="dy">The translation along the y-axis.</param>
		/// <param name="dz">The translation along the z-axis.</param>
		void translate(float tx, float ty, float tz);


		/// <summary>Adds a scale transform to the current transformation.</summary>
		/// <param name="sx">The scaling factor for the x-axis.</param>
		void scale(float sx);

		/// <summary>Adds a scale transform to the current transformation.</summary>
		/// <param name="sx">The scaling factor for the x-axis.</param>
		/// <param name="sy">The scaling factor for the y-axis.</param>
		void scale(float sx, float sy);

		/// <summary>Adds a scale transform to the current transformation.</summary>
		/// <param name="sx">The scaling factor for the x-axis.</param>
		/// <param name="sy">The scaling factor for the y-axis.</param>
		/// <param name="sz">The scaling factor for the z-axis.</param>
		void scale(float sx, float sy, float sz);


		/// <summary>Adds a rotation transform around the x-axis to the current transformation.</summary>
		/// <param name="angle">The angle of rotation, in radians.</param>
		void rotatex(float angle);

		/// <summary>Adds a rotation transform around the y-axis to the current transformation.</summary>
		/// <param name="angle">The angle of rotation, in radians.</param>
		void rotatey(float angle);

		/// <summary>Adds a rotation transform around the z-axis to the current transformation.</summary>
		/// <param name="angle">The angle of rotation, in radians.</param>
		void rotatez(float angle);


		/// <summary>Dumps the stack and sets the transformation to the identity.</summary>
		void reset();
		
		/// <summary>Sets the current transformation to the identity matrix.</summary>
		void identity();
		
		/// <summary>Creates an orthogonal projection.</summary>
		/// <param name="left">The left side of the projection.</param>
		/// <param name="right">The right side of the projection.</param>
		/// <param name="bottom">The bottom side of the projection.</param>
		/// <param name="top">The top side of the projection.</param>
		/// <param name="near">The near side of the projection.</param>
		/// <param name="far">The far side of the projection.</param>
		void ortho(float left, float right, float bottom, float top, float near, float far);


		/// <summary>Multiplies the current matrix by a custom transformation.</summary>
		/// <param name="matrix">The custom transformation matrix.</param>
		void custom(const TransformMatrix& matrix);
	};

}

namespace std
{

	template <typename T, int _Columns, int _Rows>
	struct hash<onion::matrix<T, _Columns, _Rows>>
	{
		size_t operator()(const onion::matrix<T, _Columns, _Rows>& value) const noexcept
		{
			hash<T> hasher;
			size_t res = 0;
			for (int k = (_Columns * _Rows) - 1; k >= 0; --k)
				res = hasher(value.get(k)) ^ (res << 1);
			return res;
		}
	};

}