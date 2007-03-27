/*
 *  CocoaProgressBar.h
 *  Helios
 *
 *  Created by Michael A Jackson on 12/27/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */
#ifndef __CocoaProgressBar_H
#define __CocoaProgressBar_H 


#import <Cocoa/Cocoa.h>

#include "Usul/Interfaces/GUI/IProgressBar.h"
#include "Usul/Interfaces/GUI/IStatusBar.h"
#include "Usul/Base/Referenced.h"

class CocoaProgressBar : public Usul::Interfaces::IProgressBar,
                         public Usul::Base::Referenced,
                         public Usul::Interfaces::IStatusBar
{
  

  public:
    USUL_DECLARE_QUERY_POINTERS ( CocoaProgressBar );
  
  
    typedef Usul::Base::Referenced BaseClass;
  
    CocoaProgressBar( id inObj, id progressBar, id statusText );
    ~CocoaProgressBar();

///////////////////////////////////////////////////////////////////////////////
//
//        IProgressBar Interface
//
///////////////////////////////////////////////////////////////////////////////

    // Show the progress bar
    void showProgressBar();
    
    // Set the total of progress bar
    void setTotalProgressBar ( unsigned int value );
    
    // Update the progress bar
    void updateProgressBar ( unsigned int value );
    
    // Hide the progress bar
    void hideProgressBar();
    
    
///////////////////////////////////////////////////////////////////////////////
//
//    IStatusBar Interface
//
///////////////////////////////////////////////////////////////////////////////

    // Set the status bar text.
    void setStatusBarText ( const std::string &text, bool force );
    
    
    void ref ();
    void unref ( bool allowDeletion = true );
    Usul::Interfaces::IUnknown* queryInterface ( unsigned long iid );
    
    
  protected:


  private:
      CocoaProgressBar(); 
    id		_theObj;
    id    _progressIndicator;
    id    _textField;


};




#endif