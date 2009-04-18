
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for wrf files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _WRF_MODEL_DOCUMENT_H_
#define _WRF_MODEL_DOCUMENT_H_

#include "Experimental/WRF/WrfModel/Parser.h"
#include "Experimental/WRF/WrfModel/Channel.h"

#include "Usul/Documents/Document.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Jobs/Manager.h"

#include "Usul/Interfaces/IBooleanState.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IBusyState.h"
#include "Usul/Interfaces/ILayer.h"
#include "Usul/Interfaces/ITimestepAnimation.h"
#include "Usul/Interfaces/ITimeVaryingData.h"
#include "Usul/Interfaces/ITreeNode.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/ISerialize.h"

#include "Serialize/XML/Macros.h"

#include "OsgTools/Volume/GPURayCasting.h"
#include "OsgTools/Volume/Texture3DVolume.h"
#include "OsgTools/Volume/TransferFunction.h"

#include "osg/BoundingBox"
#include "osg/MatrixTransform"
#include "osg/Image"

#include <string>
#include <vector>
#include <list>

// Uncomment this line to use GPU ray casting.  There are still some issues to work out.
#define USE_RAY_CASTING 0

class WRFDocument : public Usul::Documents::Document,
                    public Usul::Interfaces::IBuildScene,
                    public Usul::Interfaces::ITimestepAnimation,
                    public Usul::Interfaces::ITimeVaryingData,
                    public Usul::Interfaces::IUpdateListener,
                    public Usul::Interfaces::IMenuAdd,
                    public Usul::Interfaces::ILayer,
                    public Usul::Interfaces::ITreeNode,
                    public Usul::Interfaces::ISerialize,
                    public Usul::Interfaces::IBusyState,
                    public Usul::Interfaces::IBooleanState
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document      BaseClass;
  typedef Parser::Data::value_type       DataType;
  typedef std::vector < DataType >       FloatData;
  typedef std::vector < unsigned char >  ImageData;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( WRFDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  WRFDocument();

  /// Add the view to the document.
  virtual void                addView ( Usul::Interfaces::IView *view );
  
  /// Get this as an IUnknown.
	virtual Usul::Interfaces::IUnknown*   asUnknown();

  /// Usul::Interfaces::IRenderListener inherited from base class.
  virtual void                postRenderNotify ( Unknown *caller );
  virtual void                preRenderNotify ( Unknown *caller );

  /// Build the scene.
  virtual osg::Node *         buildScene ( const BaseClass::Options &options, Unknown *caller = 0x0 );
  
  /// Get the busy state (IBusyState).
  virtual bool                busyStateGet() const;

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &file ) const;
  virtual bool                canInsert ( const std::string &file ) const;
  virtual bool                canOpen   ( const std::string &file ) const;
  virtual bool                canSave  ( const std::string &file ) const;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 );
  
  virtual void                deserialize ( const XmlTree::Node &node );

  /// Get the filters that correspond to what this document can read and write.
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersExport() const;
  
  /// See if the given level falls within this layer's range of levels.
  bool                        isInLevelRange ( unsigned int level ) const;
  
  /// Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const;

  /// Get/Set the current channel.
  void                        currentChannel ( unsigned int value );
  unsigned int                getCurrentChannel () const;
  bool                        isCurrentChannel ( unsigned int ) const;

  /// Get/Set the dirty flag.
  void                        dirty ( bool b );
  bool                        dirty () const;

  /// Usul::Interfaces::ITimeVaryingData
  virtual void                setCurrentTimeStep ( unsigned int current );
  virtual unsigned int        getCurrentTimeStep () const;

  virtual unsigned int        getNumberOfTimeSteps () const;

  /// Go to the next or previous timestep.
  void                        nextTimeStep();
  void                        previousTimeStep();

  /// Get/Set the number of planes.
  void                        numPlanesMultiply ( double factor );
  void                        numPlanes ( unsigned int numPlanes );
  unsigned int                numPlanes () const;

  /// Add volume
  void                        addData( unsigned int timestep, unsigned int channel, const FloatData& data );

  /// Load job has finished.
  void                        loadJobFinished ( Usul::Jobs::Job* job );

  /// Get/Set the animating state.
  bool                        isAnimating () const;
  void                        animating ( bool b );

  /// Get the number of items in the cache.
  unsigned int                cacheSize () const;

  /// Get the maximium number of items in the cache.
  unsigned int                maxCacheSize () const;

  /// Set the transfer function.
  void                        transferFunction ( unsigned int i );
  bool                        isTransferFunction ( unsigned int i ) const;

