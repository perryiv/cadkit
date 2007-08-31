
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
//  Dockable progress bar container.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CADKIT_HELIOS_QT_CORE_PROGRESS_BAR_DOCK_H__
#define __CADKIT_HELIOS_QT_CORE_PROGRESS_BAR_DOCK_H__

#include "Usul/Base/Object.h"

class QVBoxLayout;
class QWidget;
class QDockWidget;

namespace Usul { namespace Interfaces { struct IUnknown; } }

namespace CadKit {
namespace Helios {
namespace Core {


class ProgressBarDock : public Usul::Base::Object
{
public:
  typedef Usul::Base::Object BaseClass;

  USUL_DECLARE_REF_POINTERS ( ProgressBarDock );

  ProgressBarDock ();

  QWidget* operator () ( QDockWidget* parent );

  Usul::Interfaces::IUnknown * createProgressBar ();
protected:
  virtual ~ProgressBarDock ();

private:
  QWidget     *_widget;
  QVBoxLayout *_layout;
};


} // namespace Core
} // namespace Helios
} // namespace CadKit

#endif // __CADKIT_HELIOS_QT_CORE_PROGRESS_BAR_DOCK_H__
