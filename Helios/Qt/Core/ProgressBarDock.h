
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
#include "Usul/Interfaces/GUI/IProgressBar.h"

#include "QtCore/QObject"

#include <list>

class QVBoxLayout;
class QWidget;
class QDockWidget;
class QProgressBar;

namespace Usul { namespace Interfaces { struct IUnknown; } }

namespace CadKit {
namespace Helios {
namespace Core {


class ProgressBarDock : public QObject,
                        public Usul::Base::Object
                        
{
  Q_OBJECT;
public:
  typedef Usul::Base::Object BaseClass;

  USUL_DECLARE_REF_POINTERS ( ProgressBarDock );

  ProgressBarDock ();

  QWidget* operator () ( QDockWidget* parent );

  Usul::Interfaces::IUnknown * createProgressBar ();

protected:
  virtual ~ProgressBarDock ();

protected slots:
  void _updateProgressBars ();

private:

  // Wrapper around QProgressBar.
  class ProgressBar : public Usul::Base::Referenced,
                      public Usul::Interfaces::IProgressBar
  {
  public:
    typedef Usul::Base::Referenced BaseClass;

    USUL_DECLARE_QUERY_POINTERS ( ProgressBar );

    ProgressBar ();

    void ref ();
    void unref ( bool allowDeletion = true );

    Usul::Interfaces::IUnknown* queryInterface ( unsigned long iid );

    // Create a progress bar.
    QWidget* operator () ( QWidget *parent );

    // Set the progress bar.
    void     progressBar ( QProgressBar * );

  protected:
    virtual ~ProgressBar ();

    virtual void showProgressBar();
    virtual void setTotalProgressBar ( unsigned int value );
    virtual void updateProgressBar ( unsigned int value );
    virtual void hideProgressBar();

  private:
    QProgressBar *_progressBar;
  };

  typedef std::list < ProgressBar::RefPtr > ProgressBars;

  QWidget      *_widget;
  QVBoxLayout  *_layout;
  ProgressBars  _progressBars;
};


} // namespace Core
} // namespace Helios
} // namespace CadKit

#endif // __CADKIT_HELIOS_QT_CORE_PROGRESS_BAR_DOCK_H__
