
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Window builder class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _FOX_TOOLS_BUILDER_READER_H_
#define _FOX_TOOLS_BUILDER_READER_H_

#include "FoxTools/Export/Export.h"

#include <string>
#include <stack>
#include <map>

namespace FX { class FXWindow; }


namespace FoxTools {
namespace Builder {

struct Factory;

class FOX_TOOLS_EXPORT Reader
{
public:

  // Useful typedefs.
  typedef std::stack < FX::FXWindow * > WindowStack;
  typedef std::map < std::string, Factory * > Factories;
  typedef std::map < std::string, unsigned int > LayoutHints;

  // Constructor and destructor.
  Reader();
  ~Reader();

  // Read the xml configuration file and build the widget tree.
  void                    read ( const std::string &file, FX::FXWindow *parent );

protected:

  // Do not copy.
  Reader ( const Reader & );
  Reader &operator = ( const Reader & );

  // Callback class.
  struct Callback
  {
    explicit Callback();
    void set ( Reader *r );
    void start ( const std::string &name, const std::string &value );
    void end ( const std::string &name );
    void clear();
  private:
    Reader *_r;
  };

  friend struct Callback;

  void                    _clear();
  void                    _color ( const std::string & );
  FX::FXWindow *          _create ( const std::string &name, Factory *f );
  FX::FXWindow *          _current();

  void                    _end ( const std::string &name );

  Factory *               _factory ( const std::string & );

  void                    _layout ( const std::string & );

  void                    _title ( const std::string & );
  void                    _text ( const std::string & );

  void                    _start ( const std::string &name, const std::string &value );

private:

  std::string _file;
  WindowStack _stack;
  Factories _factories;
  LayoutHints _hints;
};


}; // namespace Builder
}; // namespace FoxTools


#endif // _FOX_TOOLS_BUILDER_READER_H_
