
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  DbBaseSource: Base class for other source database classes.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_DATABASE_BASE_LIBRARY_SOURCE_DATABASE_CLASS_H_
#define _CADKIT_DATABASE_BASE_LIBRARY_SOURCE_DATABASE_CLASS_H_

#include "DbBaseObject.h"

#include "Interfaces/IDataSource.h"
#include "Interfaces/IScale.h"

#include "Standard/SlRefPtr.h"
#include "Standard/SlMatrix44.h"


namespace CadKit
{
class DB_BASE_API DbBaseSource : public DbBaseObject, 
                                 public IDataSource,
                                 public IScaleDouble,
                                 public IScaleFloat
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
  //  IDataSource interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Set the data target.
  virtual void            setDataTarget ( IUnknown *target );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IScaleDouble interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Modify the current scale.
  virtual void            scale ( double x, double y, double z );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  IScaleDouble interface.
  //
  /////////////////////////////////////////////////////////////////////////////

  // Modify the current scale.
  virtual void            scale ( float x, float y, float z );

protected:

  SlRefPtr<IUnknown> _target;
  SlMatrix44d _scale;
  mutable bool _needToScale;

  DbBaseSource();
  virtual ~DbBaseSource();

  void                    _applyScaleOnce ( SlMatrix44d &scale ) const;
  void                    _applyScaleOnce ( SlMatrix44f &scale ) const;

  bool                    _doWeNeedToScale() const { return _needToScale; }

  void                    _setNeedToScale ( bool state ) { _needToScale = state; }

  SL_DECLARE_REFERENCE_POINTER ( DbBaseSource );
  SL_DECLARE_CLASS ( DbBaseSource, 1033073996 );
};

}; // namespace CadKit

#endif // _CADKIT_DATABASE_BASE_LIBRARY_SOURCE_DATABASE_CLASS_H_
