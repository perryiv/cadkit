
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002, Perry L. Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Document class.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _USUL_DOCUMENT_H_
#define _USUL_DOCUMENT_H_

#include "Usul/Export/Export.h"

#include "Usul/Documents/FileInfo.h"

#include "Usul/Base/Referenced.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/IMessageQueuePost.h"
#include "Usul/Interfaces/IMessageQueueFlush.h"
#include "Usul/Interfaces/IRead.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/IGetTitle.h"
#include "Usul/Interfaces/ICanClose.h"
#include "Usul/Interfaces/ICanInsert.h"
#include "Usul/Interfaces/IGUIDelegate.h"

#include <string>
#include <list>
#include <vector>
#include <map>
#include <iosfwd>


namespace Usul {
namespace Documents {


class USUL_EXPORT Document : public Usul::Base::Referenced,
                             public Usul::Interfaces::IDocument,
                             public Usul::Interfaces::IMessageQueuePostUInt,
                             public Usul::Interfaces::IMessageQueueFlush,
                             public Usul::Interfaces::IRead,
                             public Usul::Interfaces::IGetTitle,
                             public Usul::Interfaces::ICanClose,
                             public Usul::Interfaces::ICanInsert
{
public:

  /// Typedefs.
  typedef Usul::Base::Referenced                BaseClass;
  typedef Usul::Interfaces::IUnknown            IUnknown;
  typedef std::list<IUnknown::ValidRefPtr>      Unknowns;
  typedef std::pair<std::string,std::string>    Filter;
  typedef std::vector<Filter>                   Filters;
  typedef std::map<std::string,std::string>     Options;
  typedef Unknowns::iterator                    UnknownItr;
  typedef Unknowns::const_iterator              UnknownConstItr;
  typedef Usul::Interfaces::IDocument           IDocument;
  typedef Usul::Interfaces::IGUIDelegate        Delegate;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Document );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  Document ( const std::string &type );

  // Get/Set the active view
  virtual void                activeView  ( IUnknown *view );
  virtual IUnknown *          activeView() const;

  /// Add a window to the proper set.
  virtual void                addWindow ( IUnknown *window );
  virtual void                addView   ( IUnknown *view   );

  // The given window is closing
  virtual void                closing ( IUnknown *window );

  /// Iterators to the begining of the windows
  UnknownItr                  beginWindows()       { return _windows.begin(); }
  UnknownConstItr             beginWindows() const { return _windows.begin(); }

  /// Set/get the whether or not this document is binary.
  bool                        binary() const;
  void                        binary ( bool b );

  /// Ask the document if the window can close.
  bool                        canClose ( IUnknown *window, IUnknown *caller, bool checkNumWindows );
  bool                        canClose ( IUnknown *caller );

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &ext ) const = 0;
  virtual bool                canInsert ( const std::string &ext ) const = 0;
  virtual bool                canOpen   ( const std::string &ext ) const = 0;
  virtual bool                canSave   ( const std::string &ext ) const = 0;

  /// Clear any existing data.
  virtual void                clear ( IUnknown *caller = 0x0 ) = 0;

  /// Close all referenced windows except one specified
  bool                        closeWindows ( IUnknown *caller = 0x0, const IUnknown *skip = 0x0);

  /// Create default GUI
  virtual void                createDefaultGUI ( IUnknown *caller = 0x0 );

  // Get/Set the Delegate.  May be null.
  void                        delegate ( Delegate *delegate );
  Delegate*                   delegate ( );

  /// Iterators to the end
  UnknownItr                  endWindows()       { return _windows.end(); }
  UnknownConstItr             endWindows() const { return _windows.end(); }

  /// Prompt user for documents to export.
  void                        exportDocument ( IUnknown *caller = 0x0 );

  /// Set/get the file format.
  virtual IDocument::Format   fileFormat() const { return _file.format(); }
  virtual void                fileFormat ( const IDocument::Format &f ) { _file.format ( f ); }

