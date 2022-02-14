#include "../../../include/onions/world/geometry.h"

namespace onion
{
	namespace world
	{

		/// <summary>
		/// Returns true if all values in the vector are non-negative.
		/// </summary>
		/// <typeparam name="T">The number type (e.g. Int, Float).</typeparam>
		/// <param name="lambda">The vector.</param>
		/// <returns>True if all values in the vector are non-negative.</returns>
		template <typename T, int N>
		bool lambda_all_positive(const matrix<T, 1, N>& lambda)
		{
			for (int k = N - 1; k >= 0; --k)
				if (lambda.get(k) < 0)
					return false;
			return true;
		}


		/// <summary>
		/// Projects the origin onto the line segment formed by the two vertices passed.
		/// </summary>
		/// <param name="a">The first vertex of the line segment.</param>
		/// <param name="b">The second vertex of the line segment.</param>
		/// <returns>The point on the line segment with the lowest magnitude.</returns>
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



		struct Simplex
		{
		protected:
			/// <summary>
			/// Constructs a new simplex with the vertex added.
			/// </summary>
			/// <param name="vertex">The vertex to add.</param>
			/// <param name="new_simplex">Outputs the expanded simplex.</param>
			virtual void __expand(const vec3f& vertex, Simplex*& new_simplex) = 0;

			/// <summary>
			/// Constructs the sub-simplex that is closest to the origin.
			/// </summary>
			/// <param name="new_simplex">Outputs the sub-simplex that is closest to the origin.</param>
			/// <param name="dir">Outputs the smallest direction vector to the origin normal to the sub-simplex.</param>
			virtual void __prune(Simplex*& new_simplex, vec3f& dir) = 0;

		public:
			// Minimum "non-zero" floating-point value
			static constexpr Float epsilon = ONION_WORLD_GEOMETRY_SCALE * ONION_WORLD_GEOMETRY_SCALE * 0.5f;

			/// <summary>
			/// Expands the simplex by adding the given vertex. Does nothing if the simplex already contains the vertex or the simplex has 4 vertices.
			/// </summary>
			/// <param name="simplex">The simplex to expand.</param>
			/// <param name="vertex">The vertex to add to the given simplex.</param>
			static void expand(Simplex** simplex, const vec3f& vertex)
			{
				Simplex* temp;

				(*simplex)->__expand(vertex, temp);
				if (*simplex != temp)
				{
					delete* simplex;
					*simplex = temp;
				}
			}

			/// <summary>
			/// Prunes the simplex down to the sub-simplex that is closest to the origin.
			/// </summary>
			/// <param name="simplex">The simplex to prune.</param>
			/// <returns>The smallest direction vector to the origin normal to the sub-simplex.</returns>
			static vec3f prune(Simplex** simplex)
			{
				Simplex* temp = nullptr;
				vec3f dir;

				(*simplex)->__prune(temp, dir);
				if (*simplex != temp)
				{
					delete* simplex;
					*simplex = temp;
				}

				return dir;
			}

			/// <summary>
			/// Virtual deconstructor.
			/// </summary>
			virtual ~Simplex() {}

			/// <summary>
			/// Retrieves the number of vertices in the simplex.
			/// </summary>
			/// <returns>The number of vertices in the simplex.</returns>
			virtual Int count() = 0;
		};
		
		
		template <int N>
		struct NSimplex : public Simplex
		{
		private:
			friend class NSimplex<N + 1>;
			friend class NSimplex<N - 1>;

			vec3f m_Vertices[N];

			NSimplex(NSimplex<N - 1>* copy_from, const vec3f& addition)
			{
				for (int k = N - 1; k > 0; --k)
					m_Vertices[k] = copy_from->m_Vertices[k - 1];
				m_Vertices[0] = addition;
			}

			NSimplex(NSimplex<N + 1>* copy_from, int omit_index)
			{
				for (int k = N - 1; k >= 0; --k)
					m_Vertices[k] = copy_from->m_Vertices[k + (k >= omit_index ? 1 : 0)];
			}

		protected:
			/// <summary>
			/// Constructs a new simplex with the vertex added.
			/// </summary>
			/// <param name="vertex">The vertex to add.</param>
			/// <param name="new_simplex">Outputs the expanded simplex.</param>
			void __expand(const vec3f& vertex, Simplex*& new_simplex)
			{
				for (int k = N - 1; k >= 0; --k)
				{
					if (vec3f(vertex - m_Vertices[k]).square_sum() < epsilon)
					{
						new_simplex = this;
						return;
					}
				}

				new_simplex = new NSimplex<N + 1>(this, vertex);
			}

			/// <summary>
			/// Constructs the sub-simplex that is closest to the origin.
			/// </summary>
			/// <param name="new_simplex">Outputs the sub-simplex that is closest to the origin.</param>
			/// <param name="dir">Outputs the smallest direction vector to the origin normal to the sub-simplex.</param>
			void __prune(Simplex*& new_simplex, vec3f& dir);

		public:
			/// <summary>
			/// Retrieves the number of vertices in the simplex.
			/// </summary>
			/// <returns>The number of vertices in the simplex.</returns>
			Int count() 
			{
				return N;
			}
		};

		template <>
		class NSimplex<1> : public Simplex
		{
		private:
			friend class NSimplex<2>;

			vec3f m_Vertices[1];

			NSimplex(NSimplex<2>* copy_from, int omit_index)
			{
				m_Vertices[0] = copy_from->m_Vertices[(omit_index == 0 ? 1 : 0)];
			}

		protected:
			/// <summary>
			/// Constructs a new simplex with the vertex added.
			/// </summary>
			/// <param name="vertex">The vertex to add.</param>
			/// <param name="new_simplex">Outputs the expanded simplex.</param>
			void __expand(const vec3f& vertex, Simplex*& new_simplex)
			{
				if (vec3f(vertex - m_Vertices[0]).square_sum() < epsilon)
				{
					new_simplex = this;
					return;
				}

				new_simplex = new NSimplex<2>(this, vertex);
			}

			/// <summary>
			/// Constructs the sub-simplex that is closest to the origin.
			/// </summary>
			/// <param name="new_simplex">Outputs the sub-simplex that is closest to the origin.</param>
			/// <param name="dir">Outputs the smallest direction vector to the origin normal to the sub-simplex.</param>
			void __prune(Simplex*& new_simplex, vec3f& dir)
			{
				new_simplex = this;
				dir = -1.f * m_Vertices[0];
			}

		public:
			/// <summary>
			/// Constructs a simplex composed of a single vertex.
			/// </summary>
			/// <param name="vertex">The sole vertex of the simplex.</param>
			NSimplex(const vec3f& vertex)
			{
				m_Vertices[0] = vertex;
			}

			/// <summary>
			/// Retrieves the number of vertices in the simplex.
			/// </summary>
			/// <returns>The number of vertices in the simplex.</returns>
			Int count()
			{
				return 1;
			}
		};

		template <>
		void NSimplex<2>::__prune(Simplex*& new_simplex, vec3f& dir)
		{
			// Project the origin onto the line formed by the vertices
			vec3f s_diff = m_Vertices[1] - m_Vertices[0];

			Float numer[2];
			numer[0] = s_diff.dot(m_Vertices[1]);
			numer[1] = s_diff.dot(m_Vertices[0]);
			Float denom = 1.f / s_diff.square_sum();

			// The projected origin can be calculated as ((numer[0] * m_Vertices[0]) - (numer[1] * m_Vertices[1])) * denom
			// The statement (numer[0] - numer[1]) * denom = 1 will always be true.
			// This projection lies on the line segment formed by the two vertices if and only if numer[0] > 0 and numer[1] < 0

			if (numer[0] >= 0.f && numer[1] <= 0.f)
			{
				// The projected origin lies between the two vertices
				new_simplex = this;
				dir = ((m_Vertices[1] * numer[1]) - (m_Vertices[0] * numer[0])) * denom;
			}
			else
			{
				// The projected origin lies outside the line segment formed by the two vertices, so delete the vertex farther from the origin
				if (numer[0] < 0.f)
				{
					// Origin is closer to m_Vertices[1]
					new_simplex = new NSimplex<1>(this, 0);
					dir = -1.f * m_Vertices[1];
				}
				else
				{
					// Origin is closer to m_Vertices[0]
					new_simplex = new NSimplex<1>(this, 1);
					dir = -1.f * m_Vertices[0];
				}
			}
		}

