#include "includes.h"

Camera::Camera()
{
	//CameraRot.SetValue(0.0f, PI / 2.0f, 0.0f);
	ScaleVec.SetValue(1.0f, 1.0f, 1.0f);
	DesiredScale.SetValue(1.0f, 1.0f, 1.0f);
	StartScale.SetValue(1.0f, 1.0f, 1.0f);
	bScaling = false;
	ScaleTime = 1.1f;
	ScaleRate = 1.0f;

	return;
}

// this doesn't call UpdateView so that scaling isn't applied and we don't interfere with camera information
void Camera::SetDefaultView(bool bHUD)
{
	Vector3D<float> cr(0.0f, PI / 2.0f, 0.0f);
	Vector3D<float> cl;

	if (bHUD) cl.SetValue(RenderArea->Viewport.Width * -0.5f, 0.0f, RenderArea->Viewport.Height * -0.5f);

	D3DXQUATERNION q;
	D3DXVECTOR3 t(cl.X, cl.Y, cl.Z);
	D3DXVECTOR3 s(1.0f, 1.0f, 1.0f);
	D3DXVECTOR3 rc(cl.X, cl.Y, cl.Z);
	D3DXQuaternionRotationYawPitchRoll(&q, (float) cr.X, (float) cr.Y, (float) cr.Z);
	D3DXMatrixTransformation(&ViewMatrix, NULL, NULL, &s, &rc, &q, &t);
	RenderArea->MainSurface->SetTransform(D3DTS_VIEW, &ViewMatrix);

	return;
}

void Camera::UpdateView(void)
{
	D3DXQUATERNION q;
	D3DXVECTOR3 t((float) (-CameraLoc.X * ScaleVec.X), (float) (-CameraLoc.Y * ScaleVec.Y), (float) (CameraLoc.Z * ScaleVec.Z));
	D3DXVECTOR3 s((float) ScaleVec.X, (float) ScaleVec.Y, (float) ScaleVec.Z);
	D3DXVECTOR3 rc((float) (CameraLoc.X * ScaleVec.X), (float) (CameraLoc.Y * ScaleVec.Y), (float) (CameraLoc.Z * ScaleVec.Z));
	D3DXQuaternionRotationYawPitchRoll(&q, (float) CameraRot.X, (float) CameraRot.Y, (float) CameraRot.Z);
	D3DXMatrixTransformation(&ViewMatrix, NULL, NULL, &s, &rc, &q, &t);
	//RenderArea->MainSurface->SetTransform(D3DTS_VIEW, &ViewMatrix);

	return;
}

void Camera::ZoomIn(void)
{
	ScaleTime = 0.0f;
	StartScale = ScaleVec;
	DesiredScale *= 1.25f;
	bScaling = true;

	return;
}

void Camera::ZoomOut(void)
{
	ScaleTime = 0.0f;
	StartScale = ScaleVec;
	DesiredScale /= 1.25f;
	bScaling = true;

	return;
}

void Camera::ResetLocation(void)
{
	CameraLoc *= 0.0f;

	return;
}

void Camera::Tick(float DeltaTime)
{
	if (DeltaTime < 0.0f) return;

	// handle scaling of zoom
	if (bScaling)
	{
		ScaleTime += DeltaTime / ScaleRate;
		if (ScaleTime >= 1.0f)
		{
			bScaling = false;
			ScaleVec = DesiredScale;
		}
		else
		{
			Interpolate(&ScaleVec, ScaleTime);
			ScaleVec = (DesiredScale - StartScale) * ScaleVec + StartScale;
		}
	}

	UpdateView();

	return;
}

Camera::~Camera()
{
	return;
}

