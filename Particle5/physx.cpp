#include "includes.h"

PhysX::PhysX()
{
	gPhysics = NULL;
	gScene = NULL;
	Selected = NULL;

	gPhysics = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
	if (!gPhysics)
	{
		Log("PhysX failed to create!");
		return;
	}

	//gPhysics->setParameter(NX_CONTINUOUS_CD, true);
	//gPhysics->setParameter(NX_CCD_EPSILON, 0.01f);
	gPhysics->setParameter(NX_SKIN_WIDTH, 0.01f);

	NxSceneDesc sceneDesc;
 	sceneDesc.simType = NX_SIMULATION_HW;
	//sceneDesc.gravity.set(0.0f, 9.8f, 0.0f);
    gScene = gPhysics->createScene(sceneDesc);	
	if (!gScene)
	{
		sceneDesc.simType = NX_SIMULATION_SW; 
		gScene = gPhysics->createScene(sceneDesc);  
		if(!gScene)
		{
			gPhysics->release();
			gPhysics = NULL;
			Log("PhysX failed to initialize scene!");
			return;
		}
		Log("PhysX running in software mode.");
	}
	else { Log("PhysX running in hardware mode."); }

	NxMaterial* defaultMaterial = gScene->getMaterialFromIndex(0); 
	defaultMaterial->setRestitution(0.5f);
	defaultMaterial->setStaticFriction(0.5f);
	defaultMaterial->setDynamicFriction(0.5f);

	return;
}

void PhysX::ConvertMatrix(const NxMat34 &nm, D3DXMATRIX *m)
{
	nm.getColumnMajor44(&(m->_11));

	return;
}

void PhysX::InitSceneBounds(void)
{
	BoundingPlanes[0] = CreatePlaneActor(NxVec3(0.0f, -1.0f, 0.0f), RenderArea->Viewport.Height * -0.5f);
	BoundingPlanes[1] = CreatePlaneActor(NxVec3(0.0f, 1.0f, 0.0f), RenderArea->Viewport.Height * -0.5f);
	BoundingPlanes[2] = CreatePlaneActor(NxVec3(-1.0f, 0.0f, 0.0f), RenderArea->Viewport.Width * -0.5f);
	BoundingPlanes[3] = CreatePlaneActor(NxVec3(1.0f, 0.0f, 0.0f), RenderArea->Viewport.Width * -0.5f);

	return;
}

void PhysX::Simulate(float DeltaTime)
{
	/*if (Selected)
	{
		// store current global position
		SelectedLastPos = Selected->getGlobalPosition();
		Log("before = " << SelectedLastPos.x << "," << SelectedLastPos.y << "," << SelectedLastPos.z);
		// Mouse3D says where Selected should move to
		Selected->moveGlobalPosition(Mouse3D);
	}*/
	gScene->simulate(DeltaTime);
	gScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
	gScene->flushStream();
	/*if (Selected)
	{
		NxVec3 v = Selected->getGlobalPosition();
		Log("after = " << v.x << "," << v.y << "," << v.z);
	}*/

	return;
}

void PhysX::GetPhysicsResults(void)
{
	while (!gScene->fetchResults(NX_RIGID_BODY_FINISHED, false));

	return;
}

NxActor* PhysX::CreatePlaneActor(NxVec3 n, float d)
{
	NxPlaneShapeDesc *planeDesc = new NxPlaneShapeDesc;
	planeDesc->normal = n;
	planeDesc->d = d;
	NxActorDesc *actorDesc = new NxActorDesc;
	actorDesc->shapes.pushBack(planeDesc);

	return gScene->createActor(*actorDesc);
}

NxActor* PhysX::CreateSphereActor(NxVec3 l, float r, float d)
{
	NxSphereShapeDesc sphere;
	sphere.radius = r;
	NxActorDesc actorDesc;
	actorDesc.shapes.push_back(&sphere);
	NxBodyDesc bodyDesc;
	actorDesc.body = &bodyDesc;
	actorDesc.density = d;
	actorDesc.globalPose.t = l;

	return gScene->createActor(actorDesc);
}

NxActor* PhysX::CreateComplexActor(NxVec3 l, float d, vector<NxShapeDesc*> shapes)
{
	NxActorDesc actorDesc;
	for (int i = 0; i < (int) shapes.size(); i++)
		actorDesc.shapes.push_back(&(*shapes[i]));
	actorDesc.body = new NxBodyDesc;
	actorDesc.density = d;
	actorDesc.globalPose.t = l;

	return gScene->createActor(actorDesc);
}

NxActor* PhysX::FindActorUnderMouse(const NxVec3 &RayO, const NxVec3 &RayD)
{
	NxRay ray;
	ray.orig = RayO;
	ray.dir = RayD;
	NxRaycastHit hit;
	NxShape* closestShape = gScene->raycastClosestShape(ray, NX_ALL_SHAPES, hit);
	// found none
	if (!closestShape) return NULL;
	// found one of the bounding planes
	for (int i = 0; i < 4; i++)
		if (BoundingPlanes[i] == &closestShape->getActor()) return NULL;

	return &closestShape->getActor();
}

bool PhysX::PickActor(const NxVec3 &RayO, const NxVec3 &RayD)
{
	if (Selected) LetGoActor();

	Selected = FindActorUnderMouse(RayO, RayD);
	if (!Selected) return false;
	// found a static geometry actor
	if (!Selected->isDynamic())
	{
		Selected = NULL;
		return false;
	}

	Selected->raiseBodyFlag(NX_BF_KINEMATIC);
	HeldObject = (GenericObject*) Selected->userData;
	HeldObject->bUserHeld = true;

	return true;
}

void PhysX::LetGoActor(void)
{
	if (Selected)
	{
		Selected->clearBodyFlag(NX_BF_KINEMATIC);
		//Selected->addLocalForce(Selected->getGlobalPosition() - SelectedLastPos, NX_IMPULSE, true);
		HeldObject->bUserHeld = false;
		HeldObject = NULL;
		Selected = NULL;
	}

	return;
}

PhysX::~PhysX()
{
	Selected = NULL;
//	GetPhysicsResults();

	for (int i = 0; i < 4; i++) BoundingPlanes[i] = NULL;

	if (gScene) gPhysics->releaseScene(*gScene);
	gScene = NULL;

	gPhysics->release();
	gPhysics = NULL;

	return;
}
