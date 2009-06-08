
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor to iterate through children.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_FOR_EACH_FUNCTOR_H_
#define _OSG_TOOLS_FOR_EACH_FUNCTOR_H_

#include "osg/Geode"
#include "osg/Drawable"


namespace OsgTools {


///////////////////////////////////////////////////////////////////////////////
//
//  Generic definition.
//
///////////////////////////////////////////////////////////////////////////////

template < class Operation_, class NodeType_ > struct ForEach;


///////////////////////////////////////////////////////////////////////////////
//
//  Specialization for drawables.
//
///////////////////////////////////////////////////////////////////////////////

template < class Operation_ > struct ForEach < Operation_, osg::Drawable >
{
  typedef Operation_ operation_type;

  ForEach ( const operation_type &op ) : _op ( op )
  {
  }

  ForEach ( const ForEach &fe ) : _op ( fe._op )
  {
  }

  ForEach &operator = ( const ForEach &fe )
  {
    _op = fe._op;
    return *this;
  }

  void operator () ( osg::Geode *geode )
  {
    if ( geode )
    {
      unsigned int num ( geode->getNumDrawables() );
      for ( unsigned int i = 0; i < num; ++i )
      {
        osg::Drawable *drawable = geode->getDrawable ( i );
        if ( drawable )
        {
          _op ( *drawable );
        }
      }
    }
  }

private:

  operation_type _op;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_FOR_EACH_FUNCTOR_H_
