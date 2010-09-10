
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

#ifndef _FIELD_VIEW_READER_DOCUMENT_H_
#define _FIELD_VIEW_READER_DOCUMENT_H_

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IAnimatePath.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IMpdNavigator.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Documents/Manager.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Policies/Update.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "XmlTree/Document.h"

#include "osg/Image"
#include "osg/LineSegment"

#include <string>
#include <memory>

namespace osg { class Node; }


class FlowReaderDocument : public Usul::Documents::Document,
                                  public Usul::Interfaces::IBuildScene,
                                  public Usul::Interfaces::IUpdateListener
                                  
{
public:


  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Usul::Documents::Document Document;
  typedef Usul::Documents::Document::RefPtr DocumentPtr;
  typedef Usul::Documents::Manager DocManager;
  typedef DocManager::DocumentInfo Info;
  typedef Usul::Policies::NumberBased UpdatePolicy;
  typedef std::auto_ptr< UpdatePolicy > UpdatePolicyPtr;
  typedef osg::ref_ptr< osg::Group > GroupPtr;
  typedef osg::ref_ptr< osg::Image > Image;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Vec3f Vec3f;
  typedef Usul::Math::Vec4d Color4;
	typedef std::vector< double > GridAxis;
  typedef std::vector< std::string > StringVec;
 
  typedef std::pair < unsigned int, unsigned int > Progress;

	// structs

	// Flow time step
	struct Flow
	{
		Vec3d pos, v;
	};
	typedef std::vector< Flow > FlowTS;
	typedef std::vector< FlowTS > FlowTSC;

	// particles
	struct Particle
	{
		int id, group;
		Vec3d pos, v;
	};
	typedef std::vector< Particle > ParticleTS;
	typedef std::vector< ParticleTS > ParticleTSC;
 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( FlowReaderDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  FlowReaderDocument();

  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &file ) const;
  virtual bool                canInsert ( const std::string &file ) const;
  virtual bool                canOpen   ( const std::string &file ) const;
  virtual bool                canSave  ( const std::string &file ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  /// Get the filters that correspond to what this document can read and write.
  virtual Filters  filtersOpen()   const;
  virtual Filters  filtersSave()   const;
  virtual Filters  filtersInsert() const;
  virtual Filters  filtersExport() const;
  
  /// Read the document.
  virtual void     read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void     write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;
  
 

protected:

  /// Do not copy.
  FlowReaderDocument ( const FlowReaderDocument & );
  FlowReaderDocument &operator = ( const FlowReaderDocument & );

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );


	// read the grid file
	void												_readGridFile( const std::string& filename );
	void												_readFlowFiles( const std::string& path, StringVec files );
	void												_readParticleFiles( const std::string& path, StringVec files );

	FlowTS											_readFlowFile( const std::string& filename );
	ParticleTS									_readParticleFile( const std::string& filename );

  /// Use reference counting.
  virtual ~FlowReaderDocument();

private:
  GroupPtr                    _root;
  
	GridAxis										_xAxis;
	GridAxis										_yAxis;
	GridAxis										_zAxis;

	FlowTSC											_flows;
	ParticleTSC									_particles;


  
};



#endif // _FIELD_VIEW_READER_DOCUMENT_H_
