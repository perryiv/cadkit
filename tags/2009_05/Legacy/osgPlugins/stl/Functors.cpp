
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#include "Functors.h"

#include "osg/Geode"
#include "osg/Geometry"
#include "osg/Vec3"

#include "Usul/MPL/SameType.h"
#include "Usul/Types/Types.h"
#include "Usul/IO/Writer.h"

#include <iostream>


///////////////////////////////////////////////////////////////////////////////
//
//  Write out a facet in Ascii
//
///////////////////////////////////////////////////////////////////////////////

void AsciiWriter::operator () ( const osg::Vec3& normal, const osg::Vec3& v1, const osg::Vec3& v2, const osg::Vec3& v3 )
{
  _out << "facet normal " << normal[0] << " " << normal[1] << " " << normal[2] << " " << std::endl;
  _out << "  outer loop " << std::endl;
  _out << "    vertex "   << v1[0] << " " << v1[1] << " " << v1[2] << " " << std::endl;
  _out << "    vertex "   << v2[0] << " " << v2[1] << " " << v2[2] << " " << std::endl;
  _out << "    vertex "   << v3[0] << " " << v3[1] << " " << v3[2] << " " << std::endl;
  _out << "  endloop"     << std::endl;
  _out << "endfacet"      << std::endl;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Write out a facet in binary. STL is little-endian. 
//  See: http://www.transcendata.com/cadfix/support/changeNotesDX.htm
//
///////////////////////////////////////////////////////////////////////////////

void BinaryWriter::operator () ( const osg::Vec3& normal, const osg::Vec3& v1, const osg::Vec3& v2, const osg::Vec3& v3 )
{
  // Current algorithm only works on 32-bit machines.
  USUL_ASSERT_SAME_TYPE ( Usul::Types::Float32, float );

  // Write the normal and vertices.
  Usul::IO::WriteLittleEndian::write3 ( _out, normal );
  Usul::IO::WriteLittleEndian::write3 ( _out, v1 );
  Usul::IO::WriteLittleEndian::write3 ( _out, v2 );
  Usul::IO::WriteLittleEndian::write3 ( _out, v3 );

  // Write spacers to bring record size to 50 bytes.
  _out.write ( "\0", 1 );
  _out.write ( "\0", 1 );
}
