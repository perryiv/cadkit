
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that encapsulates the environment and model directories.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_IO_DIRECTORIES_CLASS_H_
#define _VRV_IO_DIRECTORIES_CLASS_H_

#include "VRV/IO/CompileGuard.h"

#include <string>


namespace VRV {
namespace IO {
namespace Detail {


class Directories
{
public:

  // Default construction.
  Directories() : _env(), _model()
  {
  }

  // Reset the members to their defaults.
  void reset()
  {
    _env.clear();
    _model.clear();
  }

  // Indicates that the start of a model has been encountered.
  void modelStart()
  {
    _model.clear();
  }

  // Indicates that the end of a model has been encountered.
  void modelFinish()
  {
    _model.clear();
  }

  // Return the proper directory string.
  const std::string &dir() const
  {
    return ( _model.empty() ) ? _env : _model;
  }

  // Set the environment directory.
  void env ( const std::string &s )
  {
    _env = s;
    this->_fixDir ( _env );
  }

  // Set the model directory.
  void model ( const std::string &s )
  {
    _model = s;
    this->_fixDir ( _model );
  }

private:

  // No copy construction or assignment.
  Directories ( const Directories & );
  Directories &operator = ( const Directories & );

  // Put a slash on the end if needed.
  void _fixDir ( std::string &dir )
  {
    // If it is not empty...
    if ( !dir.empty() )
    {
      // Append a slash if there isn't one.
      std::string::size_type last = dir.size() - 1;
      if ( '/' != dir[last] && '\\' != dir[last] )
        dir += "/";
    }
  }

  std::string _env;
  std::string _model;
};


}; // namespace Detail
}; // namespace OSG
}; // namespace VRV


#endif // _VRV_IO_DIRECTORIES_CLASS_H_
