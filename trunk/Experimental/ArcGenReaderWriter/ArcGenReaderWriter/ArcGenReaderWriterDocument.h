
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// Polyline format example
//
//
//  Format:
//  -------------------
//  ID,FIELD,FIELD
//  id,value,value
//  x,y
//  x,y
//  END
//
//  Example:
//  --------------------
//  ID,Street,Streettype
//  1,Church,Street
//  34.5,-14.3
//  12.8,-19.6
//  END
//  
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for mpd files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _ARC_GEN_READER_WRITER_DOCUMENT_H_
#define _ARC_GEN_READER_WRITER_DOCUMENT_H_

#include "Usul/Documents/Document.h"

#include "Usul/Interfaces/IArcGenReaderWriter.h"

#include <string>

class ArcGenReaderWriterDocument : public Usul::Documents::Document,
                                   public Usul::Interfaces::IArcGenReaderWriter
                                  
{
public:
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef std::vector< osg::Vec3 > Positions;
 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ArcGenReaderWriterDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  ArcGenReaderWriterDocument();

  /// Return true if this document can do it.
  virtual bool             canExport ( const std::string &file ) const;
  virtual bool             canInsert ( const std::string &file ) const;
  virtual bool             canOpen   ( const std::string &file ) const;
  virtual bool             canSave  ( const std::string &file ) const;

  /// Clear any existing data.
  virtual void             clear ( Unknown *caller = 0x0 );

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
  ArcGenReaderWriterDocument ( const ArcGenReaderWriterDocument & );
  ArcGenReaderWriterDocument &operator = ( const ArcGenReaderWriterDocument & );

  // Writers
  void                     _writePolylineZ ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 ) const;
	void                     _writeShapeFile ( const std::string& filename ) const;

  /// Use reference counting.
  virtual ~ArcGenReaderWriterDocument();

private:

  Positions                   _positions;
  double                      _measurement;
};



#endif // _ARC_GEN_READER_WRITER_DOCUMENT_H_
