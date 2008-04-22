
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_STL_PLUGIN_FUNCTORS_H__
#define __OSG_STL_PLUGIN_FUNCTORS_H__

namespace osg { class Vec3f; typedef Vec3f Vec3; }

#include <iosfwd>


///////////////////////////////////////////////////////////////////////////////
//
//  Functor to write ascii.
//
///////////////////////////////////////////////////////////////////////////////

struct AsciiWriter
{
  AsciiWriter ( std::ostream& out ) : _out ( out ) { }
  void operator() ( const osg::Vec3&, const osg::Vec3&, const osg::Vec3&, const osg::Vec3& );
private:
  std::ostream &_out;
};


///////////////////////////////////////////////////////////////////////////////
//
//  Functor to write binary.
//
///////////////////////////////////////////////////////////////////////////////

struct BinaryWriter
{
  BinaryWriter ( std::ostream& out ) : _out ( out ) { }
  void operator() ( const osg::Vec3&, const osg::Vec3&, const osg::Vec3&, const osg::Vec3& );
private:
  std::ostream &_out;
};



#endif //__OSG_STL_PLUGIN_FUNCTORS_H__
