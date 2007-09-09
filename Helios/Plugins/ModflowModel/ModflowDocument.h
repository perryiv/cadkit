
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for modflow files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_DOCUMENT_H_
#define _MODFLOW_MODEL_DOCUMENT_H_

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"

#include <iosfwd>
#include <map>
#include <string>
#include <vector>

namespace osg { class Node; }


class ModflowDocument : public Usul::Documents::Document,
                        public Usul::Interfaces::IBuildScene
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Usul::Math::Vec2ui Vec2ui;
  typedef Usul::Math::Vec2f Vec2f;
  typedef Usul::Math::Vec3f Vec3f;
  typedef std::pair<std::string,std::string> GridInfo;
  typedef std::vector<Vec3f> GridData;
  typedef std::map<GridInfo,GridData> GridMap;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ModflowDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  ModflowDocument();

  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &file ) const;
  virtual bool                canInsert ( const std::string &file ) const;
  virtual bool                canOpen   ( const std::string &file ) const;
  virtual bool                canSave   ( const std::string &file ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  /// Get the filters that correspond to what this document can read and write.
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersExport() const;

  /// Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller, Unknown *progress );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0  ) const;

protected:

  void                        _destroy();

  void                        _incrementProgress ( bool state, Unknown *progress, unsigned int &numerator, unsigned int denominator );

  void                        _readBasicPackageFile ( const std::string &name, Unknown *progress );
  void                        _readDiscretizationFile ( const std::string &name, Unknown *progress );
  void                        _readGrid ( const std::string &file, std::istream &in );

  void                        _seekToLine ( const std::string &word, std::istream &in );
  void                        _skipLines ( char c, std::istream &in );

  /// Do not copy.
  ModflowDocument ( const ModflowDocument & );
  ModflowDocument &operator = ( const ModflowDocument & );

  /// Use reference counting.
  virtual ~ModflowDocument();

private:

  unsigned int _numLayers;
  Vec2ui _gridSize;
  Vec2f _cellSize;
  GridMap _grids;
};



#endif // _POLYGON_MODEL_DOCUMENT_H_
