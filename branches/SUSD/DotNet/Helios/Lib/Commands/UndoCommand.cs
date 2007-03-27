
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
      : base()
    {
      _caller = caller;
      _text = "&Undo";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/undo_command.png");
      _toolIcon = _menuIcon;
      _keys = System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Z;
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      CadKit.Interfaces.IDocument idoc = CadKit.Documents.Manager.Instance.ActiveDocument;
      CadKit.Documents.Document doc = idoc as CadKit.Documents.Document;
      CadKit.Interfaces.ICommandHistory commands = (null == doc) ? null : doc.CommandHistory;
      if (null != commands && true == commands.CanUndo)
      {
        commands.undo();
      }
    }

    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      CadKit.Interfaces.IDocument idoc = CadKit.Documents.Manager.Instance.ActiveDocument;
      CadKit.Documents.Document doc = idoc as CadKit.Documents.Document;
      CadKit.Interfaces.ICommandHistory commands = (null == doc) ? null : doc.CommandHistory;
      return (null == commands) ? false : commands.CanUndo;
    }
  }
}