		template <>
		void NSimplex<3>::__prune(Simplex*& new_simplex, vec3f& dir)
		{
			// Calculate the two vectors that constitute the basis for the plane
			vec3f d[2] = { m_Vertices[1] - m_Vertices[0], m_Vertices[2] - m_Vertices[0] };

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
			vec3f o = normal * normal.dot(m_Vertices[0]) / normal.square_sum();

			// Calculate the barycentric coordinates of the origin projection, with lambda_0 = 1
			vec2f numer = adj * (o - m_Vertices[0]);
			Float denom = (d00 * d11) - (d01 * d01);

			if (std::abs(denom) >= epsilon)
			{
				vec3f lambda;
				lambda(0) = 1.f;
				lambda(1) = numer.get(0) / denom;
				lambda(2) = numer.get(1) / denom;

				if (lambda_all_positive(lambda))
				{
					// The projected origin lies within the triangle
					new_simplex = this;
					dir = -1.f * o;
				}
				else
				{
					// Delete one or more vertices
					for (int c = 2; c > 0; --c)
					{
						if (lambda.get(c) < 0.f)
						{
							new_simplex = new NSimplex<2>(this, c);
							dir = Simplex::prune(&new_simplex);
							break;
						}
					}
				}
			}
			else
			{
				/// If this is true, then the simplex is actually a line, not a triangle.
				/// That means the shortest vector in the last iteration was from a point on that line
				/// and, since the farthest point (now at index [0]) calculated was also on that line,
				/// that means that the "farthest point" was actually an arbitrary selection, 
				/// because any point on that line would have had the same dot product with that vector.
				/// Therefore, there are only three cases:
				/// 1. The line does not intersect with the origin, in which case that aforementioned dot 
				/// product will always be negative and the algorithm will terminate soon.
				/// 2. The original line segment intersected with the origin, in which case the algorithm 
				/// should have already terminated. So reaching this case is impossible.
				/// 3. The line intersects with the origin, but the original line segment did not. But that
				/// implies the projected origin onto that line was outside the line segment, which in turn
				/// implies that one of those vertices should have been removed from the simplex. So reaching
				/// this case is impossible.
				/// In summary, we only need to consider the first case. Furthermore, we can narrow our 
				/// scope to subsets of the first case where the projected origin onto the line lies in the
				/// original line segment, because otherwise a vertex would have been deleted.
				/// Since the algorithm from this point only needs the shortest vector from the line to the
				/// origin, we can simply prune the simplex back to the original line segment and return the
				/// prior vector.
				new_simplex = new NSimplex<2>(this, 0);
				dir = Simplex::prune(&new_simplex);
			}
		}

		template <>
		struct NSimplex<4> : public Simplex
		{
		private:
			friend class NSimplex<3>;

			vec3f m_Vertices[4];

			NSimplex(NSimplex<3>* copy_from, const vec3f& addition)
			{
				for (int k = 2; k >= 0; --k)
					m_Vertices[k] = copy_from->m_Vertices[k];
				m_Vertices[3] = addition;
			}

		protected:
			/// <summary>
			/// Does nothing.
			/// </summary>
			/// <param name="vertex">The vertex to add.</param>
			/// <param name="new_simplex">Outputs the expanded simplex.</param>
			void __expand(const vec3f& vertex, Simplex*& new_simplex)
			{
				new_simplex = this;
			}

