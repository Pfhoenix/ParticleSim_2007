class RigidBody
{
public:

	GenericObject *ParentObject;

	float Mass;
	Vector3D<float> CoM;
	D3DXMATRIX MoI;
	Vector3D<float> Location;
	Vector3D<float> LastLocation;
	D3DXQUATERNION Rotation;
	Vector3D<float> Xaxis;
	Vector3D<float> Yaxis;
	Vector3D<float> Zaxis;
	Vector3D<float> AngularVelAxis;
	float AngularVelScalar;
	float Restitution;

	RB_FIXED_TYPE FixedType;
	Vector3D<float> FixedRotRate;

	float ColSphereRadius;
	//vector<NewtonC
	vector<CollisionBody*> ColBodies;

	vector<Vector3D<float> > ForceLocs;
	vector<Vector3D<float> > ForceVects;

	RigidBody(float = 1.0f);

	void AddCollisionBody(CollisionBody*, const Vector3D<float>&, bool, const D3DXQUATERNION&);
	void RemoveCollisionBody(CollisionBody*);

	void SetRotation(const D3DXQUATERNION&);

	void SetFixedType(void);
	void SetFixedType(const Vector3D<float>&);
	void SetFixedType(const D3DXQUATERNION&);
	void SetFixedType(const Vector3D<float>&, const D3DXQUATERNION&);
	void SetFixedType(const Vector3D<float>&, const Vector3D<float>&);

	void ApplyImpulse(Vector3D<float>, Vector3D<float>);

	void QueueForce(const Vector3D<float>&, const Vector3D<float>&);

	void ProcessPhysics(float, float);

	bool IsWithin(const Vector3D<float>&);

	~RigidBody();
};
