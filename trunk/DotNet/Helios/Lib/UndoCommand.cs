
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class UndoCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public UndoCommand(object caller)
    {
      _caller = caller;
      _text = "&Undo";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.directory() + "/icons/undo_command.png");
      _keys = System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Z;
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      lock (_mutex)
      {
        CadKit.Commands.History.Instance.undo();
      }
    }

    /// <summary>
    /// Determine of the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      lock (_mutex)
      {
        return CadKit.Commands.History.Instance.CanUndo;
      }
    }
  }
}
