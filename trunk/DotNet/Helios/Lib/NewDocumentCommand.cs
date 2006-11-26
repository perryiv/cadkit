
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
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/new_document.png");
      _keys = System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N;
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    protected override void _execute()
    {
      lock (this.Mutex)
      {
      }
    }

    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      lock (this.Mutex)
      {
        return CadKit.Plugins.Manager.Instance.has<CadKit.Interfaces.IDocumentNew>();
      }
    }
  }
}
