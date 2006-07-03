
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Windows.OutputWindow
{
  public class Form : CadKit.Persistence.Form
  {
    /// <summary>
    /// Data members.
    /// </summary>
    System.Windows.Forms.RichTextBox _text = null;

    /// <summary>
    /// Constructor
    /// </summary>
    public Form()
    {
      try
      {
        _text = new System.Windows.Forms.RichTextBox();
        _text.Parent = this;
        this.Controls.Add(_text);
        _text.Dock = System.Windows.Forms.DockStyle.Fill;
        _text.Multiline = true;
        _text.Text = "Perry\r\n";
        CadKit.Tools.RedirectOutput.Instance.Notify += this._notify;
        this.FormClosing += this._formClosing;
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1773819642: {0}", e.Message);
        _text = null;
        this.Controls.Clear();
      }
    }

    /// <summary>
    /// Called when the forms is about to close.
    /// </summary>
    private void _formClosing(object sender, System.Windows.Forms.FormClosingEventArgs e)
    {
      lock (_mutex)
      {
        CadKit.Tools.RedirectOutput.Instance.Notify -= this._notify;
      }
    }

    /// <summary>
    /// Callback for changed output text.
    /// </summary>
    private void _notify(string text)
    {
      // This function gets called from worker thread. Do not lock _mutex.
      this._updateTextControl(text);
    }

    /// <summary>
    /// Delegate for BeginInvoke call.
    /// </summary>
    private delegate void UpdateTextDelegate(string text);

    /// <summary>
    /// Change the text control.
    /// </summary>
    private void _updateTextControl(string text)
    {
      // This function can get called from multiple threads and can call 
      // itself from different threads (with BeginInvoke), so we cannot 
      // lock the _mutex. See:
      // http://msdn.microsoft.com/library/default.asp?url=/library/en-us/dnforms/html/winforms06112002.asp

      // Make sure we set text from the proper thread.
      if (true == _text.InvokeRequired)
      {
        // A breakpoint here will cause the debugger to briefly hang because 
        // this is not the gui thread. Also, BeginInvoke() is thread-safe, 
        // so do not lock the _mutex.
        _text.BeginInvoke(new UpdateTextDelegate(_updateTextControl), new object[] { text });
      }
      else
      {
        _text.Text += text;
        _text.Invalidate(true);
      }
    }
  }
}
