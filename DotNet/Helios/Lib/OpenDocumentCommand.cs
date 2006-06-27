
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

        // Set filter string.
        string persistentName = this.GetType().ToString() + ".OpenFileDialog";
        string filterIndexKey = "filter_index";
        this._setFilterString(persistentName, filterIndexKey, dialog);

        // Get initial directory.
        string initialDirectoryKey = "initial_directory";
        dialog.InitialDirectory = CadKit.Persistence.Registry.Instance.getString(persistentName, initialDirectoryKey, CadKit.Helios.Application.Instance.directory());

        System.Windows.Forms.DialogResult result = dialog.ShowDialog();
        if (System.Windows.Forms.DialogResult.OK == result)
        {
          string[] names = dialog.FileNames;

          // Save final directory and filter index.
          if (null != names && names.Length > 0)
          {
            System.IO.FileInfo info = new System.IO.FileInfo(names[0]);
            CadKit.Persistence.Registry.Instance.setString(persistentName, initialDirectoryKey, info.DirectoryName);
            CadKit.Persistence.Registry.Instance.setInt(persistentName, filterIndexKey, dialog.FilterIndex);
          }

          foreach (string name in names)
          {
            //CadKit.Documents.Manager.Instance
          }
        }
      }
    }

    /// <summary>
    /// Return the filter string.
    /// </summary>
    private void _setFilterString(string persistentName, string key, System.Windows.Forms.OpenFileDialog dialog)
    {
      string finalFilter = "";
      CadKit.Interfaces.IFiltersOpen[] filtersOpen = CadKit.Plugins.Manager.Instance.getAll<CadKit.Interfaces.IFiltersOpen>();

      foreach (CadKit.Interfaces.IFiltersOpen filterOpen in filtersOpen)
      {
        CadKit.Interfaces.Filters filters = filterOpen.Filters;
        foreach (CadKit.Interfaces.Filter filter in filters)
        {
          finalFilter += (filter.Text + '|' + filter.Extensions + '|');
        }
      }

      finalFilter = finalFilter.TrimEnd(new char[] { '|' });

      try
      {
        dialog.Filter = finalFilter;
      }
      catch (System.ArgumentException)
      {
        // No telling what filter strings the plugins will make.
      }

      // Get filter index.
      dialog.FilterIndex = CadKit.Persistence.Registry.Instance.getInt(persistentName, key, 1);
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
