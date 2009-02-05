
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2005, Perry L. Miller IV and Adam Kubach
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

#include "Usul/Base/Object.h"
#include "Usul/Interfaces/IUnknown.h"
#include "Usul/Interfaces/IDocument.h"
#include "Usul/Interfaces/IRead.h"
#include "Usul/Interfaces/IBuildScene.h"
#include "Usul/Interfaces/ICanClose.h"
#include "Usul/Interfaces/ICanInsert.h"
#include "Usul/Interfaces/IView.h"
#include "Usul/Interfaces/GUI/IWindow.h"
#include "Usul/Interfaces/GUI/IGUIDelegate.h"
#include "Usul/Interfaces/IModifiedSubject.h"
#include "Usul/Interfaces/IModifiedObserver.h"
#include "Usul/Interfaces/IRenderListener.h"
#include "Usul/Interfaces/IRedraw.h"
#include "Usul/Pointers/Pointers.h"
#include "Usul/Threads/Object.h"

#include <string>
#include <list>
#include <vector>
#include <map>
#include <iosfwd>
#include <set>


namespace Usul {
namespace Documents {


class USUL_EXPORT Document : public Usul::Base::Object,
                             public Usul::Interfaces::IDocument,
                             public Usul::Interfaces::IRead,
                             public Usul::Interfaces::ICanClose,
                             public Usul::Interfaces::ICanInsert,
                             public Usul::Interfaces::IModifiedSubject,
                             public Usul::Interfaces::IRenderListener,
                             public Usul::Interfaces::IRedraw
{
public:

  /// Typedefs.
  typedef Usul::Base::Object                    BaseClass;
  typedef Usul::Interfaces::IUnknown            Unknown;
  typedef Usul::Interfaces::IWindow             Window;
  typedef Window::ValidRefPtr                   WindowPtr;
  typedef std::list<WindowPtr>                  Windows;
  typedef Usul::Interfaces::IView               View;
  typedef View::ValidRefPtr                     ViewPtr;
  typedef std::list<ViewPtr>                    Views;
  typedef std::pair<std::string,std::string>    Filter;
  typedef std::vector<Filter>                   Filters;
  typedef std::map<std::string,std::string>     Options;
  typedef Windows::iterator                     WindowItr;
  typedef Windows::const_iterator               WindowConstItr;
  typedef Usul::Interfaces::IDocument           IDocument;
  typedef Usul::Interfaces::IGUIDelegate        Delegate;

  /// Smart-pointer definitions.
  USUL_DECLARE_REF_POINTERS ( Document );

  /// Usul::Interfaces::IUnknown members.
  USUL_DECLARE_IUNKNOWN_MEMBERS;

  /// Construction.
  Document ( const std::string &type );

  /// Add a window to the proper set.
  virtual void                addWindow   ( Window *window );
  virtual void                addView     ( View   *view   );

  /// The application is about to close.
  virtual void                applicationClosing ( Unknown *caller = 0x0 );

  /// Return the job to close this document. Default is null.
  virtual Usul::Jobs::Job *   closeJob();

  /// The given window is closing
  virtual void                closing     ( Window *window );

  /// Iterators to the begining of the windows
  WindowItr                   beginWindows()       { return _windows.begin(); }
  WindowConstItr              beginWindows() const { return _windows.begin(); }

  /// Set/get the whether or not this document is binary.
  bool                        binary() const;
  void                        binary ( bool b );

  /// Ask the document if the window can close.
  bool                        canClose ( Window *window, Unknown *caller = 0x0, bool checkNumWindows = true );
  bool                        canClose ( Unknown *caller = 0x0 );

  /// Return true if this document can do it.
  virtual bool                canExport ( const std::string &ext ) const = 0;
  virtual bool                canInsert ( const std::string &ext ) const = 0;
  virtual bool                canOpen   ( const std::string &ext ) const = 0;
  virtual bool                canSave   ( const std::string &ext ) const = 0;

  /// Clear any existing data.
  virtual void                clear ( Unknown *caller = 0x0 ) = 0;

  /// Close all referenced windows except one specified
  bool                        closeWindows( Unknown *caller = 0x0, const Window* skip = 0x0 );

  /// Create default GUI
  virtual void                createDefaultGUI ( Unknown *caller = 0x0 );

  // Get/Set the Delegate.  May be null.
  void                        delegate ( Delegate *delegate );
  Delegate*                   delegate ( );

  /// Assign the document a default file name
  void                        defaultFilename ();

  /// Iterators to the end
  WindowItr                   endWindows()       { return _windows.end(); }
  WindowConstItr              endWindows() const { return _windows.end(); }

  /// Set/get the file format.
  virtual IDocument::Format   fileFormat() const { return _file.format(); }
  virtual void                fileFormat ( const IDocument::Format &f ) { _file.format ( f ); }

  /// Set/get the file name.
  virtual const std::string & fileName() const { return _file.name(); }
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

  /// Prompt user for documents to export.
  void                        exportDocument ( Unknown *caller = 0x0 );

	/// Do we have this option?
  bool                        hasOption ( const std::string &key, const std::string &value ) const;

  /// Prompt user for documents to insert into this one.
  void                        insert ( Unknown *caller = 0x0 );

  /// Set/get the modified flag.
  virtual bool                modified() const;
  virtual void                modified ( bool m );

  /// Return the number of windows.
  unsigned int                numWindows() const;
  unsigned int                numViews() const;

  /// Open the file. Clears any data this document already has.
  void                        open ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 );

  /// Get the options
  Options                     options() const;

  /// Read the file and add it to existing document's data.
  virtual void                read ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 ) = 0;

  /// Returns appropriate tag name for registry.
  virtual std::string         registryTagName() const;

  /// Remove a window from the proper set.
  virtual void                removeWindow   ( Window *window );
  virtual void                removeView     ( View   *view   );

  /// Ask the views to redraw.
  virtual void                requestRedraw();
  virtual bool                allowRequestRedraw() const;
  virtual void                allowRequestRedraw ( bool );

  /// Save the document to existing file name.
  void                        save ( Unknown *caller = 0x0, Unknown *progress = 0x0, std::ostream *out = 0x0 );

  /// Always prompts for new file name.
  void                        saveAs ( Unknown *caller = 0x0, Unknown *progress = 0x0, std::ostream *out = 0x0 );

  /// Use given filename.
  void                        saveAs ( const std::string& filename, Unknown *caller = 0x0, Unknown *progress = 0x0, std::ostream *out = 0x0 );

  /// Set the option.
  void                        setOption ( const std::string &key, const std::string &value );

  /// Convenience function to set progress bar and flush events.
  void                        setProgressBar ( bool state, float fraction, Unknown *caller = 0x0 );
  void                        setProgressBar ( bool state, unsigned int numerator, unsigned int denominator, Unknown *caller = 0x0 );
  void                        setProgressBar ( bool state, std::istream &in, unsigned int fileSize, Unknown *caller = 0x0 );

  /// Convenience function to set status bar and flush events.
  void                        setStatusBar ( const std::string &text, Unknown *caller = 0x0 );

  /// Return the name of this type of document.
  virtual std::string         typeName() const;

  /// Update the GUI.
  virtual void                updateGUI();
  
  /// Update the window titles.
  void                        updateWindowTitles();

  /// Bring the windows forward
  void                        windowsForward();

  /// Write the document to given file name. Does not rename this document.
  virtual void                write ( const std::string &filename, Unknown *caller = 0x0, Unknown *progress = 0x0 ) const = 0;

protected:

  /// Do not copy.
  Document ( const Document & );
  Document &operator = ( const Document & );

  /// Use reference counting.
  virtual ~Document();

  // Build the title for given window.
  virtual std::string         _buildTitle ( Window* );

  std::string                 _getSaveAsFileName ( Unknown *caller = 0x0 );

  void                        _save ( const std::string &filename, Unknown *caller, Unknown *progress, std::ostream *out = 0x0 );

  // Overload to sort the files before reading.
  virtual bool                _sortFilesBeforeInserting() const;

  // Notify the observers.
  void                        _notifyModifiedObservers();

  /// Usul::Interfaces::IModifiedSubject
  virtual void                addModifiedObserver    ( Usul::Interfaces::IModifiedObserver* observer );
  virtual void                removeModifiedObserver ( Usul::Interfaces::IModifiedObserver* observer );

  /// Usul::Interfaces::IRenderListener
  virtual void                postRenderNotify ( Unknown *caller );
  virtual void                preRenderNotify ( Unknown *caller );

  /// Usul::Interfaces::IRedraw
  virtual void                redraw();
  virtual void                setStatsDisplay ( bool b );

private:

  typedef Usul::Interfaces::IModifiedObserver::ValidRefPtr ModifiedObserverPtr;
  typedef std::set < ModifiedObserverPtr > ModifiedObserversSet;
  typedef Usul::Threads::Object<bool> AllowRequestRedraw;
  typedef Usul::Threads::Object<ModifiedObserversSet> ModifiedObservers;

  FileInfo _file;
  Windows _windows;
  Views   _views;
  std::string _typeName;
  Delegate::RefPtr _delegate;
  Options _options;
  ModifiedObservers _modifiedObservers;
  AllowRequestRedraw _allowRequestRedraw;
};


} // namespace Documents
} // namespace Usul


#endif // _USUL_DOCUMENT_H_

