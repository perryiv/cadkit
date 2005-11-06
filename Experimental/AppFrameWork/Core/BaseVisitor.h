
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

namespace AFW { namespace Core { class Application; class Window; class Group; class Frame; class MainWindow; class TextWindow; } }
namespace AFW { namespace Menus { class Button; class MenuBar; class MenuGroup; } }


namespace AFW {
namespace Core {


class APP_FRAME_WORK_EXPORT BaseVisitor : public Usul::Base::Referenced
{
public:

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BaseVisitor );

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Visit the objects.
  virtual void                        visit ( AFW::Core::Application * );
  virtual void                        visit ( AFW::Core::Window * );
  virtual void                        visit ( AFW::Core::Group * );
  virtual void                        visit ( AFW::Core::Frame * );
  virtual void                        visit ( AFW::Core::MainWindow * );
  virtual void                        visit ( AFW::Core::TextWindow * );
  virtual void                        visit ( AFW::Menus::Button * );
  virtual void                        visit ( AFW::Menus::MenuBar * );
  virtual void                        visit ( AFW::Menus::MenuGroup * );

protected:

  // Constructor
  BaseVisitor();

  // Use reference counting.
  virtual ~BaseVisitor();

  void                                _traverse ( AFW::Core::Window * );

private:

  // No copying.
  BaseVisitor ( const BaseVisitor & );
  BaseVisitor &operator = ( const BaseVisitor & );
};


} // namespace Core
} // namespace AFW


#endif //_APP_FRAME_WORK_BASE_VISITOR_CLASS_H_
