
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class OpenDocumentCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public OpenDocumentCommand ( object caller )
    {
      _caller = caller;
      _text = "&Open...";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.instance().directory() + "/icons/open_document.png");
      _keys = System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O;
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
    }

    /// <summary>
    /// Determine of the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      return CadKit.Plugins.Manager.instance().has<CadKit.Interfaces.IDocumentOpen>();
    }
  }
}
