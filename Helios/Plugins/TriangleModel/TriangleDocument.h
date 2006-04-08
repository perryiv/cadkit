
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for triangle models.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TRIANGLE_MODEL_DOCUMENT_H_
#define _TRIANGLE_MODEL_DOCUMENT_H_

#include "Usul/Documents/Document.h"

#include "OsgTools/Triangles/TriangleSet.h"
#include "OsgTools/Triangles/Loop.h"

#include "Usul/Interfaces/IFlipNormals.h"
#include "Usul/Interfaces/IFlipNormal.h"
#include "Usul/Interfaces/IAddTriangle.h"
#include "Usul/Interfaces/IFindLoops.h"
#include "Usul/Interfaces/IGetLoops.h"
#include "Usul/Interfaces/IGetVertex.h"
#include "Usul/Interfaces/IDeletePrimitive.h"
#include "Usul/Interfaces/IKeepAllConnected.h"
#include "Usul/Interfaces/IDeleteAllConnected.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ITriangle.h"
#include "Usul/Interfaces/ITriangleSV.h"
#include "Usul/Interfaces/IGetVertexNormal.h"
#include "Usul/Interfaces/IGetTriangleNormal.h"
#include "Usul/Interfaces/IGetBoundingBox.h"
#include "Usul/Interfaces/IAddSharedVertex.h"
#include "Usul/Interfaces/IGroupPrimitives.h"
#include "Usul/Interfaces/ISmoothModel.h"
#include "Usul/Interfaces/IDecimateModel.h"
#include "Usul/Interfaces/ISubdivideModel.h"
#include "Usul/Interfaces/IShowNewPrimitives.h"

#include "Usul/Types/Types.h"

#include <string>

using namespace Usul::Types;


class TriangleDocument : public Usul::Documents::Document,
                         public Usul::Interfaces::IFlipNormals,
                         public Usul::Interfaces::IFlipNormal,
                         public Usul::Interfaces::IAddTriangleWithSharedVertex,
                         public Usul::Interfaces::IAddTrangleWithOsgVec3f,
                         public Usul::Interfaces::IFindLoops,
                         public Usul::Interfaces::IGetLoops,
                         public Usul::Interfaces::IGetVertex,
                         public Usul::Interfaces::IDeletePrimitive,
                         public Usul::Interfaces::IKeepAllConnected,
                         public Usul::Interfaces::IDeleteAllConnected,
                         public Usul::Interfaces::IBuildScene,
                         public Usul::Interfaces::ITriangle,
                         public Usul::Interfaces::ITriangleSV,
                         public Usul::Interfaces::IGetVertexNormal,
                         public Usul::Interfaces::IGetTriangleNormal,
                         public Usul::Interfaces::IGetBoundingBox,
                         public Usul::Interfaces::IAddSharedVertex,
                         public Usul::Interfaces::IGroupPrimitives,
                         public Usul::Interfaces::ISmoothModel,
                         public Usul::Interfaces::IDecimateModel,
                         public Usul::Interfaces::ISubdivideModel,
                         public Usul::Interfaces::IShowNewPrimitives
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef OsgTools::Triangles::TriangleSet TriangleSet;
  typedef TriangleSet::TriangleVector TriangleVector;
  typedef OsgTools::Triangles::SharedVertex SharedVertex;
  typedef Usul::Interfaces::IGetLoops::Loops Loops;
  typedef OsgTools::Triangles::Triangle Triangle;
  typedef Usul::Interfaces::IUnknown IUnknown;

  /// Type information.
  USUL_DECLARE_TYPE_ID ( TriangleDocument );

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TriangleDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  TriangleDocument();
  
  /// Usul::Interafaces::IAddSharedVertex
  virtual SharedVertex *      addSharedVertex ( const osg::Vec3 &v, bool look = true );

  /// Add the triangle.
  virtual Triangle *          addTriangle ( const osg::Vec3f &v0, const osg::Vec3f &v1, const osg::Vec3f &v2, const osg::Vec3f &n, bool update );
  virtual Triangle *          addTriangle ( SharedVertex *sv0, SharedVertex *sv1, SharedVertex *sv2, const osg::Vec3f &n, bool update );

  /// Add an entire triangle set. Assumes the triangle set has been constructed properly.
  void                        addTriangleSet ( TriangleSet * );

  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &ext ) const;
  virtual bool                canInsert ( const std::string &ext ) const;
  virtual bool                canOpen   ( const std::string &ext ) const;
  virtual bool                canSave   ( const std::string &ext ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );

  /// Create default GUI
  virtual void                createDefaultGUI ( Unknown *caller = 0x0 );

  // Usul::Interfaces::IDeleteAllConnected
  virtual void                deleteAllConnected ( Usul::Interfaces::IUnknown *caller, const osgUtil::Hit &hit );

  // Usul::Interfaces::IDeletePrimitive
  virtual void                deletePrimitive ( const osgUtil::Hit& hit );

  /// Get the filters that correspond to what this document can read and write.
  virtual Filters             filtersExport() const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;

  // Usul::Interfaces::IFindLoops
  virtual void                findLoops ( Usul::Interfaces::IUnknown* caller );

  /// Flip the normals.
  virtual void                flipNormalVectors();

  /// Flip the normal of the i'th triangle.
  virtual void                flipNormalVector ( unsigned int i );

  // Get the bounding-box.
  virtual osg::BoundingBox    getBoundingBox() const;

  /// Get the vertex at the index
  virtual const osg::Vec3f &  getVertex ( unsigned int index ) const;

  // Get normal for the i'th vertex and triangle.
  virtual const osg::Vec3f&   getTriangleNormal ( unsigned int index ) const;
  virtual const osg::Vec3f&   getVertexNormal   ( unsigned int index ) const;
      
  /// Notify this document of the message.
  virtual void                notify ( unsigned short message );

  // Access the containers of normals. Use with caution.
  const osg::Vec3Array *      normalsT() const { return _triangles->normalsT(); }
  osg::Vec3Array *            normalsT()       { return _triangles->normalsT(); }
  const osg::Vec3Array *      normalsV() const { return _triangles->normalsV(); }
  osg::Vec3Array *            normalsV()       { return _triangles->normalsV(); }

  // Purge memory.
  void                        purge();

  /// Get the number of triangles.
  unsigned int                numTriangles() const;

  /// Read the file and add it to document's data.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0 );

  /// Reserve space for the triangles.
  void                        reserveTriangles ( unsigned int );

  // Get the shared vertices of the i'th triangle.
  virtual const SharedVertex* sharedVertex0 ( const osg::Geode *g, const osg::Drawable* d, unsigned int i ) const { return _triangles->sharedVertex0 ( g, d, i ); }
  virtual const SharedVertex* sharedVertex1 ( const osg::Geode *g, const osg::Drawable* d, unsigned int i ) const { return _triangles->sharedVertex1 ( g, d, i ); }
  virtual const SharedVertex* sharedVertex2 ( const osg::Geode *g, const osg::Drawable* d, unsigned int i ) const { return _triangles->sharedVertex2 ( g, d, i ); }
  virtual SharedVertex*       sharedVertex0 ( const osg::Geode *g, const osg::Drawable* d, unsigned int i )       { return _triangles->sharedVertex0 ( g, d, i ); }
  virtual SharedVertex*       sharedVertex1 ( const osg::Geode *g, const osg::Drawable* d, unsigned int i )       { return _triangles->sharedVertex1 ( g, d, i ); }
  virtual SharedVertex*       sharedVertex2 ( const osg::Geode *g, const osg::Drawable* d, unsigned int i )       { return _triangles->sharedVertex2 ( g, d, i ); }

  // Get the triangles. Use with caution.
  const TriangleVector &      triangles() const { return _triangles->triangles(); }
  TriangleVector &            triangles()       { return _triangles->triangles(); }

  // Get the triangle set.
  const TriangleSet *         triangleSet() const { return _triangles; }

  // Update the bounding box.
  void                        updateBounds ( const osg::Vec3f &v );

  // Get vertex pool. Use with caution.
  const osg::Vec3Array *      vertices() const { return _triangles->vertices(); }
  osg::Vec3Array *            vertices()       { return _triangles->vertices(); }

  // Get the vertices of the i'th triangle.
  const osg::Vec3f &          vertex0 ( unsigned int i ) const { return _triangles->vertex0 ( i ); }
  const osg::Vec3f &          vertex1 ( unsigned int i ) const { return _triangles->vertex1 ( i ); }
  const osg::Vec3f &          vertex2 ( unsigned int i ) const { return _triangles->vertex2 ( i ); }

  // Get the vertices of the i'th triangle.
  virtual const osg::Vec3f &  vertex0 ( const osg::Geode *g, const osg::Drawable* d, unsigned int i ) const { return _triangles->vertex0 ( g, d, i ); }
  virtual const osg::Vec3f &  vertex1 ( const osg::Geode *g, const osg::Drawable* d, unsigned int i ) const { return _triangles->vertex1 ( g, d, i ); }
  virtual const osg::Vec3f &  vertex2 ( const osg::Geode *g, const osg::Drawable* d, unsigned int i ) const { return _triangles->vertex2 ( g, d, i ); }

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename,  Unknown *caller = 0x0  ) const;
  
