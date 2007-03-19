
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class SaveDocumentCommand : CadKit.Commands.Command
  {
    public SaveDocumentCommand(object caller) : base()
    {
      _caller = caller;
      _text = "&Save...";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/save_document.gif");
      _toolIcon = _menuIcon;
      _keys = System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S;
    }

    public override void execute()
    {
    }

    protected override bool _shouldBeEnabled()
    {
      return CadKit.Documents.Manager.Instance.ActiveDocument is CadKit.Interfaces.IFileSave;
    }
  }
}
