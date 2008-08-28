
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for triangle models.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _POINT_MODEL_DOCUMENT_H_
#define _POINT_MODEL_DOCUMENT_H_

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Types/Types.h"

#include "XmlTree/Document.h"

#include "OsgTools/Points/PointSet.h"

#include "osg/Material"
#include "osg/Vec4"

#include <string>

using namespace Usul::Types;


class PointDocument : public Usul::Documents::Document,
                      public Usul::Interfaces::IBuildScene,
                      public Usul::Interfaces::IMenuAdd
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef OsgTools::Points::PointSet PointSet;
  typedef Usul::Interfaces::IUnknown IUnknown;
  typedef std::pair < unsigned int, unsigned int > Progress;
  typedef std::pair < bool, Usul::Math::Vec3ui > HeaderResult;
  typedef XmlTree::Document::Attributes Attributes;
  typedef XmlTree::Document::Children Children;
  typedef std::list< PointSet::ValidRefPtr > PointSets;

  /// Type information.
  USUL_DECLARE_TYPE_ID ( PointDocument );

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( PointDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  PointDocument();

  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &ext ) const;
  virtual bool                canInsert ( const std::string &ext ) const;
  virtual bool                canOpen   ( const std::string &ext ) const;
  virtual bool                canSave   ( const std::string &ext ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  /// Get the filters that correspond to what this document can read and write.
  virtual Filters             filtersExport() const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;

  /// Read the file and add it to document's data.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

   /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

  //Usul::Interfaces::IMenuAdd
  void                        menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );
  
protected:

  typedef std::vector< unsigned int > Connected;

  /// Do not copy.
  PointDocument ( const PointDocument & );
  PointDocument &operator = ( const PointDocument & );

  /// Use reference counting.
  virtual ~PointDocument();

  void                        _read( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  void                        _parseXML( XmlTree::Node &node, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  bool                        _parseHeader( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                        _fastReadAndSetBounds( const std::string &filename, const std::string &binaryFilename, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                        _readPoint3DFile( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                        _readAndSetBounds( const std::string &filename, const std::string &binaryFilename, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                        _readBinaryRestartFile( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                        _buildVectors( Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                        _split( Unknown *caller = 0x0, Unknown *progress = 0x0 );
  void                        _editPointColor();


private:

  PointSets                       _pointSets;
  PointSet::ValidRefPtr           _pointSet;
  Usul::Types::Uint64             _numPoints;
  osg::ref_ptr< osg::Material >   _material;
  osg::Vec4f                      _color;
  std::string                     _workingDir;
  
};


#endif // _POINT_MODEL_DOCUMENT_H_
