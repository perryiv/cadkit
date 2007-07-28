
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

#include "Parser.h"

#include "Usul/Documents/Document.h"

#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ITimestepAnimation.h"
#include "Usul/Interfaces/ITimeVaryingData.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/ICommandList.h"

#include "osg/BoundingBox"
#include "osg/MatrixTransform"
#include "osg/Image"

#include <string>
#include <vector>

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

protected:

  void                        _read ( XmlTree::Node &node );
  void                        _parseChannel ( XmlTree::Node &node );

  osg::Image *                _volume ( unsigned int timestep, unsigned int channel );
  osg::Node*                  _buildVolume( osg::Image* image );
  void                        _initBoundingBox ();

  void                        _buildScene ();

  /// Usul::Interfaces::ITimestepAnimation
  virtual void                startTimestepAnimation ();
  virtual void                stopTimestepAnimation ();

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  /// Usul::Interfaces::ICommandList
  virtual CommandList      getCommandList ();

  /// Do not copy.
  WRFDocument ( const WRFDocument & );
  WRFDocument &operator = ( const WRFDocument & );

  /// Use reference counting.
  virtual ~WRFDocument();

private:
  struct ChannelInfo
  {
    ChannelInfo () : name ( "" ), index ( 0 ), min ( 0 ), max ( 0 )
    {
    }

    std::string name;
    unsigned int index;
    double min, max;
  };

  typedef std::vector < ChannelInfo >    ChannelInfos;
  typedef osg::ref_ptr < osg::Image >    ImagePtr;
  typedef std::vector < ImagePtr >       ChannelVolumes;
  typedef std::vector < ChannelVolumes > TimestepsData;

  Parser _parser;
  std::string _filename;
  unsigned int _currentTimestep;
  unsigned int _currentChannel;
  unsigned int _timesteps;
  unsigned int _channels;
  unsigned int _x;
  unsigned int _y;
  unsigned int _z;
  unsigned int _numPlanes;
  ChannelInfos _channelInfo;
  osg::ref_ptr < osg::MatrixTransform > _root;
  osg::BoundingBox _bb;
  bool _dirty;
  TimestepsData _data;
};



#endif // _WRF_MODEL_DOCUMENT_H_
