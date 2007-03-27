
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all visitors.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_BASE_VISITOR_CLASS_H_
#define _APP_FRAME_WORK_BASE_VISITOR_CLASS_H_

#include "AppFrameWork/Core/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/RecursiveMutex.h"
#include "Usul/Threads/Guard.h"

namespace AFW { namespace Core { class Application; } }
namespace AFW { namespace Windows { class Window; class Group; class StatusBar; } }
namespace AFW { namespace Windows { class Frame; class MainWindow; class TextWindow; } }
namespace AFW { namespace Menus { class Button; class MenuBar; class MenuGroup; } }
namespace AFW { namespace Dialogs { class Dialog; } }


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT BaseVisitor : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Threads::RecursiveMutex Mutex;
  typedef Usul::Threads::Guard < Mutex > Guard;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BaseVisitor );

  // Type-id definition.
  USUL_DECLARE_TYPE_ID ( BaseVisitor );

  // Access to the mutex.
  Mutex &                             mutex() const;

  // Visit the objects.
  virtual void                        visit ( AFW::Core::Application * );
  virtual void                        visit ( AFW::Windows::Window * );
  virtual void                        visit ( AFW::Windows::Group * );
  virtual void                        visit ( AFW::Windows::Frame * );
  virtual void                        visit ( AFW::Windows::MainWindow * );
  virtual void                        visit ( AFW::Windows::TextWindow * );
  virtual void                        visit ( AFW::Windows::StatusBar * );
  virtual void                        visit ( AFW::Menus::Button * );
  virtual void                        visit ( AFW::Menus::MenuBar * );
  virtual void                        visit ( AFW::Menus::MenuGroup * );
  virtual void                        visit ( AFW::Dialogs::Dialog * );

protected:

  // Constructor
  BaseVisitor();

  // Use reference counting.
  virtual ~BaseVisitor();

  void                                _traverse ( AFW::Windows::Window * );

private:

  // No copying.
  BaseVisitor ( const BaseVisitor & );
  BaseVisitor &operator = ( const BaseVisitor & );

  mutable Mutex _mutex;
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_BASE_VISITOR_CLASS_H_
