
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios
{
  public class OpenDocumentTask :
    ReadDocumentTask,
    CadKit.Interfaces.IRecentFileList
  {
    /// <summary>
    /// Local types.
    /// </summary>
    private delegate bool CreateDefaultGuiDelegate(CadKit.Interfaces.IDocument idoc);


    /// <summary>
    /// Local data members.
    /// </summary>
    private CadKit.Interfaces.ICommand _command;


    /// <summary>
    /// Constructor.
    /// </summary>
    public OpenDocumentTask(string file, object caller, CadKit.Interfaces.ICommand command)
      : base(file, caller)
    {
      // Set members.
      _command = command;
    }


    /// <summary>
    /// Called when the thread starts.
    /// </summary>
    protected override void _startJob(CadKit.Threads.Jobs.Job job)
    {
      // Should be true.
      System.Diagnostics.Debug.Assert(false == CadKit.Threads.Tools.MainThread.Instance.IsMainThread);

      // If it exists then bring it forward.
      CadKit.Interfaces.IDocument idoc = CadKit.Documents.Manager.Instance.findDocument(this.File);
      if (null != idoc)
      {
        CadKit.Documents.Manager.Instance.windowsForward(idoc, this.Caller);
        return;
      }

      // Feedback.
      System.Console.WriteLine(System.String.Format("Opening file: {0}", this.File));

      // Open the document.
      idoc = CadKit.Documents.Manager.Instance.open(this.File, null, this);

      // Give the document a command history. Assigning this avoids a dependency.
      CadKit.Documents.Document doc = idoc as CadKit.Documents.Document;
      if (null != doc)
      {
        doc.CommandHistory = new CadKit.Commands.History();
        doc.CommandHistory.add(this.Command);
      }

      // Set the delegate.
      CadKit.Documents.Manager.Instance.setGuiDelegate(idoc, this.Caller);

      // Create the default user-interface.
      if (false == this._createDefaultGui(idoc))
      {
        idoc.close();
        CadKit.Documents.Manager.Instance.remove(idoc);
      }
    }


    /// <summary>
    /// Create the default user-interface.
    /// </summary>
    private bool _createDefaultGui(CadKit.Interfaces.IDocument idoc)
    {
      bool result = false;
      try
      {
        System.Windows.Forms.Form form = CadKit.Helios.Application.Instance.MainForm;
        if (true == form.InvokeRequired)
        {
          CreateDefaultGuiDelegate function = new CreateDefaultGuiDelegate(this._createDefaultGui);
          result = (bool)(form.Invoke(function, new object[] { idoc }));
        }
        else
        {
          CadKit.Interfaces.IGuiCreate gui = idoc as CadKit.Interfaces.IGuiCreate;
          if (null != gui)
          {
            gui.create(this.Caller);
            result = true;
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2697332471: Failed to create default user interface: {0}", e.Message);
      }
      return result;
    }


    /// <summary>
    /// Add the name.
    /// </summary>
    void CadKit.Interfaces.IRecentFileList.add(string name)
    {
      CadKit.Interfaces.IRecentFileList recent = this.Caller as CadKit.Interfaces.IRecentFileList;
      if (null != recent)
      {
        recent.add(name);
      }
    }


    /// <summary>
    /// Remove the name.
    /// </summary>
    void CadKit.Interfaces.IRecentFileList.remove(string name)
    {
      CadKit.Interfaces.IRecentFileList recent = this.Caller as CadKit.Interfaces.IRecentFileList;
      if (null != recent)
      {
        recent.remove(name);
      }
    }


    /// <summary>
    /// Clear the list.
    /// </summary>
    void CadKit.Interfaces.IRecentFileList.clear()
    {
      CadKit.Interfaces.IRecentFileList recent = this.Caller as CadKit.Interfaces.IRecentFileList;
      if (null != recent)
      {
        recent.clear();
      }
    }


    /// <summary>
    /// Get the command.
    /// </summary>
    protected CadKit.Interfaces.ICommand Command
    {
      get { using (this.Lock.read()) { return _command; } }
    }
  }
}
