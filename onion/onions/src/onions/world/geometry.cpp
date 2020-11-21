#include "../../../include/onions/world/geometry.h"

namespace onion
{
	namespace world
	{

		template <typename T, int N>
		bool lambda_all_positive(const matrix<T, 1, N>& lambda)
		{
			for (int k = N - 1; k >= 0; --k)
				if (lambda.get(k) < 0)
					return false;
			return true;
		}


		vec3f dir_to_origin(const vec3f& a, const vec3f& b)
		{
			// Project the origin onto the line formed by the vertices
			vec3f s_diff = b - a;

			Float numer[2];
			numer[0] = s_diff.dot(b);
			numer[1] = s_diff.dot(a);
			Float denom = 1.f / s_diff.square_sum();

			if (numer[0] > 0 && numer[1] < 0)
			{
				// The projected origin lies between the two vertices
				return ((b * numer[1]) - (a * numer[0])) * denom;
			}
			else
			{
				// The projected origin lies outside the line segment formed by the two vertices, so delete a vertex
				return -1.f * (a.square_sum() > b.square_sum() ? b : a);
			}
		}

		
		void nearest_simplex1d(const Simplex& s_input, Simplex& s_output, vec3f& dir)
		{
			// Project the origin onto the line formed by the vertices
			vec3f s_diff = s_input[1] - s_input[0];

			Float numer[2];
			numer[0] = s_diff.dot(s_input[1]);
			numer[1] = s_diff.dot(s_input[0]);
			Float denom = 1.f / s_diff.square_sum();

			if (numer[0] > 0 && numer[1] < 0)
			{
				// The projected origin lies between the two vertices
				s_output = s_input;
				dir = ((s_input[1] * numer[1]) - (s_input[0] * numer[0])) * denom;
			}
			else
			{
				// The projected origin lies outside the line segment formed by the two vertices, so delete a vertex
				s_output = { s_input[0] };
				dir = -1.f * s_input[0];
			}
		}

		void nearest_simplex2d(const Simplex& s_input, Simplex& s_output, vec3f& dir)
		{
			// Calculate the two vectors that constitute the basis for the plane
			vec3f d[2] = { s_input[1] - s_input[0], s_input[2] - s_input[0] };

			Float d00 = d[0].square_sum();
			Float d11 = d[1].square_sum();
			Float d01 = d[0].dot(d[1]);

			// Construct the adjugate matrix
			FLOAT_MAT3X2 adj;
			for (int k = 2; k >= 0; --k)
			{
				adj.set(0, k, (d[0].get(k) * d11) - (d[1].get(k) * d01));
				adj.set(1, k, (d[1].get(k) * d00) - (d[0].get(k) * d01));
			}

			// Calculate the normal vector of the plane formed by the vertices
			vec3f normal;
			d[0].cross(d[1], normal);

			// Calculate the projection of the origin onto the plane formed by the simplex
			vec3f o = normal * normal.dot(s_input[0]) / normal.square_sum();

			// Calculate the barycentric coordinates of the origin projection, with lambda_0 = 1
			vec2f numer = adj * (o - s_input[0]);
			Float denom = (d00 * d11) - (d01 * d01); // TODO make sure nonzero!

			vec3f lambda;
			lambda(0) = 1.f;
			lambda(1) = numer.get(0) / denom;
			lambda(2) = numer.get(1) / denom;

			if (lambda_all_positive(lambda))
			{
				// The projected origin lies within the triangle
				s_output = s_input;
				dir = -1.f * o;
			}
			else
			{
				// Delete a vertex
				Float d_min = std::numeric_limits<Float>::max();

				for (int c = 2; c > 0; --c)
				{
					if (lambda.get(c) < 0) // Discard the c-th vertex
					{
						// Construct a simplex with the c-th vertex excluded
						Simplex w = { s_input[0], s_input[c == 1 ? 2 : 1] };

						// Run the sub-routine for a triangle
						Simplex s_output_temp;
						vec3f dir_temp;
						nearest_simplex1d(w, s_output_temp, dir_temp);

						// In case there are multiple negative lambda values, check which has the better minimum norm
						Float d = dir_temp.square_sum();
						if (d < d_min)
						{
							s_output = s_output_temp;
							dir = dir_temp;
							d_min = d;
						}
					}
				}
			}
		}

