
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class NewDocumentCommand : CadKit.Commands.Command
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public NewDocumentCommand ( object caller )
    {
      _caller = caller;
      _text = "&New...";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.instance().directory() + "/icons/new_document.png");
      _keys = System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N;
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
      return CadKit.Plugins.Manager.instance().has<CadKit.Interfaces.IDocumentNew>();
    }
  }
}
