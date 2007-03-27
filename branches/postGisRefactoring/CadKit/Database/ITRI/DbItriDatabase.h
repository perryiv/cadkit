
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbItriDatabase: Represents an ITRI database (list of facets).
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_ITRI_LIBRARY_DATABASE_H_
#define _CADKIT_DATABASE_ITRI_LIBRARY_DATABASE_H_

#include "DbItriApi.h"

#include "Database/Base/DbBaseTarget.h"

#include "Interfaces/ITriangleAppend.h"
#include "Interfaces/IFileExtension.h"
#include "Interfaces/IDataWrite.h"
#include "Interfaces/IOutputAttribute.h"
#include "Interfaces/IOutputPrecision.h"

#include "Standard/SlStack.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
# include <list>
#endif


namespace CadKit
{
class DB_ITRI_API DbItriDatabase : public DbBaseTarget,
                                   public ITriangleAppendFloat,
                                   public IFileExtension,
                                   public IDataWrite,
                                   public IOutputAttribute,
                                   public IOutputPrecision
{
public:

  DbItriDatabase();

  // IUnknown interface.
  CADKIT_DECLARE_IUNKNOWN_MEMBERS;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IDataTarget interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Tell the target it is about to receive data.
  virtual bool            dataTransferStart ( IUnknown *caller );

  // Tell the target it is done receiving data.
  virtual bool            dataTransferEnd ( IUnknown *caller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IFormatAttribute interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Does the format have the attribute?
  virtual bool            isAttributeSupported ( const FormatAttribute &attribute ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  ITriangleAppendFloat interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Append the triangle.
  virtual bool            appendTriangle ( float t0v0, float t0v1, float t0v2, 
                                           float t1v0, float t1v1, float t1v2,
                                           float t2v0, float t2v1, float t2v2,
                                           IUnknown *caller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IFileExtension interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the file extension.
  virtual std::string     getFileExtension() const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IDataWrite interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Write the data.
  virtual bool            writeData ( const std::string &filename );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IOutputAttribute interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set the output attribute.
  virtual bool            setOutputAttribute ( const FormatAttribute &attribute );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IOutputPrecision interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set the number of decimals to output.
  virtual bool            setOutputNumDecimals ( unsigned int numDecimals );

protected:

  typedef SlVec3<SlVec3f> Triangle;
  typedef std::list<Triangle> Triangles;
  Triangles _triangles;
  unsigned int _numDecimals;

  virtual ~DbItriDatabase();

  SL_DECLARE_REFERENCE_POINTER ( DbItriDatabase );
  SL_DECLARE_DYNAMIC_CLASS ( DbItriDatabase, 1038408962 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_ITRI_LIBRARY_DATABASE_H_
