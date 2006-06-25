
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
    public ExitCommand ( object caller )
    {
      _caller = caller;
      _text = "E&xit";
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      System.Windows.Forms.Form form = _caller as System.Windows.Forms.Form;
      if (null != form)
        form.Close();
    }
  }
}
