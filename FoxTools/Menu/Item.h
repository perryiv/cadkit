
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for all menu items.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_MENU_ITEM_CLASS_H_
#define _FOX_TOOLS_MENU_ITEM_CLASS_H_

#include "FoxTools/Export/Export.h"
#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

#include <string>


namespace FoxTools {
namespace Menu {


class FOX_TOOLS_EXPORT Item : public Usul::Base::Referenced
{
public:

  // Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Base::Referenced UserData;
  typedef USUL_REF_POINTER(UserData) UserDataPtr;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Item );

  // Set/get the name.
  const std::string &   name() const;
  void                  name ( const std::string & );

  // Clear the item.
  virtual void          clear() = 0;

  // Set/get the user-data.
  UserData *            userData();
  const UserData *      userData() const;
  void                  userData ( UserData * );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Predicate to determine if another item is equal to this
  //
  /////////////////////////////////////////////////////////////////////////////

  struct IsEqual
  {
    IsEqual( const std::string& name ) :
    _name ( name )
    { }

    template < class T > bool operator() ( const T& item ) const
    {
      // Make copies
      std::string thisCopy ( _name );
      std::string itemCopy ( item->_name );

      // Remove any & from the this copy
      std::string::iterator end = std::remove ( thisCopy.begin(), thisCopy.end(), '&' );
      thisCopy.resize( end - thisCopy.begin() );

      // Remove any & from the item copy
      end = std::remove ( itemCopy.begin(), itemCopy.end(), '&' );
      itemCopy.resize( end - itemCopy.begin() );

      // Make lower case
      std::transform ( thisCopy.begin(), thisCopy.end(), thisCopy.begin(), ::tolower );
      std::transform ( itemCopy.begin(), itemCopy.end(), itemCopy.begin(), ::tolower );

      // Are they equal?
      return thisCopy == itemCopy;
    }

  private:
    IsEqual();

    std::string _name;
  };

protected:

  // Constructors.
  Item();
  Item ( const std::string &name );

  // Use reference counting.
  virtual ~Item();

private:

  // No copying.
  Item ( const Item & );
  Item &operator = ( const Item & );

  std::string _name;
  UserDataPtr _data;
};


}; // namespace Menu
}; // namespace FoxTools


#endif // _FOX_TOOLS_MENU_ITEM_CLASS_H_
