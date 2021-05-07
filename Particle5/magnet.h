class Magnet : public ChargedObject
{
public:

	Magnet();

	virtual void Render(void);

	virtual void GetAffectedBy(const NxVec3&, float);
	virtual void AffectOther(ChargedObject*);

	~Magnet();
};
