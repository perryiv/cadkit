
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
    public NewDocumentCommand(object caller)
      : base()
    {
      _caller = caller;
      _text = "&New...";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/new_document.png");
      _toolIcon = _menuIcon;
      _keys = System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N;

      // Force creation of new document window.
      CadKit.Helios.Lib.NewDocumentForm.Instance.Caller = _caller;
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      CadKit.Interfaces.IDockPanel dock = _caller as CadKit.Interfaces.IDockPanel;
      if (null != dock)
      {
        CadKit.Helios.Lib.NewDocumentForm.Instance.Show(dock.DockPanel as WeifenLuo.WinFormsUI.DockPanel);
      }
      else
      {
        CadKit.Helios.Lib.NewDocumentForm.Instance.Show();
      }
    }

    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      return CadKit.Plugins.Manager.Instance.has<CadKit.Interfaces.IDocumentNew>();
    }
  }
}
