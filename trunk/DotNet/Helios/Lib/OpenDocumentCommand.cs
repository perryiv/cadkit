
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
    /// Local types.
    /// </summary>
    //class Filters 

    /// <summary>
    /// Constructor.
    /// </summary>
    public OpenDocumentCommand ( object caller )
    {
      _caller = caller;
      _text = "&Open...";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.directory() + "/icons/open_document.png");
      _keys = System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O;
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    public override void execute()
    {
      lock (_mutex)
      {
        System.Windows.Forms.OpenFileDialog dialog = new System.Windows.Forms.OpenFileDialog();
        dialog.AddExtension = true;
        dialog.CheckFileExists = true;
        dialog.CheckPathExists = true;
        dialog.DereferenceLinks = true;
        dialog.RestoreDirectory = false;
        dialog.Title = CadKit.Helios.Application.Instance.Name + " -- Open Document";
        dialog.Multiselect = true;

        // Get initial directory.
        string persistentName = this.GetType().ToString() + ".OpenFileDialog";
        string key = "initial_directory";
        dialog.InitialDirectory = CadKit.Persistence.Registry.Instance.getString(persistentName, key, CadKit.Helios.Application.Instance.directory());

        System.Windows.Forms.DialogResult result = dialog.ShowDialog();
        if (System.Windows.Forms.DialogResult.OK == result)
        {
          string[] names = dialog.FileNames;

          // Save final directory.
          if (null != names && names.Length > 0)
          {
            System.IO.FileInfo info = new System.IO.FileInfo(names[0]);
            CadKit.Persistence.Registry.Instance.setString(persistentName, key, info.DirectoryName);
          }

          foreach (string name in names)
          {
            //CadKit.Documents.Manager.Instance
          }
        }
      }
    }

    /// <summary>
    /// Determine of the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      lock (_mutex)
      {
        return CadKit.Plugins.Manager.Instance.has<CadKit.Interfaces.IDocumentOpen>();
      }
    }
  }
}