protected:

  void                        _initBoundingBox ();
  osg::Node *                 _buildProxyGeometry ();
  osg::Node *                 _buildVectorField ( unsigned int timestep, unsigned int channel0, unsigned int channel1 );
  void                        _buildDefaultTransferFunctions ();

  bool                        _dataCached ( unsigned int timestep, unsigned int channel );
  bool                        _dataRequested ( unsigned int timestep, unsigned int channel );
  void                        _requestData ( unsigned int timestep, unsigned int channel, bool wait );

  void                        _purgeCache();
  void                        _updateCache();
  void                        _launchNextCacheRequest();
  bool                        _cacheFull() const;

  void                        _buildScene ();

  /// Usul::Interfaces::ITimestepAnimation
  virtual void                startTimestepAnimation ();
  virtual void                stopTimestepAnimation ();

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  /// Add to the menu.
  virtual void                menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown * caller = 0x0 );
  
  /// Get the guid.
  virtual std::string         guid() const;
  
  /// Get/Set the name.
  virtual std::string         name() const;
  virtual void                name( const std::string& );
  
  /// Get/Set show layer
  virtual void                showLayer ( bool b );
  virtual bool                showLayer() const;
  
  // Get the number of children (ITreeNode).
  virtual unsigned int        getNumChildNodes() const;
  
  // Get the child node (ITreeNode).
  virtual ITreeNode::RefPtr   getChildNode ( unsigned int which );
  
  // Set/get the name (ITreeNode).
  virtual void                setTreeNodeName ( const std::string & );
  virtual std::string         getTreeNodeName() const;

  // Set/get the state (IBooleanState).
  virtual void                setBooleanState ( bool );
  virtual bool                getBooleanState() const;

  /// Do not copy.
  WRFDocument ( const WRFDocument & );
  WRFDocument &operator = ( const WRFDocument & );

  /// Use reference counting.
  virtual ~WRFDocument();

private:

  struct VectorField
  {
    VectorField ( ) : name ( "" ), u ( 0 ), v ( 0 )
    {
    }

    std::string name;
    unsigned int u, v;
  };

  /// Typedefs.
  typedef std::vector < VectorField >                           VectorFields;
  typedef std::vector < Channel::RefPtr >                       ChannelInfos;
  typedef osg::ref_ptr < osg::Image >                           ImagePtr;
  typedef std::pair < unsigned int, unsigned int >              Request;
  typedef std::map < Request, Usul::Jobs::Job::RefPtr >         Requests;
  typedef std::map < Request, ImageData >                       VolumeCache;
  typedef std::map < Request, FloatData >                       DataCache;
  typedef OsgTools::Volume::TransferFunction                    TransferFunction;
  typedef TransferFunction::RefPtr                              TransferFunctionPtr;
  typedef std::vector < TransferFunctionPtr >                   TransferFunctions;
#if USE_RAY_CASTING
  typedef OsgTools::Volume::GPURayCasting Volume;
#else
  typedef OsgTools::Volume::Texture3DVolume Volume;
#endif

  Parser _parser;
  std::string _filename;
  unsigned int _currentTimestep;
  unsigned int _currentChannel;
  unsigned int _timesteps;
  unsigned int _channels;
  unsigned int _x;
  unsigned int _y;
  unsigned int _z;
  unsigned int _num2DFields;
  unsigned int _numPlanes;
  ChannelInfos _channelInfo;
  osg::ref_ptr < osg::MatrixTransform > _root;
  osg::ref_ptr < osg::MatrixTransform > _volumeTransform;
  osg::ref_ptr < Volume > _volumeNode;
  osg::BoundingBox _bb;
  bool _dirty;
  Requests _requests;
  Usul::Jobs::Job::RefPtr _jobForScene;
  bool _animating;
  VectorFields _vectorFields;
  osg::Vec3 _cellSize;
  osg::Vec3 _cellScale;
  unsigned int _maxCacheSize;
  bool _cacheRawData;
  VolumeCache _volumeCache;
  DataCache _dataCache;
  bool _headers;
  Usul::Math::Vec2d _lowerLeft;
  Usul::Math::Vec2d _upperRight;
  unsigned int _currentTransferFunction;
  TransferFunctions _transferFunctions;
  
  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_CLASS_NAME ( WRFDocument );
  SERIALIZE_XML_SERIALIZE_FUNCTION;
  SERIALIZE_XML_ADD_MEMBER_FUNCTION;
};


#endif // _WRF_MODEL_DOCUMENT_H_
