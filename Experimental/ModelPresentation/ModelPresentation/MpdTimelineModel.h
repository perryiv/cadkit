
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MODELPRESENTATION_TIMELINE_MODEL_H__
#define __MODELPRESENTATION_TIMELINE_MODEL_H__

#include "Usul/Commands/Command.h"


class MpdTimelineModel : public Usul::Commands::Command
{
public:
  typedef Usul::Commands::Command BaseClass;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( MpdTimelineModel );

  MpdTimelineModel ( Usul::Interfaces::IUnknown * caller, const std::string text, unsigned int set );

protected:
  virtual ~MpdTimelineModel ();

  virtual void _execute ();

  virtual bool updateCheck () const;

private:
  std::string             _text;
  unsigned int            _set;


};


#endif // __MODELPRESENTATION_TIMELINE_MODEL_H__
