
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios.Commands
{
  public class OpenDocumentCommand : 
    CadKit.Commands.Command
  {
    /// <summary>
    /// Constructor.
    /// </summary>
    public OpenDocumentCommand ( object caller )
    {
      _caller = caller;
      _text = "&Open...";
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/open_document.png");
      _keys = System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O;
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    protected override void _execute()
    {
      lock (this.Mutex)
      {
        string[] names = this._askForFileNames();

        // Open the files.
        foreach (string name in names)
        {
          try
          {
            this._open(name);
          }
          catch (System.Exception e)
          {
            System.Console.WriteLine("Error 8905677560: {0}", e.Message);
          }
        }
      }
    }

    /// <summary>
    /// Open the file.
    /// </summary>
    protected void _open(string name)
    {
      lock (this.Mutex)
      {
        // Should be true.
        System.Diagnostics.Debug.Assert(false == CadKit.Helios.Application.Instance.MainForm.InvokeRequired);

        // This will start a separate thread and immediately return.
        OpenDocumentTask task = new OpenDocumentTask(name, _caller);
      }
    }

    /// <summary>
    /// Ask for the file names.
    /// </summary>
    private string[] _askForFileNames()
    {
      lock (this.Mutex)
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
        dialog.InitialDirectory = CadKit.Persistence.Registry.Instance.getString(persistentName, initialDirectoryKey, CadKit.Helios.Application.Instance.Directory);

        // Show dialog and get names.
        dialog.ShowDialog();
        string[] names = dialog.FileNames;

        // Save final directory and filter index.
        if (null != names && names.Length > 0)
        {
          System.IO.FileInfo info = new System.IO.FileInfo(names[0]);
          CadKit.Persistence.Registry.Instance.setString(persistentName, initialDirectoryKey, info.DirectoryName);
          CadKit.Persistence.Registry.Instance.setInt(persistentName, filterIndexKey, dialog.FilterIndex);
        }

        // Return names.
        return names;
      }
    }

    /// <summary>
    /// Return the filter string.
    /// </summary>
    private void _setFilterString(string persistentName, string key, System.Windows.Forms.OpenFileDialog dialog)
    {
      lock (this.Mutex)
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
    }

    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected override bool _shouldBeEnabled()
    {
      lock (this.Mutex)
      {
        return CadKit.Plugins.Manager.Instance.has<CadKit.Interfaces.IDocumentOpen>();
      }
    }
  }
}
