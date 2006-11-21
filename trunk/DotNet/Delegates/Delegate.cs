
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
    CadKit.Interfaces.IWindowsForward
  {
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
      lock (this.Mutex)
      {
      }
    }


    /// <summary>
    /// Get the mutex.
    /// </summary>
    public object Mutex
    {
      get { return _mutex; }
    }


    /// <summary>
    /// Called when the form closes.
    /// </summary>
    /// HERE
    /// Was trying to use delegates to hook up a document-view architecture. 
    /// Not there yet; I need more delegate types and need to epose them with properties.
#if no
    private void _viewClosed(object sender, System.Windows.Forms.FormClosedEventArgs args)
    {
      try
      {
        lock (this.Mutex)
        {
          CadKit.Viewer.Viewer view = sender as CadKit.Viewer.Viewer;
          if (null != view)
          {
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1028145785: {0}", e.Message);
      }
    }
#endif
  }
}
