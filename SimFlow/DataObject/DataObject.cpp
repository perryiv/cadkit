
#include "DataObject.h"

using namespace SimFlow::DataObject;

DataObject::DataObject() : 
Referenced      (),
mColor          ( 1 ),
mDirty          ( false ) 
//mLabel        (	), 
//mLabelPosition()
{
}

DataObject::~DataObject()
{
}

const unsigned int& DataObject::color() const 
{
	return mColor;
}

void DataObject::color( const unsigned int& color )
{
	mColor = color;
	this->dirty( true );
}

bool DataObject::dirty() const
{
	return mDirty;
}

void DataObject::dirty( bool dirty )
{
	mDirty = dirty;
}
/*
const std::string DataObject::label() const
{
	return mLabel;
}

void DataObject::label( const std::string& label )
{
	mLabel = label;
	this->dirty( true );
}

const osg::Vec3& DataObject::labelPosition() const 
{
	return mLabelPosition;
}

void DataObject::labelPosition( const osg::Vec3& pos )
{
	mLabelPosition = pos;
	this->dirty( true );
}
*/

	