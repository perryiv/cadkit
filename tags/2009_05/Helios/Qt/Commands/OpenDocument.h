
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Perry L Miller IV
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Command to open a document.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef _CADKIT_HELIOS_QT_COMMANDS_OPEN_DOCUMENT_COMMAND_H_
#define _CADKIT_HELIOS_QT_COMMANDS_OPEN_DOCUMENT_COMMAND_H_

#include "Helios/Qt/Commands/Command.h"

#include "Usul/Interfaces/ILoadFileDialog.h"
#include "Usul/Jobs/Job.h"
#include "Usul/Documents/Document.h"

#include <vector>

namespace Usul { namespace Threads { class Thread; } }


namespace CadKit {
namespace Helios {
namespace Commands {


class HELIOS_QT_COMMANDS_EXPORT OpenDocument : public CadKit::Helios::Commands::Command
{
public:

  // Typedefs.
  typedef CadKit::Helios::Commands::Command     BaseClass;
  typedef Usul::Interfaces::IUnknown            IUnknown;
  typedef Usul::Interfaces::ILoadFileDialog     ILoadFileDialog;
  typedef ILoadFileDialog::FileNames            FileNames;
  typedef ILoadFileDialog::Filters              Filters;
  typedef Usul::Documents::Document             Document;

  // Smart-pointer definitions.
  USUL_DECLARE_COMMAND ( OpenDocument );

  // Constructor.
  OpenDocument ( IUnknown *caller );

  /// Get/Set the filename to open.
  void                filename ( const std::string& filename );
  const std::string&  filename () const;

protected:

  // Use reference counting.
  virtual ~OpenDocument();
  OpenDocument ( const OpenDocument & );
  OpenDocument &operator = ( const OpenDocument & );

  // Internal job class.
  class Job : public Usul::Jobs::Job
  {
  public:
    typedef Usul::Jobs::Job BaseClass;
    Job ( Document::RefPtr doc, const std::string &name, IUnknown::RefPtr caller );
  protected:
    virtual ~Job();
    virtual void _started();
  private:
    Document::RefPtr _document;
    std::string _name;
    IUnknown::RefPtr _caller;
  };

  FileNames                 _askForFileNames ( const std::string &title );

  virtual void              _execute();

  void                      _startJob ( const std::string& filename );

private:

  std::string _filename;
};


} // namespace Commands
} // namespace Helios
} // namespace CadKit


#endif //_CADKIT_HELIOS_QT_COMMANDS_OPEN_DOCUMENT_COMMAND_H_
