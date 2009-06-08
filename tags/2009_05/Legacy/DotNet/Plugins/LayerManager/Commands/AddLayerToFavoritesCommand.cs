
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Adam Kubach
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.LayerManager.Commands
{
  class AddLayerToFavoritesCommand : CadKit.Commands.Command
  {
    private CadKit.Plugins.LayerManager.Layers _form = null;

    /// <summary>
    /// Constructor.
    /// </summary>
    public AddLayerToFavoritesCommand(object caller, CadKit.Plugins.LayerManager.Layers form)
      : base()
    {
      _form = form;
      _caller = caller;
      _text = "Add Layer To Favorites";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/add_layer_to_favorites_command.gif");
      _toolIcon = _menuIcon;
    }


    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      if (null != _form)
      {
        _form.saveSelectedLayers();
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
