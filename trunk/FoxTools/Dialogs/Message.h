
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Message dialog.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_MESSAGE_DIALOG_H_
#define _FOX_TOOLS_MESSAGE_DIALOG_H_

#include "FoxTools/Export/Export.h"
#include "FoxTools/Headers/Object.h"

#include <vector>
#include <map>
#include <string>

namespace FX { class FXObject; class FXIcon; class FXRegistry; };


namespace FoxTools {
namespace Dialogs {


class FOX_TOOLS_EXPORT Message : FX::FXObject
{
public:

  /// Alignment of message text within their cells.
  /// TODO, this is not hooked up yet.
  enum HorizontalAlignment { ALLIGN_LEFT, ALLIGN_CENTER, ALLIGN_RIGHT  };
  enum VerticalAlignment   { ALLIGN_TOP,  ALLIGN_MIDDLE, ALLIGN_BOTTOM };

  /// Useful typedefs.
  typedef FX::FXObject BaseClass;
  typedef std::vector<std::string> Buttons;
  typedef std::map<std::string,bool> Checks;
  typedef std::pair<HorizontalAlignment,VerticalAlignment> Alignment;
  typedef std::pair<std::string,Alignment> Cell;
  typedef std::vector<Cell> Row;
  typedef std::vector<Row> Cells;
  typedef std::pair<bool,bool> Bools;
  typedef std::pair<std::string,Bools> CacheIf;
  typedef std::pair<FX::FXIcon*,bool> Icon;

  /// Messages.
  enum
  {
    ID_CLOSE,
    ID_CHECK_BUTTON,
    ID_LAST
  };

  /// Default constructor.
  Message();

  /// Construct message dialog with single "OK" button.
  Message ( const std::string &title,
            const std::string &message );

  /// Construct a "Remember this decision" dialog.
  Message ( const std::string &id, bool offerToRemember, bool persistAnswerToDisk );

  /// Destructor.
  ~Message();

  /// Convenience functions to set some buttons. Deletes any existing buttons. 
  /// In the second function, separate buttons with a '|' character. 
  /// For example, "OK|Cancel".
  void                    button ( const std::string &b );
  void                    buttons ( const std::string &b );
  void                    buttons ( const std::string &b1, const std::string &b2 );
  void                    buttons ( const std::string &b1, const std::string &b2, const std::string &b3 );

  /// Return the buttons.
  Buttons &               buttons()       { return _buttons; }
  const Buttons &         buttons() const { return _buttons; }

  /// Cache the results of run() if this check-button has the final state.
  /// Last argument determines if decision persists past program exit.
  void                    cacheIf ( const std::string &check, bool state, bool persist );

  /// Set/get the string to return if the user closes the dialog with the 
  /// window manager's close button, or if the escape keys is pressed.
  const std::string &     closeButton() const;
  void                    closeButton ( const std::string &c );

  /// Return the cell.
  const Cell &            cell ( unsigned int row, unsigned int column ) const;
  Cell &                  cell ( unsigned int row, unsigned int column );

  /// Return the checks.
  Checks &                checks()       { return _checks; }
  const Checks &          checks() const { return _checks; }

  /// Convenience function to display an error dialog. Separate buttons 
  /// with a '|' character. For example, "OK|Cancel".
  static std::string      error ( const std::string &buttons, 
                                  const std::string &title, 
                                  const std::string &text,
                                  FX::FXObject *owner = 0x0 );

  /// Set/get the icon. Pass true for "destroy" if this instance should 
  /// delete the icon in the destructor. The last function creates the icon.
  /// The object is used to get a pointer to the application.
  const Icon &            icon() const;
  void                    icon ( FX::FXIcon *, bool destroy );
  void                    icon ( unsigned int id );

  /// Set/get the string used to save the result.
  const std::string &     id() const;
  void                    id ( const std::string &s );

  /// Convenience function to display an information dialog. Separate buttons 
  /// with a '|' character. For example, "OK|Cancel".
  static std::string      info ( const std::string &buttons, 
                                 const std::string &title, 
                                 const std::string &text,
                                 FX::FXObject *owner = 0x0  );

  /// Convenience function to display a question dialog. Separate buttons 
  /// with a '|' character. For example, "Yes|No|Cancel".
  static std::string      question ( const std::string &buttons, 
                                     const std::string &title, 
                                     const std::string &text,
                                     FX::FXObject *owner = 0x0  );

  /// Set resizing flag.
  void                    resizable ( bool r );
  bool                    resizable() const;

  /// Message maps.
  long                    onCommandClose       ( FX::FXObject *, FX::FXSelector, void * );
  long                    onCommandCheckButton ( FX::FXObject *, FX::FXSelector, void * );

  /// Run the dialog in a modal loop. Note: default placement is FX::PLACEMENT_OWNER.
  std::string             run ( FX::FXObject *owner, unsigned int placement = 4 );

  /// Return the text at the cell.
  const std::string &     text ( unsigned int row, unsigned int column ) const;
  std::string &           text ( unsigned int row, unsigned int column );

  /// Convenience function for setting the title and message string. 
  /// This makes one cell and deletes any existing cells.
  void                    text ( const std::string &title, const std::string &message );
  void                    text ( const std::string &message );

  /// Set/get the title.
  void                    title ( const std::string &t ) { _title = t; }
  const std::string &     title() const { return _title; }

protected:

  // No copying or assigning.
  Message ( const Message & );
  Message &               operator = ( const Message & );

  std::string             _find ( FX::FXRegistry &reg ) const;

  void                    _grow ( unsigned int rows, unsigned int columns );

  void                    _save ( const std::string &result, FX::FXRegistry &reg ) const;

private:

  std::string _title;
  Cells _cells;
  Checks _checks;
  Buttons _buttons;
  bool _resizable;
  Icon _icon;
  std::string _result;
  std::string _close;
  std::string _id;
  CacheIf _cacheIf;

  FXDECLARE ( Message );
};


}; // namespace Dialogs
}; // namespace FoxTools


#endif // _FOX_TOOLS_FILE_SELECTION_DIALOG_H_
