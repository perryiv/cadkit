
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Delegates
{
  abstract public class Delegate :
    CadKit.Referenced.Base,
    CadKit.Interfaces.IWindowsForward
  {
    /// <summary>
    /// Local types.
    /// </summary>
    private delegate void VoidReturnZeroArgumentsDelegate();


    /// <summary>
    /// Data members.
    /// </summary>
    private CadKit.Interfaces.IDocument _document = null;
    private object _mutex = new object();


    /// <summary>
    /// Constructor
    /// </summary>
    protected Delegate()
    {
    }


    /// <summary>
    /// Set the document.
    /// </summary>
    public CadKit.Interfaces.IDocument Document
    {
      get { lock (this.Mutex) { return _document; } }
      set { lock (this.Mutex) { _document = value; } }
    }


    /// <summary>
    /// Bring the windows forward.
    /// </summary>
    void CadKit.Interfaces.IWindowsForward.windowsForward(object caller)
    {
      CadKit.Interfaces.IDocument document = this.Document;
      CadKit.Interfaces.IDocumentView[] views = this.Document.views();
      foreach (CadKit.Interfaces.IDocumentView view in views)
      {
        this._activateView(view);
      }
    }


    /// <summary>
    /// Bring the view forward.
    /// </summary>
    private void _activateView(CadKit.Interfaces.IDocumentView view)
    {
      System.Windows.Forms.Form form = view as System.Windows.Forms.Form;
      if (null != form)
      {
        if (true == form.InvokeRequired)
        {
          form.BeginInvoke(new VoidReturnZeroArgumentsDelegate(form.Activate));
        }
        else
        {
          form.Activate();
        }
      }
    }


    /// <summary>
    /// Get the mutex.
    /// </summary>
    public object Mutex
    {
      get { return _mutex; }
    }
  }
}
