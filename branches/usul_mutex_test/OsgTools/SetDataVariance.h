
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Functor that sets the data-variance.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_SET_DATA_VARIANCE_H_
#define _OSG_TOOLS_SET_DATA_VARIANCE_H_


namespace OsgTools {


struct SetDataVariance
{
  SetDataVariance ( const osg::Object::DataVariance &v ) : _variance ( v )
  {
  }
  SetDataVariance ( const SetDataVariance &v ) : _variance ( v._variance )
  {
  }
  SetDataVariance &operator = ( const SetDataVariance &v )
  {
    _variance = v._variance;
    return *this;
  }
  void operator () ( osg::Object *object )
  {
    if ( object )
      object->setDataVariance ( _variance );
  }
  const osg::Object::DataVariance &variance() const
  {
    return _variance;
  }
  void variance ( const osg::Object::DataVariance &v )
  {
    _variance = v;
  }

private:

  osg::Object::DataVariance _variance;
};


}; // namespace OsgTools


#endif // _OSG_TOOLS_SET_DATA_VARIANCE_H_
