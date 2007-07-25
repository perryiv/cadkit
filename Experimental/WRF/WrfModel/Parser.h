
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

  /// Get the data for the given channel and timestep
  void                           data ( Data& data, unsigned int channel, unsigned int timestep );

private:
  std::string   _filename;
  FILE         *_fp;
  unsigned int  _xSize;
  unsigned int  _ySize;
  unsigned int  _zSize;
  unsigned int  _numTimesteps;
  unsigned int  _numChannels;
};


#endif // __WRF_PARSER_H__
