#include "Usul/Types/Types.h"
#include "VRV/Export.h"

#include "vpr/IO/SerializableObject.h"
#include "vpr/IO/ObjectReader.h"
#include "vpr/IO/ObjectWriter.h"

#include "vector"

namespace VRV {
namespace Core {

template < class T > struct ReaderWriter;

template<>
struct ReaderWriter<bool>
{
  static void read ( vpr::ObjectReader *reader, bool value )
  {
    reader->readBool ( value );
  }
  static void write ( vpr::ObjectWriter *writer, bool value )
  {
    writer->writeBool ( value );
  }
};

template<>
struct ReaderWriter<float>
{
  static void read ( vpr::ObjectReader *reader, float value )
  {
    reader->readFloat ( value );
  }
  static void write ( vpr::ObjectWriter *writer, float value )
  {
    writer->writeFloat ( value );
  }
};

template<>
struct ReaderWriter<double>
{
  static void read ( vpr::ObjectReader *reader, double value )
  {
    reader->readDouble ( value );
  }
  static void write ( vpr::ObjectWriter *writer, double value )
  {
    writer->writeDouble ( value );
  }
};

template<>
struct ReaderWriter<Usul::Types::Uint8>
{
  static void read ( vpr::ObjectReader *reader, Usul::Types::Uint8 value )
  {
    reader->readUint8( value );
  }
  static void write ( vpr::ObjectWriter *writer, Usul::Types::Uint8 value )
  {
    writer->writeUint8 ( value );
  }
};


template<>
struct ReaderWriter<Usul::Types::Uint16>
{
  static void read ( vpr::ObjectReader *reader, Usul::Types::Uint16 value )
  {
    reader->readUint16( value );
  }
  static void write ( vpr::ObjectWriter *writer, Usul::Types::Uint16 value )
  {
    writer->writeUint16 ( value );
  }
};

template<>
struct ReaderWriter<Usul::Types::Uint32>
{
  static void read ( vpr::ObjectReader *reader, Usul::Types::Uint32 value )
  {
    reader->readUint32 ( value );
  }
  static void write ( vpr::ObjectWriter *writer, Usul::Types::Uint32 value )
  {
    writer->writeUint32 ( value );
  }
};

template<>
struct ReaderWriter<Usul::Types::Uint64>
{
  static void read ( vpr::ObjectReader *reader, Usul::Types::Uint64 value )
  {
    reader->readUint64 ( value );
  }
  static void write ( vpr::ObjectWriter *writer, Usul::Types::Uint64 value )
  {
    writer->writeUint64( value );
  }
};

}
}
