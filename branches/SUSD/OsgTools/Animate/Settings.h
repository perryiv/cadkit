
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSGTOOLS_ANIMATE_SETTINGS_H__
#define __OSGTOOLS_ANIMATE_SETTINGS_H__  

#include "OsgTools/Export.h"

#include "OsgTools/Animate/Date.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"

namespace OsgTools
{
  namespace Animate
  {
    class OSG_TOOLS_EXPORT Settings : public Usul::Base::Referenced 
    {
    public:
      typedef Usul::Base::Referenced BaseClass;

      USUL_DECLARE_REF_POINTERS( Settings );

      Settings();

      enum TimestepType
      {
        DAY,
        MONTH,
        YEAR
      };

      /// First date showing.
      void                                firstDate( const OsgTools::Animate::Date& date );
      const OsgTools::Animate::Date&      firstDate() const;

      /// Last date showing.
      void                                lastDate( const OsgTools::Animate::Date& date );
      const OsgTools::Animate::Date&      lastDate() const;

      inline bool                         isDateShown( const OsgTools::Animate::Date& date ) const;
      inline bool                         isDateRangeShown ( const OsgTools::Animate::Date& first, const OsgTools::Animate::Date& last ) const;

      /// Get/Set the animation flag.
      void                                animate( bool b );
      bool                                animate() const;

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

    protected:
      virtual ~Settings();

    private:
      OsgTools::Animate::Date _firstDate;
      OsgTools::Animate::Date _lastDate;
      boost::gregorian::date_period  _period;

      bool          _animate;
      bool          _showPastDays;
      bool          _timeWindow;
      unsigned int  _windowLength;
      TimestepType  _type;
    };

    inline bool Settings::isDateShown( const OsgTools::Animate::Date& date ) const
    {
      return ( date >= _firstDate && date <= _lastDate );
    }

    inline bool Settings::isDateRangeShown ( const OsgTools::Animate::Date& first, const OsgTools::Animate::Date& last ) const
    {
      // Have to add one more the end of the date duration.
      // This is because if the duration is 1/1/2006 to 1/31/2006 and the date is 1/31/2006, the contains function will return false.
      OsgTools::Animate::Date temp ( last );
      temp.increment();
      boost::gregorian::date_period period ( first.date(), temp.date() );

      return _period.intersects ( period );
    }
  }
}

#endif // __OSGTOOLS_ANIMATE_SETTINGS_H__