			/// <summary>
			/// Constructs the sub-simplex that is closest to the origin.
			/// </summary>
			/// <param name="new_simplex">Outputs the sub-simplex that is closest to the origin.</param>
			/// <param name="dir">Outputs the smallest direction vector to the origin normal to the sub-simplex.</param>
			void __prune(Simplex*& new_simplex, vec3f& dir)
			{
				// Construct a barycentric coordinate system
				vec3f s_diff[3];
				for (int c = 3; c > 0; --c)
					s_diff[c - 1] = m_Vertices[c] - m_Vertices[0];

				// Calculate the barycentric coordinates of the origin
				vec3f o = -1.f * m_Vertices[0];

				vec3f cross[3];
				o.cross(s_diff[1], cross[0]);
				s_diff[0].cross(o, cross[1]);
				s_diff[0].cross(s_diff[1], cross[2]);

				Float denom = cross[2].dot(s_diff[2]);
				if (std::abs(denom) >= epsilon) // Denom is non-zero
				{
					// The three vectors form a basis for R^3
					vec4f lambda;
					lambda(0) = 1.f;
					lambda(1) = cross[0].dot(s_diff[2]) / denom;
					lambda(2) = cross[1].dot(s_diff[2]) / denom;
					lambda(3) = cross[2].dot(o) / denom;

					if (lambda_all_positive(lambda))
					{
						// The origin lies within the tetrahedron
						new_simplex = this;
						dir = vec3f(0.f, 0.f, 0.f);
					}
					else
					{
						// Delete one or more vertices
						for (int c = 3; c > 0; --c)
						{
							if (lambda.get(c) < 0.f)
							{
								new_simplex = new NSimplex<3>(this, c);
								dir = Simplex::prune(&new_simplex);
								break;
							}
						}
					}
				}
				else
				{
					/// This case only occurs if the newest vertex is on the same plane as the prior simplex.
					/// TODO come up with justification for following same procedure as NSimplex<3>::__prune
					new_simplex = new NSimplex<3>(this, 0);
					dir = Simplex::prune(&new_simplex);
				}
			}

		public:
			/// <summary>
			/// Retrieves the number of vertices in the simplex.
			/// </summary>
			/// <returns>The number of vertices in the simplex.</returns>
			Int count()
			{
				return 4;
			}
		};



		/// <summary>
		/// Calculates the minimum distance to the other shape.
		/// </summary>
		/// <param name="other">The shape to calculate the distance from.</param>
		/// <returns>The minimum distance to the other shape.</returns>
		Int Shape::get_distance(const Shape* other) const
		{
			static constexpr Float inv_distance_scale = 1.f / ONION_WORLD_GEOMETRY_SCALE;

			// Generate an initial guess
			vec3f d = ONION_WORLD_GEOMETRY_SCALE * (other->get_position() - get_position());
			vec3f a = support(d) - other->support(-1.f * d);
			d = -1.f * a;
			
			Simplex* s = new NSimplex<1>(a);

			volatile int num_iterations = 0;

			// Begin iteration
			while (num_iterations++ >= 0)
			{
				if (num_iterations > 100)
				{
					volatile int something_has_gone_very_wrong = 1;
				}

				// Calculate the farthest point on the Minkowski difference in the direction of the shortest-found vector from the Minkowski difference to the origin
				a = support(d) - other->support(-1.f * d);
				bool obtuse = (a.dot(d) < 0.f); // The dot product of a and d is negative if the angle is obtuse

				// Expand the simplex to include this new point (ignoring duplicates)
				Simplex::expand(&s, a);

				// Calculate shortest vector from simplex to origin, and simplify the simplex
				d = Simplex::prune(&s);

				// Calculate the (squared) shortest distance found from Minkowski difference to origin
				Float distance = d.square_sum();

				// If the angle between direction vector to origin and farthest point on Minkowski difference in that direction is obtuse, 
				// then the Minkowski difference does not contain the origin and the two shapes do not intersect
				if (obtuse) 
				{
					// Clean up simplex
					delete s;

					// Calculate minimum distance to origin, then terminate algorithm
					return (Int)roundf(sqrtf(distance) * inv_distance_scale);
				}
				else if (distance < Simplex::epsilon)
				{
					// Clean up simplex
					delete s;

					// Minkowski difference contains origin, so distance between shapes is 0.
					return 0;
				}
			}
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
			vec3i res = m_Position;

			for (int c = 1; c >= 0; --c)
			{
				if (dir.dot(m_Radii[c]) > 0)
					res += m_Radii[c];
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