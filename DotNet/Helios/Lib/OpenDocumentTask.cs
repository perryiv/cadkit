
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
    CadKit.Interfaces.IRecentFileList,
    CadKit.Interfaces.IProgressBar
  {
    /// <summary>
    /// Local types.
    /// </summary>
    private delegate void CreateDefaultGuiDelegate(CadKit.Interfaces.IDocument idoc);

    /// <summary>
    /// Constructor.
    /// </summary>
    private object _mutex = new object();
    private object _caller = null;
    private string _file = null;
    private CadKit.Interfaces.IProgressBar _progress = null;

    /// <summary>
    /// Constructor.
    /// </summary>
    public OpenDocumentTask(string file, object caller)
      : base()
    {
      // Set members.
      _caller = caller;
      _file = file;

      // Check state.
      if (null == _file)
        throw new System.ArgumentNullException("Error 4095426091: Given file name is null");
      if (0 == _file.Length)
        throw new System.ArgumentException("Error 8065017300: Length of given file name is zero");

      // Make a job.
      CadKit.Threads.Jobs.Job job = new CadKit.Threads.Jobs.Job();

      // Set delegates.
      job.Start += this._start;
      job.Finish += this._finish;
      job.Progress.Minimum = 0;
      job.Progress.Maximum = this._fileSize(file);

      // Progress interface.
      _progress = job.Progress as CadKit.Interfaces.IProgressBar;

      // Queue the job.
      CadKit.Threads.Jobs.Manager.Instance.add(job);
    }

    /// <summary>
    /// Return the file size.
    /// </summary>
    private int _fileSize(string file)
    {
      System.IO.FileInfo info = new System.IO.FileInfo(file);
      return (int)info.Length;
    }

    /// <summary>
    /// Called when the thread starts.
    /// </summary>
    private void _start(CadKit.Threads.Jobs.Job job)
    {
      lock (this.Mutex)
      {
        // Should be true.
        System.Diagnostics.Debug.Assert(false == CadKit.Threads.Tools.MainThread.Instance.IsMainThread);

        // If it exists then bring it forward.
        CadKit.Interfaces.IDocument idoc = CadKit.Documents.Manager.Instance.findDocument(_file);
        if (null != idoc)
        {
          CadKit.Documents.Manager.Instance.windowsForward(idoc, _caller);
          return;
        }

        // Feedback.
        System.Console.WriteLine(System.String.Format("Opening file: {0}", _file));

        // Open the document.
        idoc = CadKit.Documents.Manager.Instance.open(_file, null, this);

        // Give the document a command history. Assigning this avoids a dependency.
        CadKit.Documents.Document doc = idoc as CadKit.Documents.Document;
        if (null != doc)
        {
          doc.CommandHistory = new CadKit.Commands.History();
          doc.CommandHistory.add(this);
        }

        // Set the delegate.
        CadKit.Documents.Manager.Instance.setGuiDelegate(idoc, _caller);

        // Create the default user-interface.
        this._createDefaultGui(idoc);
      }
    }

    /// <summary>
    /// Called when the thread finishes.
    /// </summary>
    private void _finish(CadKit.Threads.Jobs.Job job)
    {
      lock (this.Mutex)
      {
      }
    }

    /// <summary>
    /// Create the default user-interface.
    /// </summary>
    private void _createDefaultGui(CadKit.Interfaces.IDocument idoc)
    {
      System.Windows.Forms.Form form = CadKit.Helios.Application.Instance.MainForm;
      if (true == form.InvokeRequired)
      {
        form.BeginInvoke(new CreateDefaultGuiDelegate(this._createDefaultGui), new object[] { idoc });
      }
      else
      {
        CadKit.Interfaces.IGuiCreate gui = idoc as CadKit.Interfaces.IGuiCreate;
        if (null != gui)
        {
          gui.create(_caller);
        }
      }
    }

    /// <summary>
    /// Get the mutex.
    /// </summary>
    private object Mutex
    {
      get { return _mutex; }
    }

    /// <summary>
    /// Add the name.
    /// </summary>
    void CadKit.Interfaces.IRecentFileList.add(string name)
    {
      lock (this.Mutex)
      {
        CadKit.Interfaces.IRecentFileList recent = _caller as CadKit.Interfaces.IRecentFileList;
        if (null != recent)
        {
          recent.add(name);
        }
      }
    }

    /// <summary>
    /// Remove the name.
    /// </summary>
    void CadKit.Interfaces.IRecentFileList.remove(string name)
    {
      lock (this.Mutex)
      {
        CadKit.Interfaces.IRecentFileList recent = _caller as CadKit.Interfaces.IRecentFileList;
        if (null != recent)
        {
          recent.remove(name);
        }
      }
    }

    /// <summary>
    /// Clear the list.
    /// </summary>
    void CadKit.Interfaces.IRecentFileList.clear()
    {
      lock (this.Mutex)
      {
        CadKit.Interfaces.IRecentFileList recent = _caller as CadKit.Interfaces.IRecentFileList;
        if (null != recent)
        {
          recent.clear();
        }
      }
    }

    /// <summary>
    /// Set the text.
    /// </summary>
    string CadKit.Interfaces.IProgressBar.Text
    {
      get
      {
        lock (this.Mutex)
        {
          return (null != _progress) ? _progress.Text : "";
        }
      }
      set
      {
        lock (this.Mutex)
        {
          if (null != _progress)
          {
            _progress.Text = value;
          }
        }
      }
    }

    /// <summary>
    /// Get the range.
    /// </summary>
    int CadKit.Interfaces.IProgressBar.Range
    {
      get
      {
        lock (this.Mutex)
        {
          return (null != _progress) ? _progress.Range : 100;
        }
      }
    }

    /// <summary>
    /// Set the value.
    /// </summary>
    int CadKit.Interfaces.IProgressBar.Value
    {
      get
      {
        lock (this.Mutex)
        {
          return (null != _progress) ? _progress.Value : 0;
        }
      }
      set
      {
        lock (this.Mutex)
        {
          if (null != _progress)
          {
            _progress.Value = value;
          }
        }
      }
    }
  }
}
