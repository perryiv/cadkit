
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Geometry represented by a regular grid.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _OSG_TOOLS_OBJECTS_REGULAR_GRID_OBJECT_H_
#define _OSG_TOOLS_OBJECTS_REGULAR_GRID_OBJECT_H_

#include "OsgTools/Objects/VertexSequence.h"


namespace OsgTools {
namespace Objects {


class OSG_TOOLS_EXPORT RegularGrid : public OsgTools::Objects::VertexSequence
{
public:

  // Typedefs.
  typedef OsgTools::Objects::VertexSequence BaseClass;

  // Boiler-plate code.
  OSG_TOOLS_OBJECTS_DECLARE_CLASS ( RegularGrid );

  // Get/set the number of grid rows.
  unsigned int                        numRows() const;
  void                                numRows ( unsigned int );

  // Get/set the number of grid columns.
  unsigned int                        numColumns() const;
  void                                numColumns ( unsigned int );

  // Update the object.
  virtual void                        update ( Usul::Interfaces::IUnknown *caller );

protected:

  RegularGrid();
  RegularGrid ( const RegularGrid & );
  virtual ~RegularGrid();

  virtual void                        _setColorValues();
  virtual void                        _setPointValues();
  virtual void                        _setTexCoordValues();

private:

  // No assignment.
  RegularGrid &operator = ( const RegularGrid & );

  void                                _destroy();

  unsigned int _columns;
  unsigned int _rows;
};


}; // namespace Objects
}; // namespace OsgTools


#endif // _OSG_TOOLS_OBJECTS_REGULAR_GRID_OBJECT_H_
