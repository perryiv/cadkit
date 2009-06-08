
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WRF_PARSER_H__
#define __WRF_PARSER_H__

#include "Usul/Types/Types.h"

#include <string>
#include <cstdio>
#include <vector>

class Parser
{
public:
  typedef std::vector < Usul::Types::Float32 > Data;

  Parser( );
  Parser( const std::string & filename );
  Parser( const Parser& );
  ~Parser ();

  /// Assignment.
  Parser& operator = ( const Parser& parser );

  /// Get/Set the filename.
  void                           filename ( const std::string& filename );
  const std::string&             filename () const;

  /// Set the sizes of the data.
  void                           setSizes ( unsigned int x, unsigned int y, unsigned int z );

  /// Get/Set the number of timesteps.
  void                           timesteps ( unsigned int );
  unsigned int                   timesteps () const;

  /// Get/Set the number of channels.
  void                           channels ( unsigned int );
  unsigned int                   channels () const;

  /// Get/Set the nubmer of 2D fields.
  void                           numFields2D ( unsigned int );
  unsigned int                   numFields2D () const;

  /// Get the data for the given channel and timestep
  void                           data ( Data& data, unsigned int timestep, unsigned int channel );

  /// Get the i'th 2D field.
  void                           field2D ( Data& data, unsigned int i );

  /// Set has header flag.
  void                           headers ( bool b );
  bool                           headers () const;

private:
  /// Open the file.
  void                           _open ();

  /// Seek.
  void                           _seek ( Usul::Types::Int64 offset );

  /// Seek.
  void                           _seek ( Usul::Types::Int64 offset, int whence );

  /// Read a 2D slice.
  void                           _readSlice ( Data::value_type* buffer );

  /// Get the size of a header.
  unsigned int                   _headerSize () const;

  std::string   _filename;
  FILE         *_fp;
  unsigned int  _xSize;
  unsigned int  _ySize;
  unsigned int  _zSize;
  unsigned int  _numTimesteps;
  unsigned int  _numChannels;
  unsigned int  _numFields2D;
  bool          _headers;
};


#endif // __WRF_PARSER_H__
