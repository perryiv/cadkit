
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_SCOPED_DISPLAY_LISTS_H__
#define __OSG_TOOLS_SCOPED_DISPLAY_LISTS_H__

#include "osg/ref_ptr"
#include "osg/Drawable"


namespace OsgTools {

struct ScopedDisplayLists
{
  ScopedDisplayLists ( osg::Drawable* draw, bool orig, bool final ) :
    _drawable( draw ),
    _final ( final )
  {
    _drawable->setUseDisplayList( orig );
  }

  ~ScopedDisplayLists()
  {
    _drawable->setUseDisplayList( _final );
  }

private:

  ScopedDisplayLists ( const ScopedDisplayLists & );
  ScopedDisplayLists &operator = ( const ScopedDisplayLists & );

  osg::ref_ptr< osg::Drawable > _drawable;
  bool _final;
};

}

#endif //__OSG_TOOLS_SCOPED_DISPLAY_LISTS_H__

