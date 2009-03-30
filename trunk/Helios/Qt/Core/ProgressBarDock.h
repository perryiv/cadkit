
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
#include "Usul/Interfaces/GUI/IStatusBar.h"

#include "QtCore/QObject"

#include <list>

class QVBoxLayout;
class QScrollArea;
class QDockWidget;
class QProgressBar;
class QLabel;

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

  Usul::Interfaces::IUnknown::RefPtr      createProgressBar ( bool waitIfNotGuiThread );

protected:
  virtual ~ProgressBarDock ();

protected slots:
  void _updateProgressBars ();

private:

  // Wrapper around QProgressBar.
  class ProgressBar : public Usul::Base::Object,
                      public Usul::Interfaces::IProgressBar,
                      public Usul::Interfaces::IStatusBar
  {
  public:

    typedef Usul::Base::Object BaseClass;

    USUL_DECLARE_QUERY_POINTERS ( ProgressBar );
    USUL_DECLARE_TYPE_ID ( ProgressBar );

    ProgressBar ();

    void ref();
    void unref ( bool allowDeletion = true );

    Usul::Interfaces::IUnknown* queryInterface ( unsigned long iid );

    // Create a progress bar.
    void operator () ( QVBoxLayout *layout );

    // Is it created?
    bool     isCreated() const;

    // Set the progress bar.
    void     setProgressBar ( QProgressBar * );

  protected:

    virtual ~ProgressBar();

    virtual void showProgressBar();
    virtual void setTotalProgressBar ( unsigned int value );
    virtual void updateProgressBar ( unsigned int value );
    virtual void hideProgressBar();

    // Set the status bar text.
    virtual void setStatusBarText ( const std::string &text, bool force );

  private:
    QProgressBar *_progressBar;
    QLabel *_label;
    QVBoxLayout *_layout;
    QVBoxLayout *_parentLayout;
    bool _created;
  };

  void              _destroy();

  typedef std::list < ProgressBar::RefPtr > ProgressBars;

  QScrollArea  *_scrollArea;
  QVBoxLayout  *_layout;
  ProgressBars  _progressBars;
  bool _canCreate;
};


} // namespace Core
} // namespace Helios
} // namespace CadKit

#endif // __CADKIT_HELIOS_QT_CORE_PROGRESS_BAR_DOCK_H__