protected:

  typedef std::vector< unsigned int > Connected;

  /// Do not copy.
  TriangleDocument ( const TriangleDocument & );
  TriangleDocument &operator = ( const TriangleDocument & );

  /// Use reference counting.
  virtual ~TriangleDocument();

  void                        _findAllConnected ( Usul::Interfaces::IUnknown* caller, Connected& connected, Uint32 seed, bool showProgress, bool clearFlags );

  /// Usul::Interfaces::IKeepAllConnected
  virtual void                keepAllConnected ( Usul::Interfaces::IUnknown *caller, const osgUtil::Hit &hit );

  /// Usul::Interfaces::IGroupPrimitives
  virtual void                groupPrimitives ( Usul::Interfaces::IUnknown *caller);
  virtual unsigned int        groupsNumber () const;
  virtual IUnknown*           getPrimitiveGroup ( unsigned int i );

  /// Usul::Interfaces::IGetLoops
  virtual const Loops&        getUncappedLoops() const;
  virtual Loops&              getUncappedLoops();
  virtual const Loops&        getCappedLoops() const;
  virtual Loops&              getCappedLoops();

  /// Usul::Interfaces::ISmoothModel
  virtual void                smoothModel ( );

  /// Usul::Interfaces::IDecimateModel
  virtual void                decimateModel ( );

  /// Usul::Interfaces::ISubdivideModel
  virtual void                subdivideModel ( unsigned int numSubdivisions);

  /// Usul::Interfaces::IShowNewPrimitives
  virtual osg::Node*          getNewPrimitives ( );

private:

  TriangleSet::ValidRefPtr _triangles;
  Loops _uncapped;
  Loops _capped;
};


#endif // _TRIANGLE_MODEL_DOCUMENT_H_
