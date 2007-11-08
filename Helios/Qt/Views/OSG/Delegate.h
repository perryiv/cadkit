
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Default Qt Viewer delegate class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_QT_VIEWS_OSG_DELEGATE_H__
#define __CADKIT_HELIOS_QT_VIEWS_OSG_DELEGATE_H__

#include "Helios/Qt/Views/OSG/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"

#include <string>

namespace CadKit {
namespace Helios {
namespace Views {
namespace OSG {

class HELIOS_QT_VIEWS_OSG_EXPORT Delegate : public Usul::Base::Referenced,
                                            public Usul::Interfaces::IGUIDelegate
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Delegate );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  Delegate();

  /// Usul::Interfaces::IGUIDelegate
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );

protected: 

  // Do not copy.
  Delegate ( const Delegate & );
  Delegate &operator = ( const Delegate & );

  /// Use reference counting.
  virtual ~Delegate();


};

}
}
}
}

#endif // __CADKIT_HELIOS_QT_VIEWS_OSG_DELEGATE_H__
