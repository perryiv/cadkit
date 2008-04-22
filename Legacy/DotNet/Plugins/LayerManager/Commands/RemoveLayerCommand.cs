
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.LayerManager.Commands
{
  class RemoveLayerCommand : CadKit.Commands.Command
  {
    private CadKit.Plugins.LayerManager.Layers _form = null;

    /// <summary>
    /// Constructor.
    /// </summary>
    public RemoveLayerCommand(object caller, CadKit.Plugins.LayerManager.Layers form)
      : base()
    {
      _form = form;
      _caller = caller;
      _text = "Remove Layer";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/remove_layer_command.gif");
      _toolIcon = _menuIcon;
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      if (null != _form)
      {
        _form.removeSelectedLayers();
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
