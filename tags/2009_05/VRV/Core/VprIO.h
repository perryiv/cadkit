
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Usul/Types/Types.h"
#include "Usul/Math/Matrix44.h"

#include "vpr/IO/SerializableObject.h"
#include "vpr/IO/ObjectReader.h"
#include "vpr/IO/ObjectWriter.h"

#include "osg/Matrixd"

#include "vector"

namespace VRV {
namespace Core {

template < class T > struct ReaderWriter;


///////////////////////////////////////////////////////////////////////////////
//
//  Read/Write a boolean value
//
///////////////////////////////////////////////////////////////////////////////

template<>
struct ReaderWriter<bool>
{
  static void read ( vpr::ObjectReader *reader, bool &value )
  {
    reader->readBool ( value );
  }
  static void write ( vpr::ObjectWriter *writer, bool value )
  {
    writer->writeBool ( value );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Read/Write a float value
//
///////////////////////////////////////////////////////////////////////////////

template<>
struct ReaderWriter<float>
{
  static void read ( vpr::ObjectReader *reader, float &value )
  {
    reader->readFloat ( value );
  }
  static void write ( vpr::ObjectWriter *writer, float value )
  {
    writer->writeFloat ( value );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Read/Write a double value
//
///////////////////////////////////////////////////////////////////////////////

template<>
struct ReaderWriter<double>
{
  static void read ( vpr::ObjectReader *reader, double &value )
  {
    reader->readDouble ( value );
  }
  static void write ( vpr::ObjectWriter *writer, double value )
  {
    writer->writeDouble ( value );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Read/Write an unsigned 8 bit value.
//
///////////////////////////////////////////////////////////////////////////////

template<>
struct ReaderWriter<Usul::Types::Uint8>
{
  static void read ( vpr::ObjectReader *reader, Usul::Types::Uint8 &value )
  {
    reader->readUint8( value );
  }
  static void write ( vpr::ObjectWriter *writer, Usul::Types::Uint8 value )
  {
    writer->writeUint8 ( value );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Read/Write an unsigned 16 bit value.
//
///////////////////////////////////////////////////////////////////////////////

template<>
struct ReaderWriter<Usul::Types::Uint16>
{
  static void read ( vpr::ObjectReader *reader, Usul::Types::Uint16 &value )
  {
    reader->readUint16( value );
  }
  static void write ( vpr::ObjectWriter *writer, Usul::Types::Uint16 value )
  {
    writer->writeUint16 ( value );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Read/Write an unsigned 32 bit value.
//
///////////////////////////////////////////////////////////////////////////////

template<>
struct ReaderWriter<Usul::Types::Uint32>
{
  static void read ( vpr::ObjectReader *reader, Usul::Types::Uint32 &value )
  {
    reader->readUint32 ( value );
  }
  static void write ( vpr::ObjectWriter *writer, Usul::Types::Uint32 value )
  {
    writer->writeUint32 ( value );
  }
};



///////////////////////////////////////////////////////////////////////////////
//
//  Read/Write an unsigned 64 bit value.
//
///////////////////////////////////////////////////////////////////////////////
#if 0
// This isn't compiling on the viz cluster.
// I don't think it's current being used any where
template<>
struct ReaderWriter<Usul::Types::Uint64>
{
  static void read ( vpr::ObjectReader *reader, Usul::Types::Uint64 &value )
  {
    reader->readUint64 ( value );
  }
  static void write ( vpr::ObjectWriter *writer, Usul::Types::Uint64 value )
  {
    writer->writeUint64( value );
  }
};
#endif


///////////////////////////////////////////////////////////////////////////////
//
//  Read/Write a std::string.
//
///////////////////////////////////////////////////////////////////////////////

template<>
struct ReaderWriter<std::string>
{
  static void read ( vpr::ObjectReader *reader, std::string &value )
  {
    reader->readString ( value );
  }
  static void write ( vpr::ObjectWriter *writer, std::string value )
  {
    writer->writeString ( value );
  }
};


///////////////////////////////////////////////////////////////////////////////
//
//  Read/Write an osg::Matrixd.
//
///////////////////////////////////////////////////////////////////////////////

template<>
struct ReaderWriter<osg::Matrixd>
{
  static void read ( vpr::ObjectReader *reader, osg::Matrixd &matrix )
  {
    osg::Matrixd::value_type *ptr ( matrix.ptr() );
    for ( unsigned int i = 0; i < 16; ++i )
      reader->readDouble( ptr[i] );
  }
  static void write ( vpr::ObjectWriter *writer, osg::Matrixd matrix )
  {
    const osg::Matrixd::value_type *ptr ( matrix.ptr() );
    for ( unsigned int i = 0; i < 16; ++i )
      writer->writeDouble( ptr[i] );
  }
};

///////////////////////////////////////////////////////////////////////////////
//
//  Read/Write an Usul::Math::Matrix44d.
//
///////////////////////////////////////////////////////////////////////////////

template<>
struct ReaderWriter<Usul::Math::Matrix44d>
{
  static void read ( vpr::ObjectReader *reader, Usul::Math::Matrix44d &matrix )
  {
    Usul::Math::Matrix44d::value_type *ptr ( matrix.get() );
    for ( unsigned int i = 0; i < 16; ++i )
      reader->readDouble( ptr[i] );
  }
  static void write ( vpr::ObjectWriter *writer, Usul::Math::Matrix44d matrix )
  {
    const Usul::Math::Matrix44d::value_type *ptr ( matrix.get() );
    for ( unsigned int i = 0; i < 16; ++i )
      writer->writeDouble( ptr[i] );
  }
};


}
}
