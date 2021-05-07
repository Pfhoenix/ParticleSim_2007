class Camera
{
public:

	Vector3D<float> CameraLoc;
	Vector3D<float> CameraRot;
	Vector3D<float> ScaleVec;
	Vector3D<float> DesiredScale;
	Vector3D<float> StartScale;
	bool bScaling;
	float ScaleTime;
	float ScaleRate;

	Camera();

	void SetDefaultView(bool = false);

	void UpdateView(void);

	void ZoomIn(void);
	void ZoomOut(void);

	void ResetLocation(void);

	void Tick(float);

	~Camera();
};
