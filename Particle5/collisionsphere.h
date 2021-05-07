class CollisionSphere : public CollisionBody
{
public:

	float Radius;

	CollisionSphere();
	CollisionSphere(float);

	virtual Vector3D<float> GetPointClosestTo(const Vector3D<float>&);
	virtual Vector3D<float> GetPointFurthestFrom(const Vector3D<float>&);
	virtual bool IsWithin(const Vector3D<float>&);

	~CollisionSphere();
};
