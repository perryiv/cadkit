
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant and Perry L. Miller IV.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Predicates that operate on osg::Object objects.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CV_SCENE_PREDICATES_H_
#define _CV_SCENE_PREDICATES_H_

#include "CadViewer/Interfaces/ISelection.h"
#include "CadViewer/Interfaces/IVisibility.h"

#include <functional>


namespace CV {
namespace OSG {
namespace Predicates {


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate class for determining selection.
//
///////////////////////////////////////////////////////////////////////////////

struct IsSelected : public std::unary_function < osg::Object *, bool >
{
  IsSelected ( CV::Interfaces::ISelection *s ) : _s ( s ){}
  IsSelected ( const IsSelected &s ) : _s ( s._s ){}
  bool operator () ( const osg::Object *object ) const
  {
    return ( object ) ? _s->isSelected ( object ) : false;
  }
private:
  CV::Interfaces::ISelection::ValidRefPtr _s;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Predicate class for determining visibility.
//
///////////////////////////////////////////////////////////////////////////////

struct IsVisible : public std::unary_function < osg::Node *, bool >
{
  IsVisible ( CV::Interfaces::IVisibility *v ) : _v ( v ){}
  IsVisible ( const IsVisible &v ) : _v ( v._v ){}
  bool operator () ( const osg::Node *node ) const
  {
    return ( node ) ? _v->isVisible ( node ) : false;
  }
private:
  CV::Interfaces::IVisibility::ValidRefPtr _v;
};


}; // namespace Predicates
}; // namespace OSG
}; // namespace CV


#endif // _CV_SCENE_PREDICATES_H_
