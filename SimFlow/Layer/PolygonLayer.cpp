
#include "PolygonLayer.h"

using namespace SimFlow::Layer;

PolygonLayer::PolygonLayer() : 
Layer           (),
mAttributeMode  ( 1 ),
mAutoRotate     ( false ), 
mAutoScale      ( false ) 
{	
}

PolygonLayer::~PolygonLayer()
{
}

const unsigned int& PolygonLayer::attributeMode() const
{
	return mAttributeMode;
}

void PolygonLayer::attributeMode( const unsigned int& mode )
{
	mAttributeMode = mode;
}

const bool& PolygonLayer::autoRotate() const
{
	return mAutoRotate;
}

void PolygonLayer::autoRotate( const bool& val )
{
	mAutoRotate = val;	
}

const bool&	PolygonLayer::autoScale() const
{
	return mAutoScale;
}

void PolygonLayer::autoScale( const bool& val )
{
	mAutoScale = val;
}


