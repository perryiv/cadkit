
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for modflow files.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MODFLOW_MODEL_DOCUMENT_H_
#define _MODFLOW_MODEL_DOCUMENT_H_

#include "Helios/Plugins/ModflowModel/CompileGuard.h"
#include "Helios/Plugins/ModflowModel/Readers/BaseReader.h"
#include "Helios/Plugins/ModflowModel/Model/Layer.h"

#include "XmlTree/Node.h"

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IDirtyState.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IIntersectListener.h"
#include "Usul/Interfaces/ITreeNode.h"
#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"

#include <iosfwd>
#include <string>
#include <vector>

namespace osg { class Node; class Group; }
namespace Usul { namespace Factory { class ObjectFactory; } }


namespace Modflow {


class ModflowDocument : public Usul::Documents::Document,
                        public Usul::Interfaces::IBuildScene,
                        public Usul::Interfaces::IDirtyState,
                        public Usul::Interfaces::IUpdateListener,
                        public Usul::Interfaces::IIntersectListener,
                        public Usul::Interfaces::ITreeNode,
                        public Usul::Interfaces::IBooleanState
{
public:

  // Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Usul::Math::Vec2ui Vec2ui;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3f Vec3f;
  typedef std::vector<double> GridData;
  typedef std::pair<std::string,GridData> GridInfo;
  typedef std::vector<GridInfo> GridVector;
  typedef Modflow::Model::Layer Layer;
  typedef std::vector<Layer::RefPtr> Layers;
  typedef Usul::Factory::ObjectFactory Factory;
  typedef Modflow::Readers::BaseReader BaseReader;
  typedef Usul::Interfaces::ITreeNode ITreeNode;
  typedef Usul::Interfaces::IBooleanState IBooleanState;

  // Type information.
  USUL_DECLARE_TYPE_ID ( ModflowDocument );

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( ModflowDocument );

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Construction.
  ModflowDocument();

  // Build the scene.
  virtual osg::Node *                     buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );

  // Return true if this document can do it.
  virtual bool                            canExport ( const std::string &file ) const;
  virtual bool                            canInsert ( const std::string &file ) const;
  virtual bool                            canOpen   ( const std::string &file ) const;
  virtual bool                            canSave   ( const std::string &file ) const;

  // Clear any existing data.
  virtual void                            clear ( Unknown *caller = 0x0 );

  // Set/get the dirty flag.
  virtual void                            dirtyState ( bool );
  virtual bool                            dirtyState() const;

  // Get the filters that correspond to what this document can read and write.
  virtual Filters                         filtersOpen()   const;
  virtual Filters                         filtersSave()   const;
  virtual Filters                         filtersInsert() const;
  virtual Filters                         filtersExport() const;

  // Set/get the flag.
  void                                    flags ( unsigned int );
  unsigned int                            flags() const;

  // Return the grid size.
  Vec2ui                                  gridSize() const;

  // Usul::Interfaces::IIntersectListener.
  virtual void                            intersectNotify ( float x, float y, const osgUtil::Hit &hit, Usul::Interfaces::IUnknown *caller );

  // Set/get the layers.
  void                                    layers ( Layers & );
  const Layers &                          layers() const;
  Layers &                                layers();

  // Return number of layers.
  unsigned int                            numLayers() const;

  // Read the document.
  virtual void                            read ( const std::string &filename, Unknown *caller, Unknown *progress );

  // Write the document to given file name.
  virtual void                            write ( const std::string &filename, Unknown *caller = 0x0  ) const;

  // Usul::Interfaces::IUpdateListener
  virtual void                            updateNotify ( Usul::Interfaces::IUnknown *caller );

  // Usul::Interfaces::ITreeNode.
  virtual unsigned int                    getNumChildNodes() const;
  virtual const ITreeNode *               getChildNode ( unsigned int which ) const;
  virtual ITreeNode *                     getChildNode ( unsigned int which );
  virtual void                            setTreeNodeName ( const std::string & );
  virtual std::string                     getTreeNodeName() const;

  // Usul::Interfaces::IBooleanState.
  virtual void                            setBooleanState ( bool );
  virtual bool                            getBooleanState() const;

protected:

  void                                    _buildScene ( Unknown *caller );

  void                                    _destroy();

  void                                    _incrementProgress ( bool state, Unknown *progress, unsigned int &numerator, unsigned int denominator );

  void                                    _read ( Factory &, XmlTree::Node *file, Unknown *progress );
  void                                    _read ( Factory &, const std::string &type, const std::string &file, Unknown *progress );

  // Do not copy.
  ModflowDocument ( const ModflowDocument & );
  ModflowDocument &operator = ( const ModflowDocument & );

  // Use reference counting.
  virtual ~ModflowDocument();

private:

  Vec2ui _gridSize;
  Vec2d _cellSize;
  GridVector _bounds;
  GridVector _startHeads;
  GridInfo _landElev;
  GridVector _bottomElev;
  Layers _layers;
  osg::ref_ptr<osg::Group> _root;
  osg::ref_ptr<osg::Group> _built;
  unsigned int _flags;
};


} // namespace Modflow


#endif // _POLYGON_MODEL_DOCUMENT_H_
