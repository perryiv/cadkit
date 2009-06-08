
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all readers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_BASE_READER_CLASS_H_
#define _MODFLOW_MODEL_BASE_READER_CLASS_H_

#include "ModflowModel/CompileGuard.h"
#include "ModflowModel/Model/Layer.h"

#include "Usul/Base/Observed.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"

#include <fstream>
#include <map>
#include <string>
#include <vector>

namespace Modflow { class ModflowDocument; }
class OGRSpatialReference;

namespace Modflow {
namespace Readers {


class BaseReader : public Usul::Base::Observed
{
public:

  // Useful typedefs.
  typedef Usul::Base::Observed BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef Modflow::Model::Layer Layer;
  typedef std::vector<Layer::RefPtr> Layers;
  typedef std::vector<double> GridData;
  typedef std::pair<std::string,GridData> GridInfo;
  typedef Usul::Math::Vec2ui Vec2ui;
  typedef Usul::Math::Vec2d Vec2d;
  typedef std::map < std::string, std::string > FileAttributes;
  typedef std::vector<std::string> RegistryPath;

  // Type information.
  USUL_DECLARE_TYPE_ID ( BaseReader );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BaseReader );

  // Set/get the file attributes.
  void                    fileAttributes ( const FileAttributes & );
  std::string             fileAttribute ( const std::string & ) const;

  // Set the "no data" value.
  virtual void            noDataSet ( ModflowDocument *, long );

  // Read the file.
  virtual void            read ( ModflowDocument *doc, const std::string &file, Unknown *progress ) = 0;

  // Set/get the registry path.
  void                    registryPath ( const RegistryPath & );
  RegistryPath            registryPath() const;

protected:

  // Construction.
  BaseReader();

  // Use reference counting.
  virtual ~BaseReader();

  void                    _checkStream();

  static unsigned int     _countLines ( const std::string &label, const std::string &file, bool atStart = true );

  std::string             _getLine();

  void                    _open ( const std::string &file );

  void                    _readGrid ( unsigned int numWordsToSkip, GridInfo &grid );

  void                    _seekToLine ( const std::string &word );
  void                    _set ( ModflowDocument *document, Unknown *progress );
  void                    _skipLines ( char c );

  OGRSpatialReference *   _coordinateSystem() const;
private:

  // Do not copy.
  BaseReader ( const BaseReader & );
  BaseReader &operator = ( const BaseReader & );

protected:

  std::string _file;
  std::ifstream _in;
  Unknown::RefPtr _progress;
  ModflowDocument *_document;
  FileAttributes _fileAttributes;
  RegistryPath _registryPath;
};


} // namespace Readers
} // namespace Modflow


#endif // _MODFLOW_MODEL_BASE_READER_CLASS_H_
