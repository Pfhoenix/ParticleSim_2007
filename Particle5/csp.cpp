#include "includes.h"
#define SPACE_ZONE_EXTENT 50

CSP::CSP()
{
	return;
}

void CSP::UpdateExtents(const Vector3D<int> &n)
{
	// check if the lower extent gets pushed lower
	if (LowExtent.X > n.X) LowExtent.X = n.X;
	if (LowExtent.Y > n.Y) LowExtent.Y = n.Y;
	if (LowExtent.Z > n.Z) LowExtent.Z = n.Z;

	// check if the higher extent gets pushed higher
	if (HighExtent.X < n.X) HighExtent.X = n.X;
	if (HighExtent.Y < n.Y) HighExtent.Y = n.Y;
	if (HighExtent.Z < n.Z) HighExtent.Z = n.Z;

	return;
}

CSPZone* CSP::CreateZone(const Vector3D<int> &n)
{
	CSPZone *z = new CSPZone;

	if (!z) return NULL;

	z->ID = n;
	Zones.insert(ZoneHash::value_type(n, z));
	UpdateExtents(n);

	return z;
}

bool CSP::ZoneIDExtentCheck(const Vector3D<int> &ID)
{
	if (ID.X < LowExtent.X) return false;
	if (ID.X > HighExtent.X) return false;
	if (ID.Y < LowExtent.Y) return false;
	if (ID.Y > HighExtent.Y) return false;
	if (ID.Z < LowExtent.Z) return false;
	if (ID.Z > HighExtent.Z) return false;

	return true;
}

CSPZone* CSP::FindZone(const Vector3D<int> &ID, bool bCreate)
{
	if (Zones.end() != Zones.find(ID)) return Zones[ID];
	else if (bCreate) return CreateZone(ID);

	return NULL;
}

Vector3D<int> CSP::GetZoneIDIn(const Vector3D<float> &v)
{
	Vector3D<int> vi = v.ConvertToInt();

	vi /= SPACE_ZONE_EXTENT;

	return vi;
}

CSPZone* CSP::GetZoneIn(const Vector3D<float> &v, bool bCreate)
{
	return FindZone(GetZoneIDIn(v), bCreate);
}

vector<CSPZone*> CSP::GetZonesIn(const Vector3D<float> &loc, const Vector3D<float> &dim, bool bCreate)
{
	vector<CSPZone*> zones;
	CSPZone *z;

	Vector3D<int> s, e;

	s = (loc - dim * 0.5f).ConvertToInt() / SPACE_ZONE_EXTENT;
	e = (loc + dim * 0.5f).ConvertToInt() / SPACE_ZONE_EXTENT;

	for (int k = s.Z; k <= e.Z; k++)
	for (int j = s.Y; j <= e.Y; j++)
	for (int i = s.X; i <= e.X; i++)
	{
		z = FindZone(Vector3D<int>(i, j, k));
		if (z) zones.push_back(z);
		else if (bCreate)
		{
			z = CreateZone(Vector3D<int>(i, j, k));
			if (z) zones.push_back(z);
		}
	}

	return zones;
}

vector<CSPZone*> CSP::GetZonesIn(GenericObject *O)
{
	vector<CSPZone*> z;

	if (!O) return z;
	if (Objects.end() == Objects.find(O)) return z;

	return Objects[O];
}

void CSP::Add(GenericObject *O)
{
	// no object
	if (!O) return;
	// object already in hash
	if (Objects.end() != Objects.find(O)) return;

	NxVec3 nxv = O->Actor->getGlobalPosition();
	Vector3D<float> v(nxv.x, nxv.y, nxv.z);

	vector<CSPZone*> zones = GetZonesIn(v, O->ColSphereRadius * 2.0f, true);
	for (int i = 0; i < (int) zones.size(); i++) zones[i]->Add(O);
	Objects.insert(ObjectHash::value_type(O, zones));

	return;
}

void CSP::Move(GenericObject *O, const Vector3D<float> &dest)
{
	if (!O) return;

	vector<CSPZone*> sz = GetZonesIn(O);
//	O->RB->Location = dest;
	vector<CSPZone*> ez = GetZonesIn(dest, Vector3D<float>(O->ColSphereRadius * 2.0f), true);
	// scan each list for zones that are in both lists
	for (int j = 0; j < (int) ez.size(); j++)
		for (int i = 0; i < (int) sz.size(); i++)
		{
			// we have a match, so they don't need to be processed further on
			if (sz[i]->ID == ez[j]->ID)
			{
				vector<CSPZone*>::iterator it = sz.begin();
				it += i;
				sz.erase(it);
				it = ez.begin();
				it += j--;
				ez.erase(it);
				break;
			}
		}
	// the zones left in sz need to have C removed from them
	for (int i = 0; i < (int) sz.size(); i++) sz[i]->Remove(O);
	// the zones left in ez need to have C added to them
	for (int i = 0; i < (int) ez.size(); i++) ez[i]->Add(O);

	return;
}

void CSP::Remove(GenericObject *O)
{
	if (!O) return;

	vector<CSPZone*> zones = GetZonesIn(O);
	for (int i = 0; i < (int) zones.size(); i++) zones[i]->Remove(O);
	Objects.erase(O);

	return;
}

bool CSP::IsWithin(const Vector3D<float> &v, const Vector3D<float> &s, const Vector3D<float> &e)
{
	if (v.X < s.X) return false;
	if (v.X > e.X) return false;
	if (v.Y < s.Y) return false;
	if (v.Y > e.Y) return false;
	if (v.Z < s.Z) return false;
	if (v.Z > e.Z) return false;

	return true;
}

