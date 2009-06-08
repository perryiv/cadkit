
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#include "Point.h"

using namespace SimFlow::DataObject;

Point::Point() : 
DataObject(), 
mSize( 1 ), 
mCenter(), 
mRenderType( 1 ) 
//mMaterial( 0x00 ), 
//mTransform( 0x00 )
{
	
}

Point::~Point()
{
}

osg::Node* Point::buildScene() 
{
	// Implement this function. 
	return 0x00;
}


osg::Node* Point::buildGeometry()
{
	// Implement this function. 
	return 0x00;
}

const float& Point::size() const 
{
	return mSize;
}

void Point::size( const float& size )
{
	mSize = size;
	this->dirty( true );
}

const osg::Vec3& Point::center() const 
{
	return mCenter;
}

void Point::center( const osg::Vec3& center )
{
	mCenter = center;
	this->dirty( true );
}

const unsigned int& Point::renderType() const
{
	return mRenderType;
}

void Point::renderType( const unsigned int& renderType )
{
	mRenderType = renderType;
	this->dirty( true );
}

/*
osg::Material* Point::material()
{
	return mMaterial.get();
}

void Point::material( osg::Material* material )
{
	mMaterial = material;
	this->dirty( true );
}
*/

