class Vector2D
{
	public:

		double X, Y;

		Vector2D(void);
		Vector2D(double, double);

		void SetValue(double, double);
		double Length(void);
		Vector2D Normal(void);

		bool operator == (const Vector2D);
		bool operator != (const Vector2D);
		Vector2D operator + (const Vector2D);
		Vector2D operator - (const Vector2D);
		Vector2D operator * (int);
		Vector2D operator * (float);
		Vector2D operator * (double);
		Vector2D operator / (int);
		Vector2D operator / (float);
		Vector2D operator / (double);
		void operator -= (const Vector2D);
		void operator += (const Vector2D);
		void operator *= (int);
		void operator *= (float);
		void operator *= (double);
		void operator /= (int);
		void operator /= (float);
		void operator /= (double);

		~Vector2D(void);
};
