class CollisionBody
{
public:

	RigidBody *Parent;
	Vector3D<float> LocationOffset;
	D3DXQUATERNION RotationOffset;
	Vector3D<float> Xaxis;
	Vector3D<float> Yaxis;
	Vector3D<float> Zaxis;

	bool bParentRotation;

	CollisionBody();

	void SetParentRotation(bool);

	Vector3D<float> GetWorldLocation(void);

	virtual Vector3D<float> GetPointClosestTo(const Vector3D<float>&);
	virtual Vector3D<float> GetPointFurthestFrom(const Vector3D<float>&);
	virtual bool IsWithin(const Vector3D<float>&);
	void UpdateRotation(void);

	~CollisionBody();
};
