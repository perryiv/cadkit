
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Registry functions.
//
///////////////////////////////////////////////////////////////////////////////

#include "OsgFox/Views/Registry.h"

#include "FoxTools/Registry/Registry.h"

#include <sstream>

namespace OsgFox {
namespace Registry {


///////////////////////////////////////////////////////////////////////////////
//
//  Read the vector.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class Vector, unsigned int Size > struct ReadVector
  {
    Vector operator () ( const std::string &section, const std::string &key, const Vector &vec )
    {
      std::istringstream s ( FoxTools::Registry::read ( section, key, std::string() ) );
      Vector temp;
      for ( unsigned int i = 0; i < Size; ++i )
      {
        if ( s.eof() )
          temp[i] = vec[i];
        else
          s >> temp[i];
      }
      return temp;
    }
  };
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the vector.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec4f read ( const std::string &section, const std::string &key, const osg::Vec4f &vec )
{
  return Detail::ReadVector<osg::Vec4f,4>() ( section, key, vec );
}
osg::Vec4d read ( const std::string &section, const std::string &key, const osg::Vec4d &vec )
{
  return Detail::ReadVector<osg::Vec4d,4>() ( section, key, vec );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the vector.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec3f read ( const std::string &section, const std::string &key, const osg::Vec3f &vec )
{
  return Detail::ReadVector<osg::Vec3f,3>() ( section, key, vec );
}
osg::Vec3d read ( const std::string &section, const std::string &key, const osg::Vec3d &vec )
{
  return Detail::ReadVector<osg::Vec3d,3>() ( section, key, vec );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Read the vector.
//
///////////////////////////////////////////////////////////////////////////////

osg::Vec2f read ( const std::string &section, const std::string &key, const osg::Vec2f &vec )
{
  return Detail::ReadVector<osg::Vec2f,2>() ( section, key, vec );
}
osg::Vec2d read ( const std::string &section, const std::string &key, const osg::Vec2d &vec )
{
  return Detail::ReadVector<osg::Vec2d,2>() ( section, key, vec );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the vector.
//
///////////////////////////////////////////////////////////////////////////////

void write ( const std::string &section, const std::string &key, const osg::Vec2f &vec )
{
  std::ostringstream out;
  out << vec[0] << ' ' << vec[1];
  FoxTools::Registry::write ( section, key, out.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the vector.
//
///////////////////////////////////////////////////////////////////////////////

void write ( const std::string &section, const std::string &key, const osg::Vec3f &vec )
{
  std::ostringstream out;
  out << vec[0] << ' ' << vec[1] << ' ' << vec[2];
  FoxTools::Registry::write ( section, key, out.str() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write the vector.
//
///////////////////////////////////////////////////////////////////////////////

void write ( const std::string &section, const std::string &key, const osg::Vec4f &vec )
{
  std::ostringstream out;
  out << vec[0] << ' ' << vec[1] << ' ' << vec[2] << ' ' << vec[3];
  FoxTools::Registry::write ( section, key, out.str() );
}


}; // namespace Registry
}; // namespace OsgFox
