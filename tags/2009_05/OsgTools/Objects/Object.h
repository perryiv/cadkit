
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for objects.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_OBJECTS_BASE_OBJECT_H_
#define _OSG_TOOLS_OBJECTS_BASE_OBJECT_H_

#include "OsgTools/Export.h"
#include "OsgTools/Declarations.h"
#include "OsgTools/Objects/Macros.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IUnknown.h"


namespace OsgTools {
namespace Objects {


class OSG_TOOLS_EXPORT Object : public Usul::Base::Object
{
public:

  // Typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef BaseClass::Mutex Mutex;
  typedef BaseClass::Guard Guard;

  // Boiler-plate code.
  OSG_TOOLS_OBJECTS_DECLARE_CLASS ( Object );

  // Dirty flags.
  struct Dirty
  {
    enum
    {
      VERTICES   = 0x01, 
      NORMALS    = 0x02, 
      COLORS     = 0x04, 
      TEX_COORDS = 0x08, 
      ALL        = VERTICES | NORMALS | COLORS | TEX_COORDS
    };
  };

  // Set/get the dirty flag.
  bool                                isDirty() const;
  bool                                isDirty  ( unsigned char bits ) const;
  void                                setDirty ( unsigned char bits, bool state );

  // Update the object.
  virtual void                        update ( Usul::Interfaces::IUnknown *caller );

  // Set/get the parent. Could be null.
  void                                parent ( Object::RefPtr );
  const Object::RefPtr                parent() const;
  Object::RefPtr                      parent();

protected:

  Object();
  Object ( const Object & );
  virtual ~Object();

private:

  // No assignment.
  Object &operator = ( const Object & );

  void                                _destroy();

  Object::RefPtr _parent;
  unsigned char _dirty;
};


}; // namespace Objects
}; // namespace OsgTools


#endif // _OSG_TOOLS_OBJECTS_BASE_OBJECT_H_
