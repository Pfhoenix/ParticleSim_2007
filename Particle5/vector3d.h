template <class T>
class Vector3D
{
	public:

		T X, Y, Z;

		Vector3D<T>(void)
		{
			X = (T) 0;
			Y = (T) 0;
			Z = (T) 0;

			return;
		}

		Vector3D<T>(T cv)
		{
			SetValue(cv, cv, cv);

			return;
		}

		Vector3D<T>(T nX, T nY, T nZ)
		{
			SetValue(nX, nY, nZ);

			return;
		}

		Vector3D<T>(const Vector3D<T> &v)
		{
			X = v.X;
			Y = v.Y;
			Z = v.Z;

			return;
		}

		void SetValue(T nX, T nY, T nZ)
		{
			X = nX;
			Y = nY;
			Z = nZ;

			return;
		}

		T Length(void)
		{
			return (T) (sqrt(X * X + Y * Y + Z * Z));
		}

		T LengthSquared(void)
		{
			return (T) (X * X + Y * Y + Z * Z);
		}

		Vector3D<T> Normal(void)
		{
			T f;
			Vector3D<T> nv(X, Y, Z);

			f = Length();

			if (f != (T) 0) nv *= ((T) 1 / f);

			return nv;
		}

		void SetLength(T l)
		{
			if (l == (T) 0) return;

			T f = Length();
			if (f == (T) 0) return;

			X *= (l / f);
			Y *= (l / f);
			Z *= (l / f);

			return;
		}


		void SetRand(bool bUnit = true)
		{
			X = mt.rand(2001) / (T) 1000 - (T) 1;
			Y = mt.rand(2001) / (T) 1000 - (T) 1;
			Z = mt.rand(2001) / (T) 1000 - (T) 1;

			if (bUnit)
			{
				Vector3D<T> v = Normal();
				SetValue(v.X, v.Y, v.Z);
			}

			return;
		}

		Vector3D<int> ConvertToInt(void) const
		{
			return Vector3D<int>((int) X, (int) Y, (int) Z);
		}

		Vector3D<float> ConvertToFloat(void) const
		{
			return Vector3D<float>((float) X, (float) Y, (float) Z);
		}

		Vector3D<double> ConvertToDouble(void) const
		{
			return Vector3D<double>((double) X, (double) Y, (double) Z);
		}

		D3DXVECTOR3 ConvertToD3DXVector3(void) const
		{
			D3DXVECTOR3 v;

			v.x = (float) X;
			v.y = (float) Y;
			v.z = (float) Z;

			return v;
		}

		Vector3D<T> operator - (void) const
		{
			return Vector3D<T>(-X, -Y, -Z);
		}

		bool operator == (const Vector3D<T> &V) const
		{
			return ((X == V.X) && (Y == V.Y) && (Z == V.Z));
		}

		bool operator != (const Vector3D<T> &V) const
		{
			return ((X != V.X) || (Y != V.Y) || (Z != V.Z));
		}

		Vector3D<T> operator + (const Vector3D<T> &V) const
		{
			return Vector3D<T>(X + V.X, Y + V.Y, Z + V.Z);
		}

		Vector3D<T> operator - (const Vector3D<T> &V) const
		{
			return Vector3D<T>(X - V.X, Y - V.Y, Z - V.Z);
		}

		Vector3D<T> operator * (int i) const
		{
			return Vector3D<T>((T)(X * i), (T)(Y * i), (T)(Z * i));
		}

		Vector3D<T> operator * (float f) const
		{
			return Vector3D<T>((T)(X * f), (T)(Y * f), (T)(Z * f));
		}

		Vector3D<T> operator * (double d) const
		{
			return Vector3D<T>((T)(X * d), (T)(Y * d), (T)(Z * d));
		}

		Vector3D<T> operator * (const Vector3D<T> &v) const
		{
			return Vector3D<T>(X * v.X, Y * v.Y, Z * v.Z);
		}

		Vector3D<T> operator / (int i) const
		{
			return Vector3D<T>((T)(X / i), (T)(Y / i), (T)(Z / i));
		}

		Vector3D<T> operator / (float f) const
		{
			return Vector3D<T>((T)(X / f), (T)(Y / f), (T)(Z / f));
		}

		Vector3D<T> operator / (double d) const
		{
			return Vector3D<T>((T)(X / d), (T)(Y / d), (T)(Z / d));
		}

		Vector3D<T> operator / (const Vector3D<T> &v) const
		{
			return Vector3D<T>(X / v.X, Y / v.Y, Z / v.Z);
		}

		void operator -= (const Vector3D<T> &v)
		{
			X -= v.X;
			Y -= v.Y;
			Z -= v.Z;

			return;
		}

		void operator += (const Vector3D<T> &v)
		{
			X += v.X;
			Y += v.Y;
			Z += v.Z;

			return;
		}

		void operator *= (int i)
		{
			X *= i;
			Y *= i;
			Z *= i;

			return;
		}

		void operator *= (float f)
		{
			X *= f;
			Y *= f;
			Z *= f;

			return;
		}

		void operator *= (double d)
		{
			X *= d;
			Y *= d;
			Z *= d;

			return;
		}

		void operator *= (const Vector3D<T> &v)
		{
			X *= v.X;
			Y *= v.Y;
			Z *= v.Z;

			return;
		}

		void operator /= (int i)
		{
			X /= i;
			Y /= i;
			Z /= i;

			return;
		}

		void operator /= (float f)
		{
			X /= f;
			Y /= f;
			Z /= f;

			return;
		}

		void operator /= (double d)
		{
			X /= d;
			Y /= d;
			Z /= d;

			return;
		}

		void operator /= (const Vector3D<T> &v)
		{
			X /= v.X;
			Y /= v.Y;
			Z /= v.Z;

			return;
		}

		T operator | (const Vector3D<T> &v) const
		{
			return (X * v.X + Y * v.Y + Z * v.Z);
		}

		Vector3D<T> operator ^ (const Vector3D<T> &V) const
		{
			Vector3D<T> v;

			v.X = (Y * V.Z) - (Z * V.Y);
			v.Y = (Z * V.X) - (X * V.Z);
			v.Z = (X * V.Y) - (Y * V.X);

			return v;
		}

		struct compare
		{
			bool operator () (const Vector3D<T> &v, const Vector3D<T> &V) const
			{
				if (v.X > V.X) return true;
				else if (v.X < V.X) return false;
				else if (v.Y > V.Y) return true;
				else if (v.Y < V.Y) return false;
				else if (v.Z > V.Z) return true;
				else if (v.Z < V.Z) return false;
				else return false;
			}
		};
};
