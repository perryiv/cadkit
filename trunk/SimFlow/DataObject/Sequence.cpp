#include "Sequence.h"

void core::Sequence::setDrawable( osg::Drawable* drawable )
{					
	mDrawableObject->addDrawable( drawable );
}

osg::Geode* core::Sequence::getRenderObject()
{
	return mDrawableObject.get();
}