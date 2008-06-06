
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for Esri Shape Files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ESRI_SHAPE_FILE_DOCUMENT_H_
#define _ESRI_SHAPE_FILE_DOCUMENT_H_

#include "Usul/Documents/Document.h"

#include "Usul/Interfaces/IArcGenReaderWriter.h"

#include <string>

class OGRSFDriver;
class OGRLayer;

class EsriShapeFileDocument : public Usul::Documents::Document,
                              public Usul::Interfaces::IArcGenReaderWriter
                                  
{
public:
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef std::vector< osg::Vec3 > Positions;
 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( EsriShapeFileDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  EsriShapeFileDocument();

  /// Return true if this document can do it.
  virtual bool             canExport ( const std::string &file ) const;
  virtual bool             canInsert ( const std::string &file ) const;
  virtual bool             canOpen   ( const std::string &file ) const;
  virtual bool             canSave  ( const std::string &file ) const;

  /// Clear any existing data.
  virtual void             clear ( Unknown *caller = 0x0 );

	/// Get the driver.
	OGRSFDriver*             driver() const;

  /// Get the filters that correspond to what this document can read and write.
  virtual Filters          filtersOpen()   const;
  virtual Filters          filtersSave()   const;
  virtual Filters          filtersInsert() const;
  virtual Filters          filtersExport() const;
  
  /// Read the document.
  virtual void             read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void             write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

  // set the positions vector
  virtual void             setPolyLineVertices ( Positions p );

  // set the recorded measurement value
  virtual void             measurement( double m );

protected:

  /// Do not copy.
  EsriShapeFileDocument ( const EsriShapeFileDocument & );
  EsriShapeFileDocument &operator = ( const EsriShapeFileDocument & );

  // Writers
	void                     _writeShapeFile ( const std::string& filename ) const;

	static void              _addLine ( OGRLayer* layer, const Positions& p );

  /// Use reference counting.
  virtual ~EsriShapeFileDocument();

private:

	OGRSFDriver *_driver;

  Positions                   _positions;
  double                      _measurement;
};



#endif // _ESRI_SHAPE_FILE_DOCUMENT_H_
