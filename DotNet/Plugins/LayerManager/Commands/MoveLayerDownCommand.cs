
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.LayerManager.Commands
{
  class MoveLayerDownCommand : CadKit.Commands.Command
  {
    private CadKit.Plugins.LayerManager.Layers _form = null;

    /// <summary>
    /// Constructor.
    /// </summary>
    public MoveLayerDownCommand(object caller, CadKit.Plugins.LayerManager.Layers form)
      : base()
    {
      _form = form;
      _caller = caller;
      _text = "Move Layer Down";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/move_layer_down_command.gif");
      _toolIcon = _menuIcon;
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      if (null != _form)
      {
      }
    }


    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      return (null != CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.ILayerList);
    }
  }
}
