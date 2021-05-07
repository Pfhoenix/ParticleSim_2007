class CollisionBox : public CollisionBody
{
public:

	Vector3D<float> Dimensions;

	CollisionBox();
	CollisionBox(const Vector3D<float>&);

	virtual Vector3D<float> GetPointClosestTo(const Vector3D<float>&);
	virtual Vector3D<float> GetPointFurthestFrom(const Vector3D<float>&);
	virtual bool IsWithin(const Vector3D<float>&);

	~CollisionBox();
};