  /// Set/get the file name.
  virtual std::string         fileName() const { return _file.name(); }
  virtual void                fileName ( const std::string &n ) { _file.name ( n ); }

  /// Set/get the valid flag.
  virtual bool                fileValid() const { return _file.valid(); }
  virtual void                fileValid ( bool v ) { _file.valid ( v ); }

  /// Get the filters that correspond to what this document can do.
  virtual Filters             filtersExport() const = 0;
  virtual Filters             filtersInsert() const = 0;
  virtual Filters             filtersOpen()   const = 0;
  virtual Filters             filtersSave()   const = 0;

  // Flush the event queue. Gives the user the opportunity to cancel.
  void                        flushEvents();

  /// Usul::Interfaces::IGetTitle
  virtual std::string         getTitle ( IUnknown *caller );

  /// Prompt user for documents to insert into this one.
  void                        insert ( IUnknown *caller = 0x0 );

  /// Post and flush messages. Returns true if it worked.
  virtual bool                messageQueueFlushAll();
  virtual bool                messageQueueFlushOne();
  virtual bool                messageQueuePost ( unsigned int message );

  /// Set/get the modified flag.
  virtual bool                modified() const { return _file.modified(); }
  virtual void                modified ( bool m ) { _file.modified ( m ); }

  /// Notify this document of the message.
  virtual void                notify ( unsigned int message );

  // Notify the document that it is no longer active.
  virtual void                noLongerActive ( const std::string& activeType );

  // Notify the document that it is active.
  virtual void                nowActive ( const std::string& oldType );

  /// Return the number of windows.
  unsigned int                numWindows() const { return _windows.size();   }
  unsigned int                numViews()   const { return _views.size();     }

  /// Open the file. Clears any data this document already has.
  virtual void                open ( const std::string &filename, IUnknown *caller = 0x0 );

  /// Get the options
  Options&                    options()       { return _options; }
  const Options&              options() const { return _options; }

  /// Read the file and add it to existing document's data.
  virtual void                read ( const std::string &filename, IUnknown *caller = 0x0 ) = 0;

  /// Refresh the view
  virtual void                refreshView ( IUnknown *view );

  /// Remove a window from the proper set.
  virtual void                removeWindow ( IUnknown *window );
  virtual void                removeView   ( IUnknown *view   );

  /// Save the document to existing file name.
  void                        save ( IUnknown *caller = 0x0 );

  /// Always prompts for new file name.
  void                        saveAs ( IUnknown *caller = 0x0 );

  /// Convenience function to set progress bar and flush events.
  void                        setProgressBar ( bool state, unsigned int numerator, unsigned int denominator );
  void                        setProgressBar ( bool state, std::ifstream &in, unsigned int fileSize );

  /// Convenience function to set status bar and flush events.
  void                        setStatusBar ( const std::string &text );

  /// Return the name of this type of document.
  virtual std::string         typeName() const { return _typeName; }

  /// Bring the windows forward
  void                        windowsForward();

  /// Write the document to given file name. Does not rename this document.
  virtual void                write ( const std::string &filename, IUnknown *caller = 0x0  ) const = 0;

protected:

  /// Do not copy.
  Document ( const Document & );
  Document &operator = ( const Document & );

  /// Use reference counting.
  virtual ~Document();

  std::string                 _getSaveAsFileName ( Options &options, IUnknown *caller = 0x0 );

  void                        _postMessage ( Unknowns &listeners, unsigned int message, const IUnknown *skip );

  void                        _save ( const std::string &filename, IUnknown *caller, const Options &options = Options() );

  IUnknown *                  _unknown();

private:

  FileInfo _file;
  Unknowns _windows;
  Unknowns _views;
  IUnknown::RefPtr _active;
  std::string _typeName;
  Delegate::RefPtr _delegate;

  // TODO may need a std::map < Usul::Interfaces::IViewer*, Options >
  Options _options;
};


}; // namespace Documents
}; // namespace Usul


#endif // _USUL_DOCUMENT_H_
