
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbBaseObject: Base class for other target database classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_BASE_LIBRARY_BASE_OBJECT_CLASS_H_
#define _CADKIT_DATABASE_BASE_LIBRARY_BASE_OBJECT_CLASS_H_

#include "DbBaseApi.h"

#include "Interfaces/IControlled.h"
#include "Interfaces/IClientData.h"
#include "Interfaces/IZeroRange.h"

#include "Standard/SlRefBase.h"
#include "Standard/SlQueryPtr.h"
#include "Standard/SlTruncateFunctor.h"

#include "Interfaces/IMessageNotify.h"
#include "Interfaces/IFormatAttribute.h"

#ifndef _CADKIT_USE_PRECOMPILED_HEADERS
# include <string>
# include <map>
# include <memory>
#endif


namespace CadKit
{
class DB_BASE_API DbBaseObject : public SlRefBase, 
                                 public IControlled,
                                 public IGenericClientData,
                                 public IAssemblyClientData,
                                 public IPartClientData,
                                 public IInstanceClientData,
                                 public IGroupClientData,
                                 public ILodClientData,
                                 public IShapeClientData,
                                 public IZeroRangeFloat,
                                 public IFormatAttribute
{
public:

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IUnknown interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // See if the interface is supported.
  virtual IUnknown *      queryInterface ( unsigned long iid ) = 0;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IControlled interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set the controller.
  virtual void            setController ( IUnknown *controller );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IClientData interfaces.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the client data.
  virtual const void *    getClientData ( void * const key ) const;
  virtual const void *    getClientData ( const AssemblyHandle key ) const;
  virtual const void *    getClientData ( const PartHandle key ) const;
  virtual const void *    getClientData ( const InstanceHandle key ) const;
  virtual const void *    getClientData ( const GroupHandle key ) const;
  virtual const void *    getClientData ( const LodHandle key ) const;
  virtual const void *    getClientData ( const ShapeHandle key ) const;

  // Set the client data.
  virtual void            setClientData ( void * const key,         const void *data );
  virtual void            setClientData ( const AssemblyHandle key, const void *data );
  virtual void            setClientData ( const PartHandle key,     const void *data );
  virtual void            setClientData ( const InstanceHandle key, const void *data );
  virtual void            setClientData ( const GroupHandle key,    const void *data );
  virtual void            setClientData ( const LodHandle key,      const void *data );
  virtual void            setClientData ( const ShapeHandle key,    const void *data );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IZeroRangeFloat interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Get the range that gets truncated to zero.
  virtual bool            getZeroRange ( float &negative, float &positive ) const;

  // Set the range that gets truncated to zero.
  virtual void            setZeroRange ( const float &negative, const float &positive );

protected:

  typedef std::map<const void *,         const void *> GenericClientDataMap;
  typedef std::map<const AssemblyHandle, const void *> AssemblyClientDataMap;
  typedef std::map<const PartHandle,     const void *> PartClientDataMap;
  typedef std::map<const InstanceHandle, const void *> InstanceClientDataMap;
  typedef std::map<const GroupHandle,    const void *> GroupClientDataMap;
  typedef std::map<const LodHandle,      const void *> LodClientDataMap;
  typedef std::map<const ShapeHandle,    const void *> ShapeClientDataMap;

  std::auto_ptr<GenericClientDataMap>  _genericClientDataMap;
  std::auto_ptr<AssemblyClientDataMap> _assemblyClientDataMap;
  std::auto_ptr<PartClientDataMap>     _partClientDataMap;
  std::auto_ptr<InstanceClientDataMap> _instanceClientDataMap;
  std::auto_ptr<GroupClientDataMap>    _groupClientDataMap;
  std::auto_ptr<LodClientDataMap>      _lodClientDataMap;
  std::auto_ptr<ShapeClientDataMap>    _shapeClientDataMap;

  SlRefPtr<IUnknown> _controller;
  SlQueryPtr<IMessageNotify> _messageNotify;
  SlTruncateFunctor<float> _truncate;

  DbBaseObject();
  virtual ~DbBaseObject();

  void                    _clearClientDataMaps();

  bool                    _notifyError    ( const std::string &message, const unsigned long &id );
  bool                    _notifyMessage  ( const std::string &message, const unsigned long &id, const MessageType &type );
  bool                    _notifyProgress ( const std::string &message, const unsigned long &priority = 1 );
  bool                    _notifyWarning  ( const std::string &message, const unsigned long &id );

  SL_DECLARE_REFERENCE_POINTER ( DbBaseObject );
  SL_DECLARE_CLASS ( DbBaseObject, 1034649271 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_BASE_LIBRARY_BASE_OBJECT_CLASS_H_
