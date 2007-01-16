
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_VEC_FOUR_SERIALIZE_H__
#define __OSG_TOOLS_VEC_FOUR_SERIALIZE_H__

#include "osg/Vec4"

#include "boost/serialization/nvp.hpp"

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive & ar, osg::Vec4 & vec, const unsigned int version)
{
  ar & boost::serialization::make_nvp( "X", vec.x() );
  ar & boost::serialization::make_nvp( "Y", vec.y() );
  ar & boost::serialization::make_nvp( "Z", vec.z() );
  ar & boost::serialization::make_nvp( "W", vec.w() );
}

} // namespace serialization
} // namespace boost


#endif // __OSG_TOOLS_VEC_FOUR_SERIALIZE_H__
