
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Viewer interface.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _JSAL_INTERFACE_VIEWER_H_
#define _JSAL_INTERFACE_VIEWER_H_

#include "Usul/Interfaces/IUnknown.h"

namespace SAL { namespace Interfaces { struct INode; }; };


namespace JSAL {
namespace Interfaces {


struct IViewer : public Usul::Interfaces::IUnknown
{
  typedef SAL::Interfaces::INode Node;

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IViewer );

  /// Id for this interface.
  enum { IID = 2078593401u };

  // Set/get the scene.
  virtual void              scene ( Node * ) = 0;
  virtual Node *            scene() = 0;
};


}; // namespace Interfaces
}; // namespace JSAL


#endif // _JSAL_INTERFACE_VIEWER_H_