		void nearest_simplex3d(const Simplex& s_input, Simplex& s_output, vec3f& dir)
		{
			// Construct a barycentric coordinate system
			vec3f s_diff[3];
			for (int c = 3; c > 0; --c)
				s_diff[c - 1] = s_input[c] - s_input[0];

			// Calculate the barycentric coordinates of the origin
			vec3f o = -1.f * s_input[0];

			vec3f cross[3];
			o.cross(s_diff[1], cross[0]);
			s_diff[0].cross(o, cross[1]);
			s_diff[0].cross(s_diff[1], cross[2]);

			if (Float denom = cross[2].dot(s_diff[2])) 
			{
				// The three vectors form a basis for R^3
				FRAC_VEC4 lambda;
				lambda(0) = 1;
				lambda(1) = cross[0].dot(s_diff[2]) / denom;
				lambda(2) = cross[1].dot(s_diff[2]) / denom;
				lambda(3) = cross[2].dot(o) / denom;

				if (lambda_all_positive(lambda))
				{
					// The origin lies within the tetrahedron
					s_output = s_input;
					dir = vec3f(0.f, 0.f, 0.f);
				}
				else
				{
					// Delete a vertex
					Float d_min = std::numeric_limits<Int>::max();

					for (int c = 3; c > 0; --c)
					{
						if (lambda.get(c) < 0) // Discard the c-th vertex
						{
							// Construct a simplex with the c-th vertex excluded
							Simplex w;
							for (int n = 0; n < 4; ++n)
								if (n != c)
									w.push_back(s_input[n]);

							// Run the sub-routine for a triangle
							Simplex s_output_temp;
							vec3f dir_temp;
							nearest_simplex2d(w, s_output_temp, dir_temp);

							// In case there are multiple negative lambda values, check which has the better minimum norm
							Float d = dir_temp.square_sum();
							if (d < d_min)
							{
								s_output = s_output_temp;
								dir = dir_temp;
								d_min = d;
							}
						}
					}
				}
			}
			else
			{
				// The three vectors are coplanar, which shouldn't ever happen because that would mean that a * d < 0 in the __get_distance function? But just in case...
				s_output = s_input;
			}
		}
		
		/// <summary></summary>
		/// <returns>True if the generated simplex contains the origin, false otherwise.</returns>
		void nearest_simplex(const Simplex& s_input, Simplex& s_output, vec3f& dir)
		{
			switch (s_input.size())
			{
			case 2: // Line
			{
				nearest_simplex1d(s_input, s_output, dir);
				break;
			}
			case 3: // Triangle
			{
				nearest_simplex2d(s_input, s_output, dir);
				break;
			}
			case 4: // Tetrahedron
			{
				nearest_simplex3d(s_input, s_output, dir);
				break;
			}
			default: // Point
			{
				s_output = s_input;
				dir = -1.f * s_input[0];
			}
			}
		}



		Int Shape::__get_distance(const Shape* other, Simplex& s, vec3f& d) const
		{
			static constexpr Float inv_distance_scale = 1.f / ONION_WORLD_GEOMETRY_SCALE;
			static constexpr Float epsilon = ONION_WORLD_GEOMETRY_SCALE * ONION_WORLD_GEOMETRY_SCALE * 0.5f;

			// Calculate the next point on the Minkowski difference
			vec3f d0 = d;
			vec3f a1 = support(d0);
			vec3f a2 = other->support(-1.f * d0);
			vec3f a = a1 - a2;

			if (a.dot(d) < 0)
			{
				// No intersection
				if (s.size() == 1)
				{
					Simplex temp = { a, s[0] };

					nearest_simplex(temp, s, d);
					return (Int)round(sqrt(d.square_sum()) * inv_distance_scale);
				}
				else
				{
					Float dist_min = type_limits<Float>::max();

					for (int c = s.size() - 1; c >= 0; --c)
					{
						Simplex temp = { a };
						for (int n = s.size() - 1; n >= 0; --n)
							if (n != c)
								temp.push_back(s[n]);

						Simplex s_temp;
						vec3f d_temp;
						nearest_simplex(temp, s_temp, d_temp);

						Float dist = d_temp.square_sum();
						if (dist < dist_min)
						{
							s = s_temp;
							d = d_temp;
							dist_min = dist;
						}
					}

					return (Int)round(sqrt(dist_min) * inv_distance_scale);
				}
			}

			// Add a to the simplex, making sure there are no duplicates
			Simplex temp = { a };
			temp.insert(temp.end(), s.begin(), s.end());

			// Calculate the simplex on the current simplex that is nearest to the origin
			nearest_simplex(temp, s, d);
			
			Float dist = d.square_sum();
			if (dist < epsilon || s.size() == 4)
			{
				return (Int)round(sqrt(dist) * inv_distance_scale);
			}
			else
			{
				// Recurse
				return __get_distance(other, s, d);
			}
		}

