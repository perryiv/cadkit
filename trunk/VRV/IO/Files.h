
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class that encapsulates the restart and model filenames.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_IO_FILES_CLASS_H_
#define _VRV_IO_FILES_CLASS_H_

#include "VRV/IO/CompileGuard.h"

#include <string>


namespace VRV {
namespace IO {
namespace Detail {


class Files
{
public:

  // Default construction.
  Files() : _restart(), _model()
  {
  }

  // Reset the members to their defaults.
  void reset()
  {
    _restart.clear();
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

  // Get the restart file.
  const std::string &restart() const
  {
    return _restart;
  }

  // Set the restart file.
  void restart ( const std::string &s )
  {
    _restart = s;
  }

  // Get the model file.
  const std::string &model() const
  {
    return _model;
  }

  // Set the model file.
  void model ( const std::string &s )
  {
    _model = s;
  }

private:

  // No copy construction or assignment.
  Files ( const Files & );
  Files &operator = ( const Files & );

  std::string _restart;
  std::string _model;
};


}; // namespace Detail
}; // namespace OSG
}; // namespace VRV


#endif // _VRV_IO_FILES_CLASS_H_
