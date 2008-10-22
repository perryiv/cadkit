
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __MINERVA_CORE_ANIMATE_SETTINGS_H__
#define __MINERVA_CORE_ANIMATE_SETTINGS_H__  

#include "Minerva/Core/Export.h"
#include "Minerva/Core/Animate/Date.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

namespace Minerva {
namespace Core  {
namespace Animate {

class MINERVA_EXPORT Settings : public Usul::Base::Referenced 
{
public:
  typedef Usul::Base::Referenced       BaseClass;
  typedef Minerva::Core::Animate::Date Date;

  USUL_DECLARE_REF_POINTERS ( Settings );

  Settings();

  enum TimestepType
  {
    TIMESTEP_TYPE_SECOND = Date::INCREMENT_SECOND,
    TIMESTEP_TYPE_MINUTE = Date::INCREMENT_MINUTE,
    TIMESTEP_TYPE_HOUR =   Date::INCREMENT_HOUR,
    TIMESTEP_TYPE_DAY =    Date::INCREMENT_DAY,
    TIMESTEP_TYPE_MONTH =  Date::INCREMENT_MONTH,
    TIMESTEP_TYPE_YEAR =   Date::INCREMENT_YEAR,
  };

  /// First date showing.
  void                                firstDate( const Date& date );
  const Date&                         firstDate() const;

  /// Last date showing.
  void                                lastDate( const Date& date );
  const Date&                         lastDate() const;

  /// Get/Set the animation flag.
  void                                animate( bool b );
  bool                                animate() const;

  /// Get/Set the animation speed.
  void                                speed ( double d );
  double                              speed () const;

  /// Get/set show past events flag.
  void                                showPastDays( bool b );
  bool                                showPastDays() const;

  /// Get/Set the time window flag.
  void                                timeWindow( bool b );
  bool                                timeWindow() const;

  /// Get/Set the length of the time window.
  void                                windowLength ( unsigned int legnth );
  unsigned int                        windowLength () const;

  /// Get/Set the timestep type.
  void                                timestepType( TimestepType type );
  TimestepType                        timestepType() const;

  /// Get/Set the pause flag.
  void                                pause ( bool b );
  bool                                pause () const;

protected:
  virtual ~Settings();

private:
  Date _firstDate;
  Date _lastDate;

  bool          _animate;
  bool          _pause;
  bool          _showPastDays;
  bool          _timeWindow;
  unsigned int  _windowLength;
  TimestepType  _type;
};


} // namespace Animate
} // namespace Core
} // namespace Mienrva

#endif // __MINERVA_CORE_ANIMATE_SETTINGS_H__
