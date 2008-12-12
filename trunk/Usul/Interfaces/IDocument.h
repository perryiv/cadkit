
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Interface for getting the document
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_INTERFACE_DOCUMENT_H_
#define _USUL_INTERFACE_DOCUMENT_H_

#include "Usul/Interfaces/IUnknown.h"

#include <string>
#include <iosfwd>
#include <utility>
#include <vector>

namespace Usul { namespace Jobs { class Job; } }
namespace Usul { namespace Interfaces { struct IWindow; struct IView; } }


namespace Usul {
namespace Interfaces {


struct IDocument : public Usul::Interfaces::IUnknown
{
  /// Typedef(s).
  typedef Usul::Interfaces::IUnknown            Unknown;
  typedef std::pair<std::string,std::string>    Filter;
  typedef std::vector<Filter>                   Filters;

  /// Format-format enumerations.
  enum Format
  {
    FILE_FORMAT_ASCII,
    FILE_FORMAT_BINARY,
    FILE_FORMAT_UNKNOWN
  };

  /// Smart-pointer definitions.
  USUL_DECLARE_QUERY_POINTERS ( IDocument );

  /// Id for this interface.
  enum { IID = 3762815634u };

  virtual void                          addWindow   ( Usul::Interfaces::IWindow *window ) = 0;
  virtual void                          addView     ( Usul::Interfaces::IView *view   ) = 0;

  /// Return true if this document can do it.
  virtual bool                          canExport ( const std::string &ext ) const = 0;
  virtual bool                          canInsert ( const std::string &ext ) const = 0;
  virtual bool                          canOpen   ( const std::string &ext ) const = 0;
  virtual bool                          canSave   ( const std::string &ext ) const = 0;

  /// Return the job to close this document. Default is null.
  virtual Usul::Jobs::Job *             closeJob() = 0;

  /// The following window is closing
  virtual void                          closing     ( Usul::Interfaces::IWindow *window ) = 0;

  /// Create default GUI
  virtual void                          createDefaultGUI ( Usul::Interfaces::IUnknown *caller = 0x0 ) = 0;

  virtual Format                        fileFormat() const = 0;
  virtual void                          fileFormat ( const Format & ) = 0;

  virtual void                          fileName ( const std::string &f ) = 0;
  virtual const std::string &           fileName() const = 0;

  virtual bool                          fileValid() const = 0;
  virtual void                          fileValid ( bool ) = 0;

  /// Get the filters that correspond to what this document can do.
  virtual Filters                       filtersExport() const = 0;
  virtual Filters                       filtersInsert() const = 0;
  virtual Filters                       filtersOpen()   const = 0;
  virtual Filters                       filtersSave()   const = 0;

  /// Prompt user for documents to insert into this one.
  virtual void                          insert ( Unknown *caller = 0x0 ) = 0;

  virtual bool                          modified() const = 0;
  virtual void                          modified ( bool ) = 0;

  /// Save the document to existing file name.
  virtual void                          save ( Unknown *caller = 0x0, Unknown *progress = 0x0, std::ostream *out = 0x0 ) = 0;

  /// Always prompts for new file name.
  virtual void                          saveAs ( Unknown *caller = 0x0, Unknown *progress = 0x0, std::ostream *out = 0x0 ) = 0;

  // Returns appropriate tag name for registry.
  virtual std::string                   registryTagName() const = 0;

  virtual void                          removeWindow   ( Usul::Interfaces::IWindow *window ) = 0;
  virtual void                          removeView     ( Usul::Interfaces::IView *view   ) = 0;

  /// Ask the views to redraw.
  virtual void                          requestRedraw() = 0;
  virtual bool                          allowRequestRedraw() const = 0;
  virtual void                          allowRequestRedraw ( bool ) = 0;

  virtual std::string                   typeName() const = 0;

  virtual void                          updateGUI() = 0;

  /// Write the document to given file name. Does not rename this document.
  virtual void                          write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0  ) const = 0;
};


} // namespace Interfaces
} // namespace Usul


#endif // _USUL_INTERFACE_DOCUMENT_H_
