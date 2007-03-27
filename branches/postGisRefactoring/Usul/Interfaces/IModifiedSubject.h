
#ifndef __USUL_INTERFACES_IMODIFIEDSUBJECT_H__
#define __USUL_INTERFACES_IMODIFIEDSUBJECT_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {

  struct IModifiedObserver;

struct IModifiedSubject : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IModifiedSubject );

  /// Id for this interface.
  enum { IID = 14750000u };

  virtual void addModifiedObserver    ( Usul::Interfaces::IModifiedObserver* observer ) = 0;
  virtual void removeModifiedObserver ( Usul::Interfaces::IModifiedObserver* observer ) = 0;

}; // struct IModifiedSubject


} // end namespace Interfaces
} // end namespace Usul


#endif /* __USUL_INTERFACES_IMODIFIEDSUBJECT_H__ */
