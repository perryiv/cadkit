
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __WNV_LAYERS_POLYGON_LAYER_H__
#define __WNV_LAYERS_POLYGON_LAYER_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Layers/Layer.h"

namespace Minerva {
namespace Core {
namespace Layers {

class MINERVA_EXPORT PolygonLayer : public Minerva::Core::Layers::Layer
{
public:
  typedef Minerva::Core::Layers::Layer BaseClass;

  PolygonLayer();

  /// Clone the this layer.
  virtual Layer*              clone() const;

  /// Build the data objects.
  virtual void            buildDataObjects( Usul::Interfaces::IUnknown *caller = 0x0 );

  virtual void            modify( Usul::Interfaces::IUnknown *caller = 0x0 );

  /// Get/Set the format string
  void                    format( const std::string& format );
  const std::string&      format() const;

  /// Set data members from given layer.
  virtual void            setDataMembers ( Layer * );

  /// Get/Set the show interior flag.
  void                    showInterior( bool b );
  bool                    showInterior() const;

  /// Get/Set the show border flag.
  void                    border( bool b );
  bool                    border() const;

  /// Get/Set the border color.
  void                    borderColor( const osg::Vec4& color );
  const osg::Vec4&        borderColor() const;

  /// Get/Set the border width.
  void                    borderWidth ( float width );
  float                   borderWidth () const;

protected:
  virtual ~PolygonLayer();

  PolygonLayer( const PolygonLayer& layer );

  void _registerMembers();

private:

  std::string _format;
  bool        _showInterior;
  bool        _showBorder;
  osg::Vec4   _borderColor;
  float       _borderWidth;

  SERIALIZE_XML_DEFINE_MEMBERS ( PolygonLayer );
};


}
}
}

#endif // __WNV_LAYERS_POLYGON_LAYER_H__
