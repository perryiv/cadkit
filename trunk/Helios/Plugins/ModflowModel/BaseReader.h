
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all readers.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_BASE_READER_CLASS_H_
#define _MODFLOW_MODEL_BASE_READER_CLASS_H_

#include "CompileGuard.h"
#include "Layer.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"

#include <fstream>
#include <string>
#include <vector>

class ModflowDocument;


class BaseReader : public Usul::Base::Object
{
public:

  // Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef std::vector<Layer::RefPtr> Layers;
  typedef std::vector<double> GridData;
  typedef std::pair<std::string,GridData> GridInfo;
  typedef Usul::Math::Vec2ui Vec2ui;
  typedef Usul::Math::Vec2d Vec2d;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BaseReader );

  // Read the file.
  virtual void            read ( ModflowDocument *doc, const std::string &file, Unknown *progress ) = 0;

protected:

  // Construction.
  BaseReader();

  // Use reference counting.
  virtual ~BaseReader();

  void                    _checkStream();

  std::string             _getLine();

  void                    _open ( const std::string &file );

  void                    _readGrid ( GridInfo &grid );

  void                    _seekToLine ( const std::string &word );
  void                    _set ( ModflowDocument *document, Unknown *progress );
  void                    _skipLines ( char c );

private:

  // Do not copy.
  BaseReader ( const BaseReader & );
  BaseReader &operator = ( const BaseReader & );

protected:

  std::string _file;
  std::ifstream _in;
  Unknown::RefPtr _progress;
  ModflowDocument *_document;
};


#endif // _MODFLOW_MODEL_BASE_READER_CLASS_H_
