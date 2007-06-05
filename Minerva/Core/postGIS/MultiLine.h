
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_POSTGIS_MULTI_LINE_H__
#define __MINERVA_POSTGIS_MULTI_LINE_H__

#include "Minerva/Core/Export.h"
#include "Minerva/Core/postGIS/Geometry.h"
#include "Minerva/Core/postGIS/Line.h"

namespace Minerva {
namespace Core {
namespace postGIS {

class MINERVA_EXPORT MultiLine : public Geometry
{
public:
  typedef Geometry BaseClass;

  USUL_DECLARE_QUERY_POINTERS ( MultiLine );

  MultiLine();

  void         addLine     ( Line* );
  void         addGeometry ( Geometry* );

protected:
  virtual ~MultiLine();

private:
  typedef std::vector< Line::RefPtr > Lines;

  Lines _lines;
};

}
}
}


#endif // __MINERVA_POSTGIS_MULTI_LINE_H__
