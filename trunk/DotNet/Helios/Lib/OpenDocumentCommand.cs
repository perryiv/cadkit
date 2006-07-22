
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
      _menuIcon = CadKit.Images.Image.load(CadKit.Helios.Application.Instance.IconDir + "/open_document.png");
      _keys = System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O;
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    protected override void _execute()
    {
      lock (_mutex)
      {
        string[] names = this._askForFileNames();

        // Open the files.
        foreach (string name in names)
        {
          try
          {
            // Open the document.
            CadKit.Interfaces.IDocument idoc = CadKit.Documents.Manager.Instance.open(name, null, _caller);

            // This is now the active document.
            CadKit.Documents.Manager.Instance.Active = idoc;

            // Give the document a command history. Assigning this avoids a dependency.
            CadKit.Documents.Document doc = idoc as CadKit.Documents.Document;
            if (null != doc)
            {
              doc.CommandHistory = new CadKit.Commands.History();
              doc.CommandHistory.add(this);
            }

            // Set the delegate.
            CadKit.Documents.Manager.Instance.setGuiDelegate(idoc, _caller);

            // Create the default user-interface.
            CadKit.Interfaces.IGuiCreate gui = idoc as CadKit.Interfaces.IGuiCreate;
            if ( null != gui )
            {
              gui.create(_caller);
            }
          }
          catch (System.Exception e)
          {
            System.Console.WriteLine("Error 8905677560: {0}", e.Message);
          }
        }
      }
    }

    /// <summary>
    /// Ask for the file names.
    /// </summary>
    private string[] _askForFileNames()
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
