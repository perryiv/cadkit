
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
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

#ifndef _LIB_MODEL_MANAGER_H_
#define _LIB_MODEL_MANAGER_H_

#include "Export/Export.h"

#include "Usul/Base/Object.h"


namespace osg { class Node; }


class LIB_MODEL_MANAGER_EXPORT LibModelManager : public Usul::Base::Object
{
public:
 
  /// Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  
 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( LibModelManager );

  /// Construction.
  LibModelManager();

  // Reader
  void                    read( const std::string &filename, const std::string &type, Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );

  /// Build the scene.
  virtual osg::Node *     buildScene ( Unknown *caller = 0x0 );

  /// Clear any existing data.
  virtual void            clear ( Unknown *caller = 0x0 );
  
  void                    write() const;
  void                    write( const std::string &filename ) const;

 
protected:

  /// Do not copy.
  LibModelManager ( const LibModelManager & );
  LibModelManager &operator = ( const LibModelManager & );
  
  /// Use reference counting.
  virtual ~LibModelManager();

private:
  
  
};



#endif // _LIB_MODEL_MANAGER_H_
