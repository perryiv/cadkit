
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_TOOLS_VOLUME_TRANSFER_FUNCTION_1D_H__
#define __OSG_TOOLS_VOLUME_TRANSFER_FUNCTION_1D_H__

#include "OsgTools/Export.h"
#include "OsgTools/Volume/TransferFunction.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Math/Vector4.h"

#include "osg/Image"
#include "osg/Texture"

#include <vector>

namespace OsgTools {
namespace Volume {


class OSG_TOOLS_EXPORT TransferFunction1D : public OsgTools::Volume::TransferFunction
{
public:
  typedef OsgTools::Volume::TransferFunction BaseClass;
  typedef Usul::Math::Vec4uc     Color;
  typedef std::vector < Color >  Colors;
  typedef Colors::iterator       Iterator;
  typedef Colors::const_iterator ConstIterator;

  USUL_DECLARE_REF_POINTERS ( TransferFunction1D );

  /// Construction.
  TransferFunction1D ();
  TransferFunction1D ( const Colors& colors );

  /// Get the number of dimensions for this transfer function.
  virtual unsigned int   dimensions () const;

  /// Get/Set the size.
  unsigned int           size () const;
  void                   size ( unsigned int );

  /// Get/Set the mininium value.
  double                 minimium () const;
  void                   minimium ( double );

  /// Get/Set the maximium value.
  double                 maximium () const;
  void                   maximium ( double );

  /// Get/Set the colors.
  void                   colors ( const Colors& colors );
  const Colors&          colors ( ) const;

  /// Get/Set the value.
  void                   value ( unsigned int index, const Color& color );
  const Color&           value ( unsigned int index ) const;

protected:
  virtual ~TransferFunction1D ();

  void                   _init ( );
private:
  unsigned int _size;
  double _minimium;
  double _maximium;
  Colors _colors;
};

}
}

#endif // __OSG_TOOLS_VOLUME_TRANSFER_FUNCTION_1D_H__
