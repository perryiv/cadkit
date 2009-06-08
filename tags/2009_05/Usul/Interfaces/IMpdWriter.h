
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Jeff Conner
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __USUL_INTERFACES_IMPDWRITER_H__
#define __USUL_INTERFACES_IMPDWRITER_H__

#include "Usul/Interfaces/IUnknown.h"

namespace Usul {
namespace Interfaces {

struct IMpdWriter : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IMpdWriter );

  /// Id for this interface.
  enum { IID = 2994915941u };

 // Model Functions
  virtual void                    addModel( const std::string &name, const std::string &path ) = 0;

  // Set functions
  virtual void                    addSet( const std::string &name, const std::string &menuName ) = 0;
  virtual void                    addModelToSet( const std::string &modelName, const std::string &setName, const std::string &groupName ) = 0;

  virtual void                    addGroupToSet( const std::string &setName, const std::string &groupName ) = 0;

  // TimeSet functions
  virtual void                    addTimeSet( const std::string &name, const std::string &menuName, unsigned int endTime ) = 0;
  virtual void                    addModelsToTimeSet( std::vector< std::string > modelList, const std::string &timeSetName,
                                                      unsigned int startTime, unsigned int endTime ) = 0;

  // Sequence Functions
  virtual void                    addSequence( const std::string &name, const std::string &menuName ) = 0;
  virtual void                    addStepToSequence( const std::string &locationName, std::vector< std::string > modelList ) = 0;

  // Location Functions
  virtual void                    addLocation( const std::string &name, const std::string &location ) = 0;
  virtual void                    addLocation( const std::string &name, osg::Matrix location ) = 0;

  // DynamicSet Functions
  virtual void                    addDynamicSet( const std::string &name, const std::string &menuName, const std::string &prefix,
                                                 const std::string &extension, const std::string &directory, unsigned int maxFiles ) = 0;

  virtual void                    buildXMLString() = 0;
  
  virtual void                    write() const = 0;
  virtual void                    write( const std::string &filename ) const = 0;


  
}; //struct IMpdWriter



}
}

#endif // __USUL_INTERFACES_IMPDWRITER_H__