/*bool CSP::IsWithinObject(const Vector3D<float> &v, GenericObject *O)
{
	if (!O) return false;
	else return O->RB->IsWithin(v);
}*/

bool CSP::IsWithinZone(const Vector3D<float> &v, CSPZone *Z)
{
	Vector3D<float> s = Z->ID.ConvertToFloat() * SPACE_ZONE_EXTENT - SPACE_ZONE_EXTENT;
	Vector3D<float> e = s + SPACE_ZONE_EXTENT * 2;

	return IsWithin(v, s, e);
}

bool CSP::OverlapCheck(const Vector3D<float> &vs0, const Vector3D<float> &ve0, const Vector3D<float> &vs1, const Vector3D<float> &ve1)
{
	/*
		this method assumes that the corners of the respective bounding boxes are in the proper places
	*/

	if (vs0.X > ve1.X) return false;
	if (ve0.X < vs1.X) return false;
	if (vs0.Y > ve1.Y) return false;
	if (ve0.Y < vs1.Y) return false;
	if (vs0.Z > ve1.Z) return false;
	if (ve0.Z < vs1.Z) return false;

	return true;
}

vector<GenericObject*> CSP::GetWithinRadius(const Vector3D<float> &loc, float r)
{
	vector<GenericObject*> cs;

	CSPZone *tz = NULL, *z = GetZoneIn(loc, false);
	if (!z) return cs;

	int d = (int) r / SPACE_ZONE_EXTENT + 1;
	Vector3D<float> s(loc - Vector3D<float>(r)), e(loc + Vector3D<float>(r));
	GenericObject *o = NULL;

	for (int k = z->ID.Z - d; k <= z->ID.Z + d; k++)
	for (int j = z->ID.Z - d; j <= z->ID.Z + d; j++)
	for (int i = z->ID.Z - d; i <= z->ID.Z + d; i++)
	{
		tz = FindZone(Vector3D<int>(i, j, k));
		if (!tz) continue;
		for (int l = 0; l < (int) tz->Objects.size(); l++)
		{
			o = tz->Objects[l];
			if (OverlapCheck(s, e, o->RB->Location - Vector3D<float>(o->RB->ColSphereRadius * 0.5f), o->RB->Location + Vector3D<float>(o->RB->ColSphereRadius * 0.5f)))
			{
				// ensure no duplicates in our list
				int a = -1;
				for (a = 0; a < (int) cs.size(); a++)
					if (cs[a] == o) break;
				if (a >= (int) cs.size()) cs.push_back(o);
			}
		}
	}

	return cs;
}

Vector3D<float> CSP::GetClosestPoint(const Vector3D<float> &s, const Vector3D<float> &sn, const Vector3D<float> &p)
{
	return s + sn * ((p - s).Normal() | sn) * (p - s).Length();
}

Vector3D<float> CSP::GetExitPoint(const Vector3D<float> &S, const Vector3D<float> &Sn, const Vector3D<int> &zid)
{
	Vector3D<float> v;
	Vector3D<float> O;

	O = zid.ConvertToFloat() * SPACE_ZONE_EXTENT;

	// set v to be the distances to potential borders
	if (Sn.X > 0.0f) v.X = O.X - S.X + SPACE_ZONE_EXTENT;
	else if (Sn.X < 0.0f) v.X = O.X - S.X - SPACE_ZONE_EXTENT;
	if (Sn.Y >= 0.0f) v.Y = O.Y - S.Y + SPACE_ZONE_EXTENT;
	else if (Sn.Y < 0.0f) v.Y = O.Y - S.Y - SPACE_ZONE_EXTENT;
	if (Sn.Z >= 0.0f) v.Z = O.Z - S.Z + SPACE_ZONE_EXTENT;
	else if (Sn.Z < 0.0f) v.Z = O.Z - S.Z - SPACE_ZONE_EXTENT;
	// now that we have the distances to the potential borders
	//   dividing v by Sn will give us the time to travel the v component
	Vector3D<float> t;

	if (Sn.X != 0.0f) t.X = v.X / Sn.X;
	else t.X = 1000000.0;
	if (Sn.Y != 0.0f) t.Y = v.Y / Sn.Y;
	else t.Y = 1000000.0;
	if (Sn.Z != 0.0f) t.Z = v.Z / Sn.Z;
	else t.Z = 1000000.0;

	// X boundary is hit first
	if ((t.X <= t.Y) && (t.X <= t.Z))
	{
		// t.X tells us the scale to hit the rest of v by
		v = Sn * t.X + S;
	}
	// Y boundary is hit first
	else if ((t.Y <= t.X) && (t.Y <= t.Z))
	{
		// t.Y tells us the scale to hit the rest of v by
		v = Sn * t.Y + S;
	}
	// Z boundary is hit first
	else
	{
		// t.Z tells us the scale to hit the rest of v by
		v = Sn * t.Z + S;
	}

	return v;
}

GenericObject* CSP::GetObjectUnder(const Vector3D<float> &m)
{
	CSPZone *z = GetZoneIn(m, false);

	if (!z) return NULL;

	for (int i = 0; i < (int) z->Objects.size(); i++)
		if (z->Objects[i]->RB && z->Objects[i]->RB->IsWithin(m)) return z->Objects[i];

	return NULL;
}

void CSP::ClearAll(void)
{
	for (ZoneHash::iterator it = Zones.begin(); it != Zones.end(); it++)
		delete it->second;
	Zones.clear();
	
	for (ObjectHash::iterator it = Objects.begin(); it != Objects.end(); it++)
		delete it->first;
	Objects.clear();

	return;
}

CSP::~CSP()
{
	ClearAll();

	return;
}