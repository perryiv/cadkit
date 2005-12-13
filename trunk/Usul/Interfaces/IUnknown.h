
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  Base class for all other interfaces.
///
///////////////////////////////////////////////////////////////////////////////

/*!
  @class IUnknown
  @abstract Base class for all other interfaces.
*/

#ifndef _USUL_INTERFACE_UNKNOWN_H_
#define _USUL_INTERFACE_UNKNOWN_H_

#include "Usul/Pointers/Pointers.h"


namespace Usul {
namespace Interfaces {


struct IUnknown
{
  public:
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IUnknown );

  /*!
    @enum Id for this interface.
    @abstract Unique ID for this interface
    @discussion Unique ID for this interface
   */
  enum { IID = 1075271105 };

  /*!
    @function queryInterface
    @abstract See if the interface is supported
    @param iid Unique ID to query for
    @result An IUnknown that implements given ID.
   */
  virtual IUnknown *      queryInterface ( unsigned long iid ) = 0;

  /*!
    @function ref
    @abstract Reference the interface.
   */
  virtual void            ref() = 0;

  /*!
    @function unref
    @abstract Unreference the interface.
   */
  virtual void            unref ( bool allowDeletion = true ) = 0;
};


}; // namespace Interfaces
}; // namespace Usul



/*!
  @defined USUL_DECLARE_IUNKNOWN_MEMBERS
  @discussion Macro to declare IUnknown member functions.
*/


#define USUL_DECLARE_IUNKNOWN_MEMBERS \
  virtual Usul::Interfaces::IUnknown *  queryInterface ( unsigned long iid ); \
  virtual void                          ref(); \
  virtual void                          unref ( bool allowDeletion = true )


/*!
  @defined USUL_IMPLEMENT_IUNKNOWN_MEMBERS
  @discussion Macro to implement IUnknown member functions (except queryInterface).  Typically the base class will be Usul::Base::Referenced.
*/

#define USUL_IMPLEMENT_IUNKNOWN_MEMBERS(class_name,base_class) \
  void class_name::ref()                        { base_class::ref(); } \
  void class_name::unref ( bool allowDeletion ) { base_class::unref ( allowDeletion ); }


#endif // _USUL_INTERFACE_UNKNOWN_H_
