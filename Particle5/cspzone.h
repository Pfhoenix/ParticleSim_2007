class CSPZone
{
public:

	Vector3D<int> ID;
	vector<GenericObject*> Objects;	// all objects that fall within this node

	CSPZone();

	void Add(GenericObject*);
	void Remove(GenericObject*);

	~CSPZone();
};
