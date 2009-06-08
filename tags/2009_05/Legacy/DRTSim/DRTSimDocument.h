
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

#ifndef _DRT_SIM_DOCUMENT_H_
#define _DRT_SIM_DOCUMENT_H_

#include "Usul/Documents/Document.h"
#include "Usul/Interfaces/IAnimatePath.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IUpdateListener.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/IDrtCommands.h"
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

// added by Wei CHEN
#include "Area.h"
#include "Agent.h"
#include "Stock.h"
#include "Transhipment.h"
#include "Spore.h"
// end here (added by W.C )

namespace osg { class Node; }


class DRTSimDocument : public Usul::Documents::Document,
                                  public Usul::Interfaces::IBuildScene,
                                  public Usul::Interfaces::IUpdateListener,
								  public Usul::Interfaces::IMenuAdd,
								  public Usul::Interfaces::IDrtCommands 
                                  
{
public:
  // Structs
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
  typedef std::vector< Color4 > ColorTable;
  typedef std::vector< Color4 > Color4Vec;
  typedef std::vector< unsigned char > ColorVec;
  typedef std::vector< unsigned int > Vertices;
  typedef osg::ref_ptr< osg::LineSegment > LineSegmentPtr;
  typedef osg::BoundingBox BoundingBox;
  typedef osg::ref_ptr< osg::MatrixTransform > MatrixTransformPtr;

 
  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( DRTSimDocument );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  DRTSimDocument();

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
  virtual void     write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 ) const;
  
 

protected:

  /// Do not copy.
  DRTSimDocument ( const DRTSimDocument & );
  DRTSimDocument &operator = ( const DRTSimDocument & );

  /// Usul::Interfaces::IUpdateListener
  virtual void                updateNotify ( Usul::Interfaces::IUnknown *caller );

  void                        _openDocument ( const std::string &file, Usul::Documents::Document *document, Usul::Interfaces::IUnknown *caller );
  void                        _setStatusBar ( const std::string &text, Usul::Interfaces::IUnknown *caller );

  // Scene Builders
  void                        _buildScene( Unknown *caller );
  osg::Node*                  _buildWindow( Unknown *caller );
    
  //Reads
  void						  _read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );
  /// Use reference counting.
  virtual ~DRTSimDocument();

  // Usul::Interfaces::IMenuAdd
  virtual void               menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown* caller = 0x0 );

  // Usul::Interfaces::IDrtCommands
  // set the animation state
  virtual void              animating ( bool state );

  // get the animation state
  virtual bool              animating ();

  // same for other animation
  virtual void				startOver ();

  virtual void              agentShow ( bool state );
  virtual bool              agentShow ();

  virtual void              sporeShow ( bool state );
  virtual bool              sporeShow ();

  virtual void              stockShow ( bool state );
  virtual bool              stockShow ();

  virtual void              transShow ( bool state );
  virtual bool              transShow ();

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

  unsigned int _counter;

  // end here ( added by W.C )
  
  
};



#endif // _DRT_SIM_DOCUMENT_H_
