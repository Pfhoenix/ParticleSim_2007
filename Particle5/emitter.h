class Emitter : public GenericObject
{
	float Wavelength;
	float Frequency;
	float OutputRate;

	bool bOn;
	float Timer;

	Emitter();

	virtual void Tick(float, float);
	virtual void Render(void);

	~Emitter();
};
