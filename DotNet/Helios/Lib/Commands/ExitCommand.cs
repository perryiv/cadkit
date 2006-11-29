
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  class ExitCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    delegate void CloseFormDelegate();


    /// <summary>
    /// Constructor.
    /// </summary>
    public ExitCommand(object caller)
      : base()
    {
      _caller = caller;
      _text = "E&xit";
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    protected override void _execute()
    {
      this._closeForm();
    }


    /// <summary>
    /// Close the form.
    /// </summary>
    protected void _closeForm()
    {
      System.Windows.Forms.Form form = this.Form;
      if (null != form)
      {
        if (true == form.InvokeRequired)
        {
          form.BeginInvoke(new CloseFormDelegate(this._closeForm));
        }
        else
        {
          form.Close();
        }
      }
    }


    /// <summary>
    /// Get the form.
    /// </summary>
    private System.Windows.Forms.Form Form
    {
      get { using (this.Lock.read()) { return _caller as System.Windows.Forms.Form; } }
    }
  }
}
