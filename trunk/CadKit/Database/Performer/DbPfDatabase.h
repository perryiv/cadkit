
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbPfDatabase: Performer database wrapper class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_PERFORMER_LIBRARY_DATABASE_H_
#define _CADKIT_DATABASE_PERFORMER_LIBRARY_DATABASE_H_

#include "DbPfApi.h"

#include "Database/Base/DbBaseTarget.h"

#include "Interfaces/IEntityNotify.h"
#include "Interfaces/IInstanceQuery.h"
#include "Interfaces/IFileExtension.h"
#include "Interfaces/IDataWrite.h"
#include "Interfaces/IOutputAttribute.h"

#include "Standard/SlStack.h"
#include "Standard/SlErrorPolicy.h"

#include <string>
#include <map>
#include <stdexcept>

class pfGroup;
class pfLOD;
class pfGeoSet;
class pfGeoState;


namespace CadKit
{
class DB_PERFORMER_API DbPfDatabase : public DbBaseTarget,
                                      public IAssemblyNotify,
                                      public IPartNotify,
                                      public IInstanceNotify,
                                      public ILodNotify,
                                      public IShapeNotify,
                                      public ISetNotify,
                                      public IFileExtension,
                                      public IDataWrite,
                                      public IOutputAttribute
{
public:

  DbPfDatabase();

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
  //  IAssemblyNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // End the assembly.
  virtual bool            endEntity ( AssemblyHandle assembly, IUnknown *caller );

  // Start the assembly.
  virtual bool            startEntity ( AssemblyHandle assembly, IUnknown *caller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IPartNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // End the part.
  virtual bool            endEntity ( PartHandle assembly, IUnknown *caller );

  // Start the part.
  virtual bool            startEntity ( PartHandle assembly, IUnknown *caller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IInstanceNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // End the instance.
  virtual bool            endEntity ( InstanceHandle assembly, IUnknown *caller );

  // Start the instance.
  virtual bool            startEntity ( InstanceHandle assembly, IUnknown *caller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  ILodNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // End the lod.
  virtual bool            endEntity ( LodHandle lod, IUnknown *caller );

  // Start the lod.
  virtual bool            startEntity ( LodHandle lod, IUnknown *caller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IShapeNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // End the shape.
  virtual bool            endEntity ( ShapeHandle shape, IUnknown *caller );

  // Start the shape.
  virtual bool            startEntity ( ShapeHandle shape, IUnknown *caller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  ISetNotify interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // End the set.
  virtual bool            endEntity ( SetHandle set, IUnknown *caller );

  // Start the set.
  virtual bool            startEntity ( SetHandle set, IUnknown *caller );

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

protected:

  typedef CadKit::ErrorPolicy::Throw < std::out_of_range > StackErrorPolicy;
  typedef SlStack < pfGroup *, StackErrorPolicy > GroupStack;
  std::auto_ptr<GroupStack> _groupStack;
  FormatAttribute _outputAttribute;

  virtual ~DbPfDatabase();

  bool                    _addAttributes ( IUnknown *caller, ShapeHandle shape, pfGeoState *state );
  bool                    _addDataSets   ( IUnknown *caller, ShapeHandle shape, pfGeoSet *geometry );
  bool                    _addVertices   ( IUnknown *caller, ShapeHandle shape, pfGeoSet *geometry );
  bool                    _addNormals    ( IUnknown *caller, ShapeHandle shape, pfGeoSet *geometry );
  bool                    _addColors     ( IUnknown *caller, ShapeHandle shape, pfGeoSet *geometry );
  bool                    _addTexCoords  ( IUnknown *caller, ShapeHandle shape, pfGeoSet *geometry );

  void                    _clearGroupStack();

  pfGroup *               _findGroup ( InstanceHandle instance, IInstanceQueryFloat *query ) const;

  pfGroup *               _getRoot() const;

  bool                    _hasColorAttribute ( IUnknown *caller, ShapeHandle shape ) const;

  void                    _pushGroup ( pfGroup *group );
  void                    _popGroup();

  void                    _setLodParameters ( pfLOD *lod ) const;

  SL_DECLARE_REFERENCE_POINTER ( DbPfDatabase );
  SL_DECLARE_DYNAMIC_CLASS ( DbPfDatabase, 1032736074 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_PERFORMER_LIBRARY_DATABASE_H_
