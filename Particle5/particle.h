class Particle : public ChargedObject
{
public:

	Texture *T;
	float Radius;

	Particle();

	virtual void Init(float, float);
	virtual void Render(void);

	virtual void AffectOther(ChargedObject*);

	~Particle();
};
