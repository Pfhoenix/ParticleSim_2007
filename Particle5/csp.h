class CSP
{
public:

	typedef	map<Vector3D<int>,CSPZone*,Vector3D<int>::compare> ZoneHash;
	ZoneHash Zones;

	typedef map<GenericObject*,vector<CSPZone*> > ObjectHash;
	ObjectHash Objects;

	Vector3D<int> LowExtent, HighExtent;	// to know the extent in space in which we have zones

	CSP();

	void UpdateExtents(const Vector3D<int>&);

	CSPZone* CreateZone(const Vector3D<int>&);

	bool ZoneIDExtentCheck(const Vector3D<int>&);
	Vector3D<int> GetZoneIDIn(const Vector3D<float>&);
	CSPZone* FindZone(const Vector3D<int>&, bool = true);	// parameter is a zone ID
	CSPZone* GetZoneIn(const Vector3D<float>&, bool = true);	// finds the CSPZone that the parameter world coordinate falls in
	vector<CSPZone*> GetZonesIn(const Vector3D<float>&, const Vector3D<float>&, bool);
	vector<CSPZone*> GetZonesIn(GenericObject*);

	void Add(GenericObject*);
	void Move(GenericObject*, const Vector3D<float>&);
	void Remove(GenericObject*);

	bool IsWithin(const Vector3D<float>&, const Vector3D<float>&, const Vector3D<float>&);
	//bool IsWithinObject(const Vector3D<float>&, GenericObject*);
	bool IsWithinZone(const Vector3D<float>&, CSPZone*);
	bool OverlapCheck(const Vector3D<float>&, const Vector3D<float>&, const Vector3D<float>&, const Vector3D<float>&);

	vector<GenericObject*> GetWithinRadius(const Vector3D<float>&, float);

	Vector3D<float> GetClosestPoint(const Vector3D<float>&, const Vector3D<float>&, const Vector3D<float>&);
	Vector3D<float> GetExitPoint(const Vector3D<float>&, const Vector3D<float>&, const Vector3D<int>&);

	GenericObject* GetObjectUnder(const Vector3D<float>&);

	void ClearAll(void);

	~CSP();
};
