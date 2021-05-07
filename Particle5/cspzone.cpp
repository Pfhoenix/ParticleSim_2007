#include "includes.h"

CSPZone::CSPZone()
{
	return;
}

void CSPZone::Add(GenericObject *O)
{
	if (O) Objects.push_back(O);

	return;
}

void CSPZone::Remove(GenericObject *O)
{
	if (!O) return;

	for (int i = 0; i < (int) Objects.size(); i++)
		if (O == Objects[i])
		{
			vector<GenericObject*>::iterator it = Objects.begin();
			it += i;
			Objects.erase(it);
			break;
		}

	return;
}

CSPZone::~CSPZone()
{
	Objects.clear();

	return;
}
