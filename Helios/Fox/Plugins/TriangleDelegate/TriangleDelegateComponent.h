
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L Miller IV and Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _TRIANGLE_DELEGATE_COMPONENT_CLASS_H_
#define _TRIANGLE_DELEGATE_COMPONENT_CLASS_H_

#include "CompileGuard.h"

#include "FoxTools/Headers/Object.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"
#include "Usul/Interfaces/GUI/IUpdateGUI.h"
#include "Usul/Interfaces/IHandleActivatingDocument.h"
#include "Usul/Interfaces/IGetLoops.h"
#include "Usul/Types/Types.h"
#include "Usul/Math/Vector3.h"

#include "osg/Vec3"
#include "osg/Vec4"


namespace OsgTools { namespace Triangles { class Loop; class SharedVertex;} }

namespace FX { class FXVerticalFrame; class FXList; }

namespace osg { class Geode; class Drawable; }

namespace Usul { namespace Interfaces { struct IDocument; } }

#include <string>
#include <map>


class TriangleDelegateComponent : public Usul::Base::Referenced,
                                  public Usul::Interfaces::IPlugin,
                                  public Usul::Interfaces::IGUIDelegate,
                                  public Usul::Interfaces::IHandleActivatingDocument,
                                  public Usul::Interfaces::IUpdateGUI,
                                  public FX::FXObject
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced BaseClass;
  typedef Usul::Interfaces::IUnknown Unknown;
  typedef OsgTools::Triangles::Loop Loop;
  typedef Usul::Interfaces::IGetLoops::Loops Loops;
/** MIKE **/
  typedef Usul::Math::Vector3<Usul::Types::Int64> Vec3l;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( TriangleDelegateComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  TriangleDelegateComponent();

  enum
  {
    ID_FACET = 1,
    ID_VERTEX,
    ID_BOUNDING_BOX,
    ID_GLASS_BOUNDING_BOX,
    ID_GOTO_LOOP,
    ID_GOTO_SUCCEEDED_LOOP,
    ID_TRIANGULATE,
    ID_TRIANGULATE_ALL,
    ID_LOOP_DONE,
    ID_SHOW_ALL,
    ID_HIDE_ALL,
    ID_CLIP_BOX,
    ID_TOGGLE_GROUP,
    ID_HIDE_GROUPS,
    ID_SHOW_GROUPS,
    ID_HIDE_ALL_GROUPS,
    ID_SHOW_ALL_GROUPS,
    ID_GROUP_TRANSPARENCY,
    ID_LAST
  };

  /// Command message maps.
  long                      onCommandFacet             ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandVertex            ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandBoundingBox       ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandGlassBoundingBox  ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandGotoLoop          ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandGotoSucceededLoop ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandTriangulate       ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandTriangulateAll    ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandLoopDone          ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandShowAll           ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandHideAll           ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandClipBox           ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandShowLoop          ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandHideLoop          ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandToggleGroup       ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandHideGroups        ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandShowGroups        ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandHideAllGroups     ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandShowAllGroups     ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandGroupTransparency ( FX::FXObject *, FX::FXSelector, void * );

  /// Update message maps.
  long                      onUpdateFacet              ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateVertex             ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateBoundingBox        ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateGlassBoundingBox   ( FX::FXObject *, FX::FXSelector, void * );  
  long                      onUpdateClipBox            ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateToggleGroup        ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateGroupTransparency  ( FX::FXObject *, FX::FXSelector, void * );

protected: 
   /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "Triangle Delegate"; }

  /// Usul::Interfaces::IGUIDelegate
  virtual bool                doesHandle( const std::string& token ) const;
  virtual void                createDefaultGUI ( Usul::Documents::Document *document, Usul::Interfaces::IUnknown* caller );
  virtual void                refreshView      ( Usul::Documents::Document *document, Usul::Interfaces::IViewer  *viewer );

  /// Usul::Interfaces::IHandleActivatingDocument
  virtual void                noLongerActive ( Usul::Documents::Document* document, const std::string& activeType );
  virtual void                nowActive      ( Usul::Documents::Document* document, const std::string& oldType );

  /// Usul::Interfaces::IUpdateGUI
  virtual void                updateGUI();

  void _findLoops ( Usul::Interfaces::IUnknown* document );

  void _buildUncappedTab ( Usul::Interfaces::IUnknown* caller );
  void _buildSucceededTab ( Usul::Interfaces::IUnknown* caller );
  void _buildUncappedTab ( Usul::Interfaces::IUnknown* caller, Usul::Interfaces::IUnknown *document );
  void _buildSucceededTab ( Usul::Interfaces::IUnknown* caller, Usul::Interfaces::IUnknown *document );

  void _buildGroupTab ( Usul::Interfaces::IUnknown* caller );
  void _buildGroupTab ( Usul::Interfaces::IUnknown* caller, Usul::Interfaces::IUnknown *document );

  void _gotoLoop ( Loop&, Usul::Interfaces::IUnknown*, const int index );
  
  bool _triangulateSingle( unsigned int, bool buildOnFly );

  osg::Geode * _drawLoop ( OsgTools::Triangles::Loop& loop, unsigned int number, Usul::Interfaces::IUnknown *caller );
  
  //Get rid of this when everything works
  osg::Drawable* _makeNumber ( const std::string& name, const osg::Vec3& pos, const osg::Vec4& color, float size );



  // Do not copy.
  TriangleDelegateComponent ( const TriangleDelegateComponent & );
  TriangleDelegateComponent &operator = ( const TriangleDelegateComponent & );

  /// Use reference counting.
  virtual ~TriangleDelegateComponent();

  FX::FXVerticalFrame *_uncappedFrame;
  FX::FXVerticalFrame *_succeededFrame;
  FX::FXList *_uncappedList;
  FX::FXList *_succeededList;

  bool _useClipBox;

  // Uncapped loop number is first, succeeded second.
  typedef std::pair< unsigned int, unsigned int > UIntPair;
  typedef std::map < Usul::Interfaces::IDocument*, UIntPair > ActiveLoops;

  ActiveLoops _activeLoops;

  FX::FXVerticalFrame *_groupFrame;
  FX::FXList *_groupList;

  FXDECLARE( TriangleDelegateComponent )

};


#endif // _TRIANGLE_DELEGATE_COMPONENT_CLASS_H_
