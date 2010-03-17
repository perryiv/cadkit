
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for mpd files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _LIB_OPENAL_H_
#define _LIB_OPENAL_H_

#include "Export/Export.h"

#include "Usul/Documents/Document.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Policies/Update.h"


#include "osg/Camera"

#include "XmlTree/Document.h"

#include <string>
#include <memory>

namespace osg { class Node; }


class LIB_OPENAL_EXPORT LibOpenAL : public Usul::Base::Object
{
public:
 
  /// Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  
 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( LibOpenAL );

  /// Construction.
  LibOpenAL();
 
protected:

  /// Do not copy.
  LibOpenAL ( const LibOpenAL & );
  LibOpenAL &operator = ( const LibOpenAL & );

  
  /// Use reference counting.
  virtual ~LibOpenAL();

private:
  

  
};



#endif // _LIB_OPENAL_H_
