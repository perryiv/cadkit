
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
//  The component class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MEASURE_TOOL_COMPONENT_CLASS_H_
#define _MEASURE_TOOL_COMPONENT_CLASS_H_

#include "MeasureTool/CompileGuard.h"

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IPlugin.h"
#include "Usul/Interfaces/IIntersectListener.h"
#include "Usul/Interfaces/IMenuAdd.h"
#include "Usul/Interfaces/IActiveViewListener.h"
#include "Usul/Interfaces/IButtonPressListener.h"

#include "osg/Group"
#include "osg/Vec3"

#include <vector>

class MeasureToolComponent : public Usul::Base::Object,
  public Usul::Interfaces::IPlugin,
  public Usul::Interfaces::IIntersectListener,
  public Usul::Interfaces::IMenuAdd,
  public Usul::Interfaces::IActiveViewListener,
  public Usul::Interfaces::IButtonPressListener
{
public:

  /// Typedefs.
  typedef Usul::Base::Object                           BaseClass;
  typedef Usul::Interfaces::IUnknown                   Unknown;
  typedef std::vector <osg::Vec3>                      Positions;
  typedef Usul::Documents::Manager                     DocManager;
  typedef DocManager::DocumentInfo                     Info;
  typedef Usul::Interfaces::IArcGenReaderWriter        IArcGenReaderWriter;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( MeasureToolComponent );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Default construction.
  MeasureToolComponent();

  /// Usul::Interfaces::IPlugin
  virtual std::string         getPluginName() const { return "MeasureTool"; }

  /// Get/Set the measure state.
  void                        measureOn  ( bool b );
  bool                        isMeasureOn() const;
protected: 

  /// The active document has changed.
  virtual void      activeViewChanged ( Usul::Interfaces::IUnknown *oldView, Usul::Interfaces::IUnknown *newView );

  // Notify the observer of the intersection.
  virtual void      intersectNotify ( float x, float y, const osgUtil::Hit &hit, Usul::Interfaces::IUnknown *caller );

  // Add to the menu.
  virtual void      menuAdd ( MenuKit::Menu& menu, Usul::Interfaces::IUnknown* caller = 0x0 );

  // Called when button is released.
  virtual bool      buttonPressNotify ( Usul::Interfaces::IUnknown * );

  // Do not copy.
  MeasureToolComponent ( const MeasureToolComponent & );
  MeasureToolComponent &operator = ( const MeasureToolComponent & );

  /// Use reference counting.
  virtual ~MeasureToolComponent();

private:
  void              _updateMeasurement( Usul::Interfaces::IUnknown *caller ) const;
  void              _clear();
  void              _exportToArcGen( Usul::Interfaces::IUnknown *caller );

  bool _measure;
  bool _appendPosition;
  Positions _positions;
  Usul::Interfaces::IUnknown::QueryPtr _view;
  osg::ref_ptr<osg::Group> _root;

  double _measurement;
};


#endif // _MEASURE_TOOL_COMPONENT_CLASS_H_
