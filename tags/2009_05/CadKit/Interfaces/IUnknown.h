
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///
///  IUnknown: Interface for reference counting and querying other interfaces.
///
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_INTERFACE_UNKNOWN_H_
#define _CADKIT_INTERFACE_UNKNOWN_H_


namespace CadKit
{
class IUnknown
{
public:

  /// Id for this interface.
  enum { IID = 1032561430 };

  /// Get the reference count.
  virtual unsigned long   getRefCount() const = 0;

  /// See if the interface is supported.
  virtual IUnknown *      queryInterface ( unsigned long iid ) = 0;

  /// Reference the interface.
  virtual void            ref() = 0;

  /// Unreference the interface.
  virtual void            unref() = 0;
};


///////////////////////////////////////////////////////////////////////////////
///
///  Function to query for an interface. For use inside general purpose 
///  smart-pointers.
///
///////////////////////////////////////////////////////////////////////////////

inline IUnknown *queryInterface ( unsigned long iid, IUnknown *unknown )
{
  return ( unknown ) ? unknown->queryInterface ( iid ) : 0x0;
}


}; // namespace CadKit


///////////////////////////////////////////////////////////////////////////////
///
///  Macro to declare IUnknown member functions.
///
///////////////////////////////////////////////////////////////////////////////

#define CADKIT_DECLARE_IUNKNOWN_MEMBERS \
public: \
  virtual unsigned long   getRefCount() const; \
  virtual IUnknown *      queryInterface ( unsigned long iid ); \
  virtual void            ref(); \
  virtual void            unref()


///////////////////////////////////////////////////////////////////////////////
///
///  Macro to implement IUnknown member functions (except queryInterface).
///  Typically the base class will be SlRefBase or some child thereof.
///
///////////////////////////////////////////////////////////////////////////////

#define CADKIT_IMPLEMENT_IUNKNOWN_MEMBERS(class_name,base_class) \
  unsigned long class_name::getRefCount() const { return base_class::getRefCount(); } \
  void          class_name::ref()               { base_class::ref(); } \
  void          class_name::unref()             { base_class::unref(); }


#endif // _CADKIT_INTERFACE_UNKNOWN_H_