		Int Shape::get_distance(const Shape* other) const
		{
			// Generate an initial guess
			vec3f d = ONION_WORLD_GEOMETRY_SCALE * (other->get_position() - get_position());
			vec3f a1 = support(d) - other->support(-1.f * d);
			d = -1.f * d;
			vec3f a2 = support(d) - other->support(-1.f * d);
			d = dir_to_origin(a1, a2);
			
			Simplex s = { a1, a2 };

			// Calculate the distance between the two shapes using GJK algorithm
			return __get_distance(other, s, d);
		}



		Point::Point(const vec3i& pos)
		{
			m_Position = pos;
		}

		vec3i Point::get_position() const
		{
			return m_Position;
		}

		void Point::set_position(const vec3i& pos)
		{
			translate(pos - m_Position);
		}

		void Point::translate(const vec3i& trans)
		{
			m_Position += trans;
		}
		
		vec3f Point::support(const vec3f& dir) const
		{
			return ONION_WORLD_GEOMETRY_SCALE * m_Position;
		}


		Segment::Segment(const vec3i& origin, const vec3i& direction) : Point(origin)
		{
			m_EndPoint = origin + direction;
		}

		vec3f Segment::support(const vec3f& dir) const
		{
			Float d1 = dir.dot(m_Position);
			Float d2 = dir.dot(m_EndPoint);

			return ONION_WORLD_GEOMETRY_SCALE * (d1 >= d2 ? m_Position : m_EndPoint);
		}

		void Segment::translate(const vec3i& trans)
		{
			Point::translate(trans);
			m_EndPoint += trans;
		}
		
		
		OrthogonalPrism::OrthogonalPrism(const vec3i& pos, const vec3i& dimensions) : Point(pos)
		{
			m_Dimensions = dimensions;
		}

		vec3f OrthogonalPrism::support(const vec3f& dir) const
		{
			vec3i res = m_Position;
			for (int k = 2; k >= 0; --k)
				if (dir.get(k) > 0)
					res(k) += m_Dimensions.get(k);
			return ONION_WORLD_GEOMETRY_SCALE * res;
		}


		UprightRectangle::UprightRectangle(const vec3i& pos, const vec3i& dimensions) : Point(pos) 
		{
			m_Dimensions = dimensions;
		}

		vec3f UprightRectangle::support(const vec3f& dir) const
		{
			vec3i res = m_Position;

			if ((m_Dimensions.get(0) * dir.get(0)) + (m_Dimensions.get(1) * dir.get(1)) > 0.f)
			{
				res(0) += m_Dimensions.get(0);
				res(1) += m_Dimensions.get(1);
			}

			if (dir.get(2) > 0.f)
			{
				res(2) += m_Dimensions.get(2);
			}

			return ONION_WORLD_GEOMETRY_SCALE * res;
		}


		Parallelogram::Parallelogram(const vec3i& pos, const vec3i& dir1, const vec3i& dir2) : Point(pos)
		{
			m_Radii[0] = dir1;
			m_Radii[1] = dir2;
		}

		vec3f Parallelogram::support(const vec3f& dir) const
		{
			vec3i res;
			Float d_max = std::numeric_limits<Float>::min();

			for (int c = 3; c >= 0; --c)
			{
				vec3i p = m_Position;
				if (c % 2 > 0)
					p += m_Radii[0];
				if (c / 2 > 0)
					p += m_Radii[1];

				Float d = dir.dot(p);
				if (d > d_max)
				{
					res = p;
					d_max = d;
				}
			}

			return ONION_WORLD_GEOMETRY_SCALE * res;
		}





		SubpixelHandler::SubpixelHandler(Shape* shape) : m_Subpixels(0, 0, 0)
		{
			m_Shape = shape;
		}

		void SubpixelHandler::translate(const vec3i& trans)
		{
			m_Subpixels += trans;

			vec3i t;
			for (int k = 2; k >= 0; --k)
			{
				// This might give inaccurate results depending on C++ implementation of how rounding is done for negative integer division, but it should probably be fine
				t(k) = m_Subpixels.get(k) / num_subpixels;
				m_Subpixels(k) -= t.get(k) * num_subpixels;

				if (m_Subpixels.get(k) < 0)
				{
					--t(k);
					m_Subpixels(k) += num_subpixels;
				}
			}

			m_Shape->translate(t);
		}

	}
}