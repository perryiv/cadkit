
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
#include "Usul/Interfaces/IColorEditor.h"

#include <string>

namespace CadKit {
namespace Helios {
namespace Views {
namespace OSG {

class Viewer;
  
class HELIOS_QT_VIEWS_OSG_EXPORT Delegate : public Usul::Base::Referenced,
                                            public Usul::Interfaces::IGUIDelegate,
                                            public Usul::Interfaces::IColorEditor
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef CadKit::Helios::Views::OSG::Viewer QtViewer;
  
  // Non-ref'ing smart-pointers that throw if given null.
  typedef Usul::Pointers::Configs::NoRefCountingNullThrows Policy;
  typedef Usul::Pointers::SmartPointer < QtViewer, Policy > QtViewerPtr;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Delegate );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  Delegate();

  /// Usul::Interfaces::IGUIDelegate
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );

  // Usul::Interfaces::IColorEditor
  // Get the color.
  virtual bool                editColor ( Usul::Math::Vec4f& color );

protected: 

  // Do not copy.
  Delegate ( const Delegate & );
  Delegate &operator = ( const Delegate & );

  /// Use reference counting.
  virtual ~Delegate();

  // Make the viewer.
  QtViewer*                   _makeViewer ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );
  
  // Build the scene.
  void                        _buildScene ( QtViewer &viewer, Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );
  
  // Show the viewer.
  void                        _restoreStateAndShow ( QtViewer &viewer );
};

}
}
}
}

#endif // __CADKIT_HELIOS_QT_VIEWS_OSG_DELEGATE_H__
