
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all menu items window class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _APP_FRAME_WORK_MENU_ITEM_CLASS_H_
#define _APP_FRAME_WORK_MENU_ITEM_CLASS_H_

#include "AppFrameWork/Core/Export.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>


namespace AFW {
namespace Menus {

class MenuGroup;

class APP_FRAME_WORK_EXPORT MenuItem : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MenuItem );

  // Possible flags
  enum
  {
    DIRTY = 0x01
  };

  // Set/get the dirty flag.
  void                                dirty ( bool );
  bool                                dirty() const;

  // Return parent.
  const MenuGroup *                   parent() const { return _parent; }
  MenuGroup *                         parent()       { return _parent; }

  // Set/get the text.
  const std::string &                 text() const { return _text; }
  void                                text ( const std::string &t ) { _text = t; }

  // Set/get the character to underline.
  unsigned short                      underline() const { return _underline; }
  void                                underline ( unsigned short u ) { _underline = u; }

protected:

#if 0
  Now I am thinking objects and groups. Groups inherit from Objects. 
  Everything else is detail. Layout classes are a kind of group. 
  If you add an object to a group that does not make sense, it is 
  ignored or causes an error at runtime.
#endif

  // Constructors
  MenuItem();
  MenuItem ( const std::string &text, unsigned short underline = 0 );

  // Use reference counting.
  virtual ~MenuItem();

  void                                _setParent ( MenuGroup * );

private:

  // No copying.
  MenuItem ( const MenuItem & );
  MenuItem &operator = ( const MenuItem & );

  // Calls _setParent()
  friend class MenuGroup;

  std::string _text;
  unsigned short _underline;
  unsigned int _flags;
  MenuGroup *_parent;
};


} // namespace Menus
} // namespace AFW


#endif //_APP_FRAME_WORK_MENU_ITEM_CLASS_H_
