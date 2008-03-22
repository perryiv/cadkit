
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interaction functors.
//
///////////////////////////////////////////////////////////////////////////////

#include "GSG/Core/Precompiled.h"
#include "GSG/Core/Interaction.h"
#include "GSG/Core/Viewer.h"

using namespace GSG;


/////////////////////////////////////////////////////////////////////////////
//
//  Base Interaction functor.
//
/////////////////////////////////////////////////////////////////////////////

Interaction::Interaction() : Referenced(){}
Interaction::Interaction ( const Interaction &i ) : Referenced ( i ){}
Interaction::~Interaction(){}


/////////////////////////////////////////////////////////////////////////////
//
//  Trackball-rotation functor.
//
/////////////////////////////////////////////////////////////////////////////

GSG_IMPLEMENT_REFERENCED ( Trackball );
Trackball::Trackball() : Interaction(){}
Trackball::Trackball ( const Trackball &tb ) : Interaction ( tb ){}
Trackball::~Trackball(){}
void Trackball::setFrom ( const Trackball &t ) { BaseClass::setFrom ( t ); }


/////////////////////////////////////////////////////////////////////////////
//
//  Calculate the new matrix.
//
/////////////////////////////////////////////////////////////////////////////

void Trackball::operator () ( Viewer &v )
{
  GSG_ASSERT ( 0 ); // TODO
}


/////////////////////////////////////////////////////////////////////////////
//
//  Translation-interaction functor.
//
/////////////////////////////////////////////////////////////////////////////

GSG_IMPLEMENT_REFERENCED ( Translate );
Translate::Translate() : Interaction(){}
Translate::Translate ( const Translate &tr ) : Interaction ( tr ){}
Translate::~Translate(){}
void Translate::setFrom ( const Translate &t ) { BaseClass::setFrom ( t ); }


/////////////////////////////////////////////////////////////////////////////
//
//  Calculate the new matrix.
//
/////////////////////////////////////////////////////////////////////////////

void Translate::operator () ( Viewer &v )
{
  GSG_ASSERT ( 0 ); // TODO
}


/////////////////////////////////////////////////////////////////////////////
//
//  Scale-interaction functor.
//
/////////////////////////////////////////////////////////////////////////////

GSG_IMPLEMENT_REFERENCED ( Scale );
Scale::Scale() : Interaction(){}
Scale::Scale ( const Scale &s ) : Interaction ( s ){}
Scale::~Scale(){}
void Scale::setFrom ( const Scale &s ) { BaseClass::setFrom ( s ); }


/////////////////////////////////////////////////////////////////////////////
//
//  Scale the camera.
//
/////////////////////////////////////////////////////////////////////////////

void Scale::operator () ( Viewer &v )
{
  GSG_ASSERT ( 0 ); // TODO
}