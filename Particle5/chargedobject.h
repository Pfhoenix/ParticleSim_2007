class ChargedObject : public GenericObject
{
public:

	float Charge;

	ChargedObject();

	virtual void Tick(float, float);

	void SingularAffectedBy(const NxVec3&, const NxVec3&, float, float);
	virtual void GetAffectedBy(const NxVec3&, float);
	virtual void AffectOther(ChargedObject*);

	~ChargedObject();
};
