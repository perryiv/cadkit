
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/ComputeNormals/ComputeNormalsComponent.h"

USUL_IMPLEMENT_IUNKNOWN_MEMBERS ( ComputeNormalsComponent, ComputeNormalsComponent::BaseClass );


///////////////////////////////////////////////////////////////////////////////
//
// Constructor
//
///////////////////////////////////////////////////////////////////////////////

ComputeNormalsComponent::ComputeNormalsComponent()
{
}


///////////////////////////////////////////////////////////////////////////////
//
// Destructor
//
///////////////////////////////////////////////////////////////////////////////

ComputeNormalsComponent::~ComputeNormalsComponent() 
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query for the interface.
//
///////////////////////////////////////////////////////////////////////////////

Usul::Interfaces::IUnknown *ComputeNormalsComponent::queryInterface ( unsigned long iid )
{
  switch ( iid )
  {
  case Usul::Interfaces::IUnknown::IID:
  case Usul::Interfaces::IComputeTriangleNormals::IID:
    return static_cast < Usul::Interfaces::IComputeTriangleNormals*>(this);
  case Usul::Interfaces::IPlugin::IID:
    return static_cast < Usul::Interfaces::IPlugin* > ( this );
  default:
    return 0x0;
  }
}



///////////////////////////////////////////////////////////////////////////////
//
//  
//
///////////////////////////////////////////////////////////////////////////////

void ComputeNormalsComponent::computeNormalsPerTriangle ( const Vertices& vertices, Normals& normals )
{
  unsigned int numVertices ( vertices.size() );
  unsigned int numTriangles ( vertices.size() / 3 );

  // Clear.
  normals.clear();

  // Make room.
  normals.reserve ( numTriangles );

  for( Vertices::const_iterator iter = vertices.begin(); iter != vertices.end(); iter += 3)
  {
    Vertices::value_type v0 ( *iter );
    Vertices::value_type v1 ( *(iter + 1 ) );
    Vertices::value_type v2 ( *(iter + 2 ) );

    Vertices::value_type a ( v2 - v1 );
    Vertices::value_type b ( v0 - v1 );

    Normals::value_type normal ( a.cross ( b ) );
    normal.normalize();

    // Get the center of the triangle.
    Vertices::value_type center ( v0 + v1 + v2 ); 
    center /= 3;

    // Get the center of the model.
    Vertices::value_type cg ( 0.0, 0.0, 0.0 );

    // Make the reference vector.
    Vertices::value_type refVector ( center - cg );

    // Flip the normal if it's pointing towards the center. 
    if ( ( refVector.dot ( normal ) ) < 0 )
      normal = -normal;

    // Add the normal.
    normals.push_back ( normal );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Usul::Interfaces::IPlugin implementation
//
///////////////////////////////////////////////////////////////////////////////

std::string ComputeNormalsComponent::getPluginName() const 
{
  return "Compute Normals";
}
