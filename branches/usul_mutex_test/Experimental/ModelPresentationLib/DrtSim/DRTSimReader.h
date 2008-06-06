
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Wei Chen, Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document for mpd files
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _DRT_SIM_DOCUMENT_H_
#define _DRT_SIM_DOCUMENT_H_

// added by Wei CHEN
#include "Area.h"
#include "Agent.h"
#include "Stock.h"
#include "Transhipment.h"
#include "Spore.h"

#include "Usul/Documents/Document.h"
#include "Usul/Math/Vector2.h"
#include "Usul/Math/Vector3.h"
#include "Usul/Math/Vector4.h"

#include "OsgTools/Triangles/TriangleSet.h"

#include "osg/Image"
#include "osg/LineSegment"

#include <string>
#include <memory>


// end here (added by W.C )

namespace osg { class Node; }


class DrtSimReader : public Usul::Base::Object
{
public:
  // Structs
  struct DynamicInformation
  {

  };
  struct TimelineInformation
  {

  };
  struct ModelInformation
  {

  };
  struct SetInformation
  {

  };
  /// Useful typedefs.
  typedef Usul::Documents::Document BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef osg::ref_ptr< osg::Group > GroupPtr;
  typedef osg::ref_ptr< osg::Image > Image;
  typedef Usul::Math::Vec2d Vec2d;
  typedef Usul::Math::Vec3d Vec3d;
  typedef Usul::Math::Vec3f Vec3f;
  typedef Usul::Math::Vec4d Color4;
  typedef std::vector< Color4 > ColorTable;
  typedef std::vector< Color4 > Color4Vec;
  typedef std::vector< unsigned char > ColorVec;
  typedef std::vector< unsigned int > Vertices;
  typedef osg::ref_ptr< osg::LineSegment > LineSegmentPtr;
  typedef osg::BoundingBox BoundingBox;
  typedef osg::ref_ptr< osg::MatrixTransform > MatrixTransformPtr;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DrtSimReader );

  /// Usul::Interfaces::IUnknown members.
  //USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  DrtSimReader();

  /// Read the document.
  virtual void     read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Write the document to given file name.
  virtual void     write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 ) const;
  
protected:

  /// Do not copy.
  DrtSimReader ( const DrtSimReader & );
  DrtSimReader &operator = ( const DrtSimReader & );

  void                        _openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller );
 
  //Reads
  void						            _read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  void                        _init ( Unknown *progress );

  /// Use reference counting.
  virtual ~DrtSimReader();

 
  void                      _openDocument ( const std::string &file, 
                                            Usul::Documents::Document *document, 
                                            Usul::Interfaces::IUnknown *caller, 
                                            Unknown *progress );


private:
  GroupPtr                    _root;

  // added by Wei CHEN
  bool			_readAreaDetailsFile   ( );
  bool			_readAgentDetailsFile  ( );
  bool			_readStockDetailsFile  ( );
  bool			_readTranshipmentsFile ( );
  bool			_readSporeDetailsFile  ( );

  osg::Group*		_createHUDText( );

  unsigned int		_winHeight;
  unsigned int		_winWidth;
  unsigned int		_winMargin;

  std::string		_areaFileName;
  std::string		_agentFileName;
  std::string		_stockFileName;
  std::string		_transFileName;
  std::string		_sporeFileName;

  Area			_area;
  Agent			_agent;
  Stock			_stock;
  Transhipment	        _trans;
  Spore			_spore;

  // Menu variables
  bool				_isAnimating;
  bool				_isStartOver;
  bool				_isAgentOn;
  bool				_isSporeOn;
  bool				_isStockOn;
  bool				_isTransOn;

  std::string			_legendMachine;
  std::string     _writerMachine;

  unsigned int        _counter;


  // Model Presentation library pointer to create the mpd file
  ModelPresentationLib::RefPtr        _mpdWriter;
  std::string                         _workingDir;
  std::string                         _drtFileName;
  

  // end here ( added by W.C )
  
  
};



#endif // _DRT_SIM_DOCUMENT_H_
