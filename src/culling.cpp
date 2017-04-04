
#include "towerengine.h"


tCullingCompound::tCullingCompound(Operation operation, tCulling *a, tCulling *b)
{
	this->operation = operation;

	if(a)
		cullings.push_back(a);

	if(b)
		cullings.push_back(b);
}

void tCullingCompound::InitCulling()
{
	for(tCulling *culling : cullings)
		culling->InitCulling();
}

bool tCullingCompound::TestPointCulling(tVector point)
{
	if(operation == INTERSECTION)
	{
		for(tCulling *culling : cullings)
		{
			if(culling->TestPointCulling(point))
				return true;
		}
		return false;
	}
	else // if(operation == UNION)
	{
		for(tCulling *culling : cullings)
		{
			if(!culling->TestPointCulling(point))
				return false;
		}
		return true;
	}
}

bool tCullingCompound::TestSphereCulling(tVector center, float radius)
{
	if(operation == INTERSECTION)
	{
		for(tCulling *culling : cullings)
		{
			if(culling->TestSphereCulling(center, radius))
				return true;
		}
		return false;
	}
	else // if(operation == UNION)
	{
		for(tCulling *culling : cullings)
		{
			if(!culling->TestSphereCulling(center, radius))
				return false;
		}
		return true;
	}
}

bool tCullingCompound::TestBoundingBoxCulling(tBoundingBox b)
{
	if(operation == INTERSECTION)
	{
		for(tCulling *culling : cullings)
		{
			if(culling->TestBoundingBoxCulling(b))
				return true;
		}
		return false;
	}
	else // if(operation == UNION)
	{
		for(tCulling *culling : cullings)
		{
			if(!culling->TestBoundingBoxCulling(b))
				return false;
		}
		return true;
	}
}