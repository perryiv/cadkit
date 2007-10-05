
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, John K. Grant.
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Menu button.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _MENUKIT_BUTTON_
#define _MENUKIT_BUTTON_

#include "MenuKit/Item.h"

#include "Usul/Commands/Command.h"

namespace MenuKit {

class Visitor;

class MENUKIT_EXPORT Button : public Item
{
public:

  // Typedefs and smart-pointers.
  typedef Item                     BaseClass;
  typedef Usul::Commands::Command  Command;
  USUL_DECLARE_REF_POINTERS ( Button );

  // Construction.
  Button();
  Button( Usul::Commands::Command * );

  // Accept the visitor.
  virtual void        accept ( Visitor &v );

  // Set/get the icon filename.
  void                  iconFile ( const std::string &f );
  std::string           iconFile() const;

  // Set/get the text string.
  void                  text ( const std::string &t );
  std::string           text() const;

  // Set/get the information string.
  void                  info ( const std::string &s );
  std::string           info() const;

  /// Get/Set the command.
  void                  command ( Command * command );
  Command *             command ();
  const Command *       command () const;

  // Set/get the separator flag.
  bool                  separator() const;
  void                  separator ( bool e );

  // Set/get the checked flag.
  bool                  checked() const;
  void                  checked ( bool e );

  // Set/get the toggle flag.
  bool                  toggle() const;
  void                  toggle ( bool e );

  // Set/get the radio flag.
  bool                  radio() const;
  void                  radio ( bool e );

  // Set/get the flags.
  unsigned int          flags() const;
  void                  flags ( unsigned int f );

protected:

  // Use reference counting.
  virtual ~Button();

  Button ( const Button & );

  // Assignment.
  Button& operator = ( const Button & );

private:
  // Possible flags.
  enum
  {
    CHECKED     = 0x00000001, // fill in the toggle/radio
    TOGGLE      = 0x00000002, // button may be turned on/off
    RADIO       = 0x00000004, // button may be turned on/off but only one should be 'on'
    SEPARATOR   = 0x00000008  // could be used to draw separation lines
  };

  Command::RefPtr _command;
  unsigned int _flags;
};


}; // namespace MenuKit


#endif // _MENUKIT_BUTTON_
