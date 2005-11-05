
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Find loops for capping
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_FIND_LOOPS_H_
#define _USUL_INTERFACE_FIND_LOOPS_H_

#include "Usul/Interfaces/IUnknown.h"

#include "Usul/Interfaces/IStatusBar.h"
#include "Usul/Interfaces/IFlushEvents.h"
#include "Usul/Interfaces/IProgressBar.h"

#include "Usul/Policies/Update.h"
#include <list>


namespace Usul {
namespace Interfaces {


struct IFindLoops : public Usul::Interfaces::IUnknown
{
  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IFindLoops );

  /// Id for this interface.
  enum { IID = 2032218975u };

  virtual void findLoops( Usul::Interfaces::IUnknown* caller ) = 0;

  typedef std::list < unsigned int > IndexSequence;
    
  //Helper to update progress
  struct UpdateFindingLoops
  {
    template< class T > UpdateFindingLoops( double start, double finish, T *t ) :
    _progressBar( start, finish, t ),
    _statusBar( t ),
    _updatePolicy ( 500 ),
    _flush ( t )
    {
    }

      
///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

template < class IndexSequence >
void operator() ( IndexSequence& uncapped, std::string str, bool forceRedraw = false )
{
  if( _updatePolicy() || forceRedraw )
  {
    //Set the status bar
    std::ostringstream os;
    unsigned long numCapped ( uncapped.size() );
    os << str << numCapped;
    _statusBar ( os.str(), true );

    if( _flush.valid() )
      _flush->flushEventQueue();
  }
}

  
///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void operator () ( unsigned int current, unsigned int total )
{
  if( _updatePolicy() )
    _progressBar ( current, total );
}

  
///////////////////////////////////////////////////////////////////////////////
//
//
//
///////////////////////////////////////////////////////////////////////////////

void operator () ( unsigned int numLoops, std::string str, bool force = false )
{
  if( _updatePolicy() || force )
  {
    std::ostringstream os;
    os << str << numLoops;
    _statusBar ( os.str(), true );

    if( _flush.valid() )
      _flush->flushEventQueue();
  }
}

  private:
    Usul::Interfaces::IProgressBar::UpdateProgressBar _progressBar;
    Usul::Interfaces::IStatusBar::UpdateStatusBar   _statusBar;
    Usul::Policies::TimeBased _updatePolicy;

    Usul::Interfaces::IFlushEvents::QueryPtr _flush;
  };

}; // struct IFindLoops


}; // namespace Interfaces
}; // namespace Usul


#endif // _USUL_INTERFACE_FIND_LOOPS_H_
