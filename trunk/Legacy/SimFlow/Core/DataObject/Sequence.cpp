
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////
#include "Sequence.h"

void core::Sequence::setDrawable( osg::Drawable* drawable )
{					
	mDrawableObject->addDrawable( drawable );
}

osg::Geode* core::Sequence::getRenderObject()
{
	return mDrawableObject.get();
}