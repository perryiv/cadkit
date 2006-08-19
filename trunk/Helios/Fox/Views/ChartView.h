
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __CHART_VIEW_H__
#define __CHART_VIEW_H__

#include "Export.h"
#include "Helios/Fox/Views/FoxCanvas.h"

#include <vector>

#include "osg/Vec4"

namespace osg { class Group; }

#include "Usul/Math/Vector2.h"

namespace Helios {
namespace Views {


class OSG_FOX_VIEWS_EXPORT ChartView : public Helios::Views::FoxCanvas, public Usul::Interfaces::IBackground
{
public:

  // Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  // Useful typedefs.
  typedef Helios::Views::FoxCanvas BaseClass;

  typedef Usul::Math::Vec2d             Point;
  typedef std::vector< Point >          Plot;
  typedef std::vector< Plot >           Plots;
  typedef osg::Vec4                     Color;
  typedef std::vector < Color >         ColorArray;
  typedef std::vector < ColorArray >    Colors;
  typedef osg::ref_ptr < osg::Group >   GroupPtr;

  ChartView( Document* document, FX::FXComposite *parent, FX::FXGLVisual *visual );
  virtual ~ChartView();

  // Add a plot with it's color.
  void                    addPlot( const Plot& plot, const Color& color );
  void                    addPlot( const Plot& plot, const ColorArray& colors );

  // Edit the background color.
  virtual void            editBackground();

  // Set the background color to the default
  virtual void            defaultBackground();

  void                    create();

  long                    onFocusIn           ( FX::FXObject *, FX::FXSelector, void * );

  void                    render();
protected:
  virtual void          _resize ( unsigned int width, unsigned int height );
private:

  void   _buildVertices  ( const Plot& plot, osg::Vec3Array& vertices, float xOffset, float yOffset, float widthNorm, float heightNorm );
  float  _maxY  ( const Plot& );
  float  _maxX  ( const Plot& );

  void   _rebuildScene ( osg::Group& group, unsigned int width, unsigned int height );

  ChartView() { }

  Plots  _plots;
  Colors _colors;

  float _paddingPercent;
  float _graphPercent;

  FXDECLARE( ChartView )
};

}
}


#endif // __CHART_VIEW_H__

