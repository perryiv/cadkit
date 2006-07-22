
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
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/undo_command.png");
      _keys = System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Z;
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    protected override void _execute()
    {
      lock (_mutex)
      {
        CadKit.Interfaces.IDocument idoc = CadKit.Documents.Manager.Instance.Active;
        CadKit.Documents.Document doc = idoc as CadKit.Documents.Document;
        CadKit.Interfaces.ICommandHistory commands = (null == doc) ? null : doc.CommandHistory;
        if (null != commands && true == commands.CanUndo)
        {
          commands.undo();
        }
      }
    }

    /// <summary>
    /// Determine of the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      lock (_mutex)
      {
        CadKit.Interfaces.IDocument idoc = CadKit.Documents.Manager.Instance.Active;
        CadKit.Documents.Document doc = idoc as CadKit.Documents.Document;
        CadKit.Interfaces.ICommandHistory commands = (null == doc) ? null : doc.CommandHistory;
        return (null == commands) ? false : commands.CanUndo;
      }
    }
  }
}
