
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

#ifndef _SHOW_TIME_MODEL_FADE_H_
#define _SHOW_TIME_MODEL_FADE_H_

#include "ShowtimeModel/Actions/Action.h"

#include "Usul/Math/Vector2.h"


namespace Showtime {
namespace Actions {


class Fade : public Action
{
public:

  // Useful typedefs.
  typedef Action BaseClass;
  typedef Usul::Math::Vec2d AlphaRange;

  // Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Fade );

  // Construction.
  Fade();

  // Get the alpha-range.
  AlphaRange                  alphaRange() const;

  // Execute the action.
  virtual void                execute ( unsigned int step );

  // Initialize.
  virtual void                init ( ShowtimeDocument *document, const XmlTree::Node *node );

protected:

  // Use reference counting.
  virtual ~Fade();

private:

  // Do not copy.
  Fade ( const Fade & );
  Fade &operator = ( const Fade & );

  void                        _destroy();

private:

  AlphaRange _alphaRange;
};


} // namespace Fade
} // namespace Showtime


#endif // _SHOW_TIME_MODEL_FADE_H_
