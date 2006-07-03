
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class RedoCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public RedoCommand(object caller)
    {
      _caller = caller;
      _text = "&Redo";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.directory() + "/icons/redo_command.png");
      _keys = System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Y;
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      lock (_mutex)
      {
        CadKit.Commands.History.Instance.redo();
      }
    }

    /// <summary>
    /// Determine of the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      lock (_mutex)
      {
        return CadKit.Commands.History.Instance.CanRedo;
      }
    }
  }
}
