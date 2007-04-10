
#include "Ray.h"

using namespace SimFlow::SimFlowUtil::Collision;

Ray::Ray() :
mID                   (),
mStartPoint			      ( osg::Vec3f() ),
mEndPoint			        ( osg::Vec3f() ),
mBaseStartPoint	      ( mStartPoint ), 
mBaseEndPoint	        ( mEndPoint ),
mWorldIntersectPoint	( osg::Vec3f() ), 
mLocalIntersectPoint	( osg::Vec3f() ),
mWorldIntersectNormal	( osg::Vec3f() ),
mLocalIntersectNormal	( osg::Vec3f() ),
mActive					      ( true )
{
}

Ray::Ray( osg::Vec3f startPoint, osg::Vec3f endPoint, bool active )
{
	Ray();
	mStartPoint   = startPoint;
	mEndPoint     = endPoint;
	mActive       = active;
}

Ray::Ray(  std::string id, osg::Vec3f startPoint, osg::Vec3f endPoint, bool active )
{
	Ray();
	mID		      = id;
	mStartPoint = startPoint;
	mEndPoint   = endPoint;
	mActive     = active;
}

Ray::~Ray()
{}

std::string Ray::ID()
{
  return mID;
}

void Ray::ID( std::string id )
{
  mID = id;
}

bool Ray::active()
{
	return mActive;
}

void Ray::active( bool val )
{
	mActive = val;
}

osg::Vec3f Ray::startPoint()
{
	return mStartPoint;
}

void Ray::startPoint( osg::Vec3f startPoint )
{
	mStartPoint = startPoint;
}

osg::Vec3f Ray::endPoint()
{
	return mEndPoint;
}

void Ray::endPoint( osg::Vec3f endPoint )
{
	mEndPoint = endPoint;
}

osg::Vec3f Ray::baseStartPoint()
{
  return mBaseStartPoint;
}

void Ray::baseStartPoint( osg::Vec3f baseStartPoint )
{
  mBaseStartPoint = baseStartPoint;
}

osg::Vec3f Ray::baseEndPoint()
{
	return mBaseEndPoint;
}

void Ray::baseEndPoint( osg::Vec3f endPoint )
{
	mBaseEndPoint = endPoint;
}

osg::Vec3f Ray::worldIntersectPoint()
{
	return mWorldIntersectPoint;
}

void Ray::worldIntersectPoint( osg::Vec3f point )
{
	mWorldIntersectPoint = point;
}

osg::Vec3f Ray::localIntersectPoint()
{
	return mLocalIntersectPoint;
}

void Ray::localIntersectPoint( osg::Vec3f point )
{
	mLocalIntersectPoint = point;
}

osg::Vec3f Ray::worldIntersectNormal()
{
	return mWorldIntersectNormal;
}

void Ray::worldIntersectNormal( osg::Vec3f normal )
{
	mWorldIntersectNormal = normal;
}

osg::Vec3f Ray::localIntersectNormal()
{
	return mLocalIntersectNormal;
}

void Ray::localIntersectNormal( osg::Vec3f normal )
{
	mLocalIntersectNormal = normal;
}

