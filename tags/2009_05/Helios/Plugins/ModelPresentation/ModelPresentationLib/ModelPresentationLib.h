
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

#ifndef _MODEL_PRESENTATION_LIB_H_
#define _MODEL_PRESENTATION_LIB_H_

#include "MpdWriter.h"
#include "Export/Export.h"

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IAnimatePath.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IMpdNavigator.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/ITextMatrix.h"
#include "Usul/Interfaces/IViewMatrix.h"
#include "Usul/Interfaces/IViewport.h"
#include "Usul/Interfaces/IMpdWriter.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Policies/Update.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "osg/Camera"

#include "XmlTree/Document.h"

#include <string>
#include <memory>

namespace osg { class Node; }


class MODEL_PRESENTATION_LIB_EXPORT ModelPresentationLib : public Usul::Base::Object
{
public:
 
  /// Useful typedefs.
  typedef Usul::Base::Object BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  
 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ModelPresentationLib );

  /// Construction.
  ModelPresentationLib();

  // Reader
  void                    read( const std::string &filename, const std::string &type, Usul::Interfaces::IUnknown *caller = 0x0, Usul::Interfaces::IUnknown *progress = 0x0 );

  /// Build the scene.
  virtual osg::Node *     buildScene ( Unknown *caller = 0x0 );

  /// Clear any existing data.
  virtual void            clear ( Unknown *caller = 0x0 );
  
  /// Usul::Interfaces::IMpdWriter functions
  // Model Functions
  void                    addModel( const std::string &name, const std::string &path );
  // Set functions
  void                    addSet( const std::string &name, const std::string &menuName );
  void                    addModelToSet( const std::string &modelName, const std::string &setName, const std::string &groupName );
  void                    addGroupToSet( const std::string &setName, const std::string &groupName );
  // TimeSet functions
  void                    addTimeSet( const std::string &name, const std::string &menuName, unsigned int endTime );
  void                    addModelsToTimeSet( std::vector< std::string > modelList, const std::string &timeSetName, unsigned int startTime, unsigned int endTime );
  // Sequence Functions
  void                    addSequence( const std::string &name, const std::string &menuName );
  void                    addStepToSequence( const std::string &locationName, std::vector< std::string > modelList );
  // Location Functions
  void                    addLocation( const std::string &name, const std::string &location );
  void                    addLocation( const std::string &name, osg::Matrix location );
  // DynamicSet Functions
  void                    addDynamicSet( const std::string &name, const std::string &menuName, const std::string &prefix,
                                                 const std::string &extension, const std::string &directory, unsigned int maxFiles );
  void                    buildXMLString();
  void                    write() const;
  void                    write( const std::string &filename ) const;

 
protected:

  /// Do not copy.
  ModelPresentationLib ( const ModelPresentationLib & );
  ModelPresentationLib &operator = ( const ModelPresentationLib & );

  std::string            _getWorkingDir();

  
  /// Use reference counting.
  virtual ~ModelPresentationLib();

private:
  
  MpdWriter::RefPtr             _writer;
  std::string                   _workingDir;

  
};



#endif // _MODEL_PRESENTATION_LIB_H_
