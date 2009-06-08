
namespace CadKit.Plugins.Ossim.Commands
{
  class AddLayerCommand : CadKit.Commands.Command
  {
    private CadKit.Plugins.Ossim.Form _form = null;

    /// <summary>
    /// Constructor.
    /// </summary>
    public AddLayerCommand(object caller, CadKit.Plugins.Ossim.Form form)
      : base()
    {
      _form = form;
      _caller = caller;
      _text = "Add Layer";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/add_layer_command.gif");
      _toolIcon = _menuIcon;
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      System.Windows.Forms.OpenFileDialog dialog = new System.Windows.Forms.OpenFileDialog();
      dialog.AddExtension = true;
      dialog.CheckFileExists = true;
      dialog.CheckPathExists = true;
      dialog.DereferenceLinks = true;
      dialog.RestoreDirectory = false;
      dialog.Title = CadKit.Helios.Application.Instance.Name + " -- Add Layer";
      dialog.Multiselect = true;

      dialog.Filter = "All Files (*.*)|*.*|JPEG (*.jpg)|*.jpg|TIFF (*.tif,*.tiff)|*.tif,*.tiff";

      if (dialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
      {
        string[] names = dialog.FileNames;

        for (uint i = 0; i < names.Length; ++i)
        {
          _form.addImageLayer(names[i]);
        }
      }
    }

    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      return (null != CadKit.Documents.Manager.Instance.ActiveDocument as CadKit.Interfaces.IImageLayer);
    }
  }
}
