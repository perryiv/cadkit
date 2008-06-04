///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, John K. Grant
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _osg_tools_dimension_h_
#define _osg_tools_dimension_h_

#include "OsgTools/Configure/OSG.h"

#include "osg/Drawable"

#include <functional>


namespace OsgTools
{

  /** Dimension
    * The Dimension class is a functor that is used when iterating
    * over the osg::Drawable children of an osg::Geode.  It contains
    * the overloaded operator () which accepts an object for analysis.
    *
    * Currently, there is an implementation for the osg::Drawable*,
    * but this class could be expanded to support BoundingSphere tests
    * for the osg::Node types.
    */
  class Dimension : public std::unary_function<osg::Drawable*,float>
  {
  public:
    enum Strategy { X, Y, Z, VOLUME };
    typedef std::unary_function<osg::Drawable*,float> BaseClass;

    Dimension(): _k(X) {}
    Dimension(Strategy d): _k(d) {}
    Dimension(const Dimension& d): _k(d._k) {}
    virtual ~Dimension() {}
    Dimension& operator =(const Dimension& s)
    {
      _k = s._k;
      return *this;
    }

    inline float operator ()(const osg::Drawable* d)
    {
      /// TODO: use Perry's exception code to catch a NULL Drawable

      const osg::BoundingBox& b( d->getBound() );
      float value(0);

      switch( _k )
      {
      case X:
        {
          value = b.xMax() - b.xMin();
        } break;

      case Y:
        {
          value = b.yMax() - b.yMin();
        } break;

      case Z:
        {
          value = b.zMax() - b.zMin();
        } break;

      case VOLUME:
        {
          float depth( b.zMax() - b.zMin() );
          float height( b.yMax() - b.yMin() );
          float width( b.xMax() - b.xMin() );
          value = depth*height*width;
        } break;

      default:
        {
          /// TODO: use Perry's exception code here to state a bad type was given
        } break;
      }

      return value;
    }

    void strategy(Strategy d) { _k = d; }
    Strategy strategy() const { return _k; }

  private:
    Strategy _k;
  };

};

#endif
