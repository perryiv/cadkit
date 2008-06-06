
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __ASSEMBLY_H__
#define __ASSEMBLY_H__

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IAssembly.h"
#include "Usul/Interfaces/ISceneElement.h"

#include <vector>

class Assembly : public Usul::Base::Referenced,
                 public Usul::Interfaces::IAssembly
{
public:
  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::ISceneElement ISceneElement;
  typedef ISceneElement::RefPtr ISceneElementPtr;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Assembly );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  Assembly();

  /// Add a sceneElement to the end.
  void          appendSceneElement ( ISceneElement * );

protected:
  virtual ~Assembly();

  // Usul::Interfaces::IAssembly
  virtual Usul::Interfaces::ISceneElement*   at ( unsigned int i );
  virtual unsigned int                       size () const;

private:

  std::vector< ISceneElementPtr > _sceneElements;
};


#endif // __ASSEMBLY_H__
