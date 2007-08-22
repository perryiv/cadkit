
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

#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ITimestepAnimation.h"
#include "Usul/Interfaces/ITimeVaryingData.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/ICommandList.h"

#include "Serialize/XML/Macros.h"

#include "osg/BoundingBox"
#include "osg/MatrixTransform"
#include "osg/Image"

#include <string>
#include <vector>
#include <list>

namespace XmlTree { class Node; }

class WRFDocument : public Usul::Documents::Document,
                    public Usul::Interfaces::IBuildScene,
                    public Usul::Interfaces::ITimestepAnimation,
                    public Usul::Interfaces::ITimeVaryingData,
                    public Usul::Interfaces::IUpdateListener,
                    public Usul::Interfaces::ICommandList
{
public:

  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Parser::Data::value_type  DataType;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( WRFDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  WRFDocument();

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
  virtual Filters             filtersOpen()   const;
  virtual Filters             filtersSave()   const;
  virtual Filters             filtersInsert() const;
  virtual Filters             filtersExport() const;

  /// Read the document.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0 );

  /// Write the document to given file name.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0  ) const;

  /// Get/Set the current channel.
  void                        currentChannel ( unsigned int value );
  unsigned int                currentChannel () const;

  /// Get/Set the dirty flag.
  void                        dirty ( bool b );
  bool                        dirty () const;

  /// Usul::Interfaces::ITimeVaryingData
  virtual void                setCurrentTimeStep ( unsigned int current );
  virtual unsigned int        getCurrentTimeStep () const;

  virtual unsigned int        getNumberOfTimeSteps () const;

  /// Get/Set the number of planes.
  void                        numPlanes ( unsigned int numPlanes );
  unsigned int                numPlanes () const;

  /// Add volume
  void                        addVolume ( const Parser::Data& data, osg::Image* image, unsigned int timestep, unsigned int channel );

  /// Load job has finished.
  void                        loadJobFinished ( Usul::Jobs::Job* job );

  /// Are we animating?
  bool                        animating () const;

protected:

  void                        _processReadRequests ( Usul::Threads::Thread *);
  osg::Image *                _volume ( unsigned int timestep, unsigned int channel );
  osg::Node*                  _buildVolume( osg::Image* image );
  void                        _initBoundingBox ();
  osg::Node *                 _buildProxyGeometry ();
  void                        _buildTopography ();
  osg::Node *                 _buildVectorField ( unsigned int timestep, unsigned int channel0, unsigned int channel1 );
  DataType                    _value ( unsigned int timestep, unsigned int channel, unsigned int i, unsigned int j, unsigned int k );

  bool                        _dataCached ( unsigned int timestep, unsigned int channel );
  bool                        _dataRequested ( unsigned int timestep, unsigned int channel );
  void                        _requestData ( unsigned int timestep, unsigned int channel, bool wait );

  void                        _buildScene ();

  /// Usul::Interfaces::ITimestepAnimation
  virtual void                startTimestepAnimation ();
  virtual void                stopTimestepAnimation ();

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  /// Usul::Interfaces::ICommandList
  virtual CommandList         getCommandList ();

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
  typedef std::vector < VectorField >                                  VectorFields;
  typedef std::vector < Channel::RefPtr >                              ChannelInfos;
  typedef osg::ref_ptr < osg::Image >                                  ImagePtr;
  typedef std::vector < std::pair < Parser::Data, ImagePtr >  >        ChannelData;
  typedef std::vector < ChannelData >                                  TimestepsData;
  typedef std::pair < unsigned int, unsigned int >                     Request;
  typedef std::map < Request, Usul::Jobs::Job::RefPtr >                Requests;

  // Internal job to load data from file.
  class LoadDataJob : public Usul::Jobs::Job
  {
  public:
    typedef Usul::Jobs::Job                             BaseClass;
    typedef std::pair < unsigned int, Channel::RefPtr > ReadRequest;
    typedef std::list < ReadRequest >                   ReadRequests;

    USUL_DECLARE_REF_POINTERS ( LoadDataJob );

    LoadDataJob ( const ReadRequests& requests, WRFDocument* document, const Parser& parser );

    void setSize ( unsigned int x, unsigned int y, unsigned int z );
  protected:
    virtual void _started ();
    virtual void _finished ();
    osg::Image*  _createImage ( const ReadRequest& request, Parser::Data& data );

    ReadRequests _requests;
    WRFDocument::RefPtr _document;
    Parser _parser;
    unsigned int _x;
    unsigned int _y;
    unsigned int _z;
  };

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
  osg::ref_ptr < osg::Group > _geometry;
  osg::BoundingBox _bb;
  bool _dirty;
  TimestepsData _data;
  Requests _requests;
  Usul::Jobs::Job::RefPtr _jobForScene;
  unsigned int _lastTimestepLoaded;
  bool _animating;
  osg::Vec3 _offset;
  osg::ref_ptr < osg::Node > _topography;
  std::string _textureFile;
  VectorFields _vectorFields;
  std::vector < osg::ref_ptr < osg::Node > > _vectorCache;
  osg::Vec3 _cellSize;
  osg::Vec3 _cellScale;
  unsigned int _cacheSize;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_CLASS_NAME ( WRFDocument );
  SERIALIZE_XML_SERIALIZE_FUNCTION;
public:
  virtual void deserialize ( const XmlTree::Node &node );

  SERIALIZE_XML_ADD_MEMBER_FUNCTION;
};



#endif // _WRF_MODEL_DOCUMENT_H_
