
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Class to contain data for labeling objects.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_LAYERS_POSTGIS_LABEL_DATA_H__
#define __MINERVA_LAYERS_POSTGIS_LABEL_DATA_H__

#include "Minerva/Layers/PostGIS/Export.h"

#include "Serialize/XML/Macros.h"

#include "Usul/Base/Object.h"
#include "Usul/Math/Vector4.h"

namespace Minerva {
namespace Layers {
namespace PostGIS {


class MINERVA_POSTGIS_EXPORT LabelData : public Usul::Base::Object
{
public:

  typedef Usul::Base::Object BaseClass;
  
  USUL_DECLARE_REF_POINTERS ( LabelData );

  LabelData();
  LabelData ( const LabelData& data );

  /// Get/Set show label.
  void                        show ( bool b );
  bool                        show() const;

  /// Get/Set the label color.
  void                        color ( const Usul::Math::Vec4f& color );
  Usul::Math::Vec4f           color() const;

  /// Get/Set the label zOffset.
  void                        zOffset ( double offset );
  double                      zOffset() const;

  /// Get/Set the label size.
  void                        size ( float size );
  float                       size() const;

protected:

  virtual ~LabelData();

private:

  void _registerMembers();

  bool _show;
  Usul::Math::Vec4f _color;
  double _zOffset;
  float _size;

  SERIALIZE_XML_DEFINE_MAP;
  SERIALIZE_XML_DEFINE_MEMBERS ( LabelData );
};


}
}
}
      
#endif // __MINERVA_LAYERS_POSTGIS_LABEL_DATA_H__
