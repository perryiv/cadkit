
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Base class for most classes in this library.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_BASE_OBJECT_CLASS_H_
#define _MODFLOW_MODEL_BASE_OBJECT_CLASS_H_

#include "ModflowModel/CompileGuard.h"

#include "OsgTools/Configure/OSG.h"

#include "Usul/Base/Observed.h"
#include "Usul/Interfaces/IStringGridGet.h"
#include "Usul/Properties/Attribute.h"

#include "osg/Referenced"


namespace Modflow {
namespace Base {


class BaseObject : public Usul::Base::Observed,
                   public Usul::Interfaces::IUnknown
{
public:

  // Type information.
  USUL_DECLARE_TYPE_ID ( BaseObject );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( BaseObject );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Useful typedefs.
  typedef Usul::Base::Observed BaseClass;
  typedef Usul::Pointers::WeakPointer < BaseObject > WeakPtr;
  typedef Usul::Properties::Attribute < WeakPtr, osg::Referenced > UserData;
  typedef Usul::Interfaces::IStringGridGet IStringGridGet;
  typedef IStringGridGet::StringRow StringRow;
  typedef IStringGridGet::StringGrid StringGrid;

  // Get the class name.
  virtual std::string         className() const;

  // Clear the object.
  virtual void                clear() = 0;

  // Helper functions to make string rows.
  static StringRow            makeStringRow ( const std::string & );
  static StringRow            makeStringRow ( const std::string &, const std::string & );

  // Ask the active document to redraw.
  static void                 requestActiveDocumentRedraw();

protected:

  // Construction.
  BaseObject ( const std::string &name );

  // Use reference counting.
  virtual ~BaseObject();

private:

  // Do not copy.
  BaseObject ( const BaseObject & );
  BaseObject &operator = ( const BaseObject & );

  void                        _destroy();
};


} // namespace Base
} // namespace Modflow


#endif // _MODFLOW_MODEL_BASE_OBJECT_CLASS_H_
