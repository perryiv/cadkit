
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface to notify of an elevation change.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_INTERFACES_ITILES_CHANGED_LISTENER_H__
#define __MINERVA_INTERFACES_ITILES_CHANGED_LISTENER_H__

#include "Usul/Interfaces/IUnknown.h"


namespace Minerva {
namespace Interfaces {


struct ITilesChangedListener : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( ITilesChangedListener );
  
  /// Id for this interface.
  enum { IID = 1809784420u };
  
  virtual void tileAddNotify     ( Usul::Interfaces::IUnknown::RefPtr childTile, Usul::Interfaces::IUnknown::RefPtr parentTile ) = 0;
  virtual void tileRemovedNotify ( Usul::Interfaces::IUnknown::RefPtr childTile, Usul::Interfaces::IUnknown::RefPtr parentTile ) = 0;
};


} // End namespace Interfaces
} // End namespace Minerva

#endif // __MINERVA_INTERFACES_ITILES_CHANGED_LISTENER_H__
