
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Fade action.
//
///////////////////////////////////////////////////////////////////////////////

#include "ShowtimeModel/Actions/Fade.h"
#include "ShowtimeModel/ShowtimeDocument.h"

#include "XmlTree/Node.h"

#include "OsgTools/Callbacks/SortBackToFront.h"
#include "OsgTools/State/StateSet.h"

#include "Usul/Adaptors/MemberFunction.h"
#include "Usul/Convert/Vector2.h"
#include "Usul/Functions/SafeCall.h"
#include "Usul/Trace/Trace.h"

using namespace Showtime::Actions;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor
//
///////////////////////////////////////////////////////////////////////////////

Fade::Fade() : BaseClass(),
  _alphaRange ( 0, 0 )
{
  USUL_TRACE_SCOPE;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor
//
///////////////////////////////////////////////////////////////////////////////

Fade::~Fade()
{
  USUL_TRACE_SCOPE;
  Usul::Functions::safeCall ( Usul::Adaptors::memberFunction ( this, &Fade::_destroy ), "1919774201" );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destroy
//
///////////////////////////////////////////////////////////////////////////////

void Fade::_destroy()
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize
//
///////////////////////////////////////////////////////////////////////////////

void Fade::init ( ShowtimeDocument *document, const XmlTree::Node *node )
{
  USUL_TRACE_SCOPE;

  // Check input.
  if ( 0x0 == node )
    throw std::invalid_argument ( "Error 1235449705: null node passed when initializing action" );

  // Set members.
  Usul::Convert::Type<std::string,AlphaRange>::convert ( node->attribute ( "alphas" ), _alphaRange );

  // Call base class.
  BaseClass::init ( document, node );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the alpha-range.
//
///////////////////////////////////////////////////////////////////////////////

Fade::AlphaRange Fade::alphaRange() const
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );
  return _alphaRange;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Execute the action.
//
///////////////////////////////////////////////////////////////////////////////

void Fade::execute ( unsigned int step )
{
  USUL_TRACE_SCOPE;
  Guard guard ( this );

  // Handle bad state.
  if ( 0x0 == this->_getDocument() )
    return;

  // Get the model.
  Showtime::Model::RefPtr model ( this->_getDocument()->model ( this->model() ) );
  if ( false == model.valid() )
    return;

  // Make sure the model is shown.
  model->show();

  // Determine the alpha.
  const StepRange stepRange ( this->stepRange() );
  const double minStep ( stepRange[0] );
  const double maxStep ( stepRange[1] );
  const double currentStep ( step );
  const double percent ( ( currentStep - minStep ) / ( maxStep - minStep ) );
  const double alpha ( _alphaRange[0] + percent * ( _alphaRange[1] - _alphaRange[0] ) );

  // Make sure it's in range.
  if ( alpha < 0 || alpha > 1 )
    return;

  // Set the alpha.
  OsgTools::State::StateSet::setAlpha ( model->root().get(), static_cast < float > ( alpha ) );

  // If we're not completely opaque or transparent then sort triangles.
  if ( alpha > 0 && alpha < 1 )
  {
    osg::ref_ptr<osg::NodeVisitor> visitor ( new OsgTools::Callbacks::SetSortToFrontCallback );
    model->root()->accept ( *visitor );
  }
  else
  {
    osg::ref_ptr<osg::NodeVisitor> visitor ( new OsgTools::Callbacks::UnSetSortToFrontCallback );
    model->root()->accept ( *visitor );
  }
}
