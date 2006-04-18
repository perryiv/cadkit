
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2004, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __OSG_FOX_PREFERENCES_H__
#define __OSG_FOX_PREFERENCES_H__

#include "Helios/Fox/Core/Export.h"

#include "FoxTools/Headers/DialogBox.h"

#include "Usul/Interfaces/Fox/IFoxTabItem.h"
#include "Usul/Interfaces/Fox/IFoxTabBook.h"

#include <string>
#include <map>
#include <vector>

namespace Helios {
namespace Core {

class OSG_FOX_CORE_EXPORT Preferences : public FX::FXDialogBox,
                                         public Usul::Interfaces::IFoxTabItem,
                                         public Usul::Interfaces::IFoxTabBook
{
public:

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  typedef FX::FXDialogBox BaseClass;

  static Preferences* instance();
  static void owner ( FX::FXWindow *w );
  static void releaseInstance();
 
  enum
  {
    ID_SET_ANIMATION_TIME = BaseClass::ID_LAST,
    ID_WARN_NO_UNDO,
    ID_VIEW_ALL_ON_LOAD,
    ID_CLEAR_REGISTRY,
    ID_LAST
  };

  void showDialog(  );

  void writeOutValues() const;

  long                      onCommandSetAnimateTime    ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandWarnNoUndo        ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandViewAllOnLoad     ( FX::FXObject *, FX::FXSelector, void * );
  long                      onCommandClearRegistry     ( FX::FXObject *, FX::FXSelector, void * );

  long                      onUpdateSetAnimateTime     ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateWarnNoUndo         ( FX::FXObject *, FX::FXSelector, void * );
  long                      onUpdateViewAllOnLoad      ( FX::FXObject *, FX::FXSelector, void * );

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IFoxTabItem
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual void                  addTab    ( const std::string &name );
  virtual void                  deleteTab ( const std::string &name );
  virtual void                  deleteTabs();
  virtual FX::FXTabItem *       tabItem   ( const std::string &name );
  virtual const FX::FXTabItem * tabItem   ( const std::string &name ) const;

  /////////////////////////////////////////////////////////////////////////////
  //
  //  Usul::Interfaces::IFoxTabBook
  //
  /////////////////////////////////////////////////////////////////////////////

  virtual FX::FXTabBook *       tabBook  (  );
  virtual const FX::FXTabBook * tabBook  (  ) const;

protected:

  Preferences ( ) { }
  Preferences( FX::FXWindow * );
  virtual ~Preferences();

  Preferences( Preferences * ); // no copying
  Preferences* operator= ( Preferences* ); //no assignment

  void         _initGeneralTab();

  void         _populate();

private:

  // Useful typedefs.
  typedef std::map < std::string, FX::FXTabItem* > TabItems;

  FX::FXTabBook *_tabBook;
  //Map to hold the tab items
  TabItems _tabItems;

  static Preferences *_instance;
  static FX::FXWindow *_owner;

  FXDECLARE ( Preferences );
};

} // namespace Core
} // namespace Helios


#endif //__OSG_FOX_PREFERENCES_H__


