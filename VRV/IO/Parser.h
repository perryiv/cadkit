
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Restart-file parser.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _VRV_IO_READER_CLASS_H_
#define _VRV_IO_READER_CLASS_H_

#include "VRV/IO/Files.h"
#include "VRV/IO/Directories.h"
#include "VRV/IO/Matrices.h"
#include "VRV/IO/Rotation.h"

#include "VRV/Interfaces/IParseRestart.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Math/Matrix44.h"
#include "Usul/Interfaces/IPlugin.h"

#include <string>


namespace VRV {
namespace IO {


class Parser : public Usul::Base::Referenced,
               public VRV::Interfaces::IParseRestart,
               public Usul::Interfaces::IPlugin
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Detail::Files Files;
  typedef Detail::Directories Directories;
  typedef Detail::Matrices<float> Matrices;
  typedef Detail::Rotation<float> Rotation;
  typedef Matrices::Matrix Matrix;
  typedef Rotation::Vec3 Vec3;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Parser );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction.
  Parser();

protected:

  // Do not copy.
  Parser ( const Parser & );
  Parser &operator = ( const Parser & );

  // Use reference counting.
  virtual ~Parser();

  /// Usul::Interfaces::IPlugin.
  virtual std::string getPluginName() const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  VRV::Interfaces::IParseRestart
  //
  /////////////////////////////////////////////////////////////////////////////

  // Parse the restart-file. The caller should support IRequestRead.
  virtual void          parseRestartFile ( const std::string &filename, Unknown *caller );

  /////////////////////////////////////////////////////////////////////////////

  // Convert the units into a scale matrix.
  void                  _convertUnits ( const std::string &units, Matrix &matrix );

  // Restore default values.
  void                  _reset();

  // Pre-multiply the current model matrix.
  void                  _preMult ( const Matrix &m );

  // Report progress.
  void                  _progress ( const std::string &message );

  // Callbacks for the xml-reader.
  void                  _setEnvDir    ( const std::string & );
  void                  _setEnvUnits  ( const std::string & );
  void                  _setEnvCache  ( const std::string & );
  void                  _modelStart   ( const std::string & );
  void                  _setModelDir  ( const std::string & );
  void                  _preMult      ( const std::string & );
  void                  _setRotDeg    ( const std::string & );
  void                  _setRotRad    ( const std::string & );
  void                  _setRotAxis   ( const std::string & );
  void                  _rotate       ( const std::string & );
  void                  _translate    ( const std::string & );
  void                  _scale        ( const std::string & );
  void                  _setModelFile ( const std::string & );
  void                  _modelFinish  ( const std::string & );

  // Convert the string to a 3D vector.
  void                  _convert ( const std::string &s, Vec3 &v ) const;


  // Read the data from file.
  void                  _read();

private:

  Unknown::RefPtr _caller;
  Files _files;
  Directories _dirs;
  Matrices _matrices;
  Rotation _rotation;
};


}; // namespace IO
}; // namespace VRV


#endif // _VRV_IO_READER_CLASS_H_
