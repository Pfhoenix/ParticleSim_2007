class PhysX
{
public:

	NxPhysicsSDK *gPhysics;
	NxScene *gScene;
	NxActor *Selected;
	NxVec3 SelectedLastPos;

	NxActor *BoundingPlanes[4];
	
	PhysX();

	void ConvertMatrix(const NxMat34&, D3DXMATRIX*);

	void InitSceneBounds(void);
	void Simulate(float);
	void GetPhysicsResults(void);

	NxActor* CreatePlaneActor(NxVec3, float);
	NxActor* CreateSphereActor(NxVec3, float, float);
	NxActor* CreateComplexActor(NxVec3, float, vector<NxShapeDesc*>);

	NxActor* FindActorUnderMouse(const NxVec3&, const NxVec3&);
	bool PickActor(const NxVec3&, const NxVec3&);
	void LetGoActor(void);

	~PhysX();
};
