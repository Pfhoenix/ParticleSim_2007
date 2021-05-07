class GenericObject
{
public:

	NxActor *Actor;

	bool bUserHeld;

	GenericObject();

	virtual void Init(float);
	virtual void Tick(float, float);
	virtual void Render(void);

	void SetFixed(int);

	~GenericObject();
};
