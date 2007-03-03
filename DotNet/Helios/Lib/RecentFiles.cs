
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios
{
  public class RecentFiles : 
    CadKit.Referenced.Base,
    CadKit.Interfaces.IRecentFileList
  {
    /// <summary>
    /// Local types.
    /// </summary>
    private class Names : System.Collections.Generic.Queue<string> { }


    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();
    private Names _names = new Names();
    private uint _maxCount = 10;
    private System.Windows.Forms.ToolStripMenuItem _menu = new System.Windows.Forms.ToolStripMenuItem();
    private string _section = "RecentFiles";
    private string _entriesKey = "Entries";
    private string _maxCountKey = "MaxCount";
    private char _delimeter = ';';


    /// <summary>
    /// Constructor.
    /// </summary>
    public RecentFiles()
    {
      // Set the max count.
      this.MaxCount = CadKit.Persistence.Registry.Instance.getUint(_section, _maxCountKey, this.MaxCount);

      // Add names from the registry.
      string[] names = this._getNamesFromRegistry();
      this._add(names);

      // Set menu properties and build it.
      _menu.Text = "Recent &Files";
      _menu.DropDownOpening += this._menuDropDownOpening;
      this._buildMenu();
    }


    /// <summary>
    /// Descructor.
    /// </summary>
    ~RecentFiles()
    {
      // Calling store() in here is too late because Registry's destructor 
      // has already been called. (Isn't non-deterministic destruction 
      // cool?) Keeping this here as a reminder.
    }


    /// <summary>
    /// Clean this instance.
    /// </summary>
    protected override void _cleanup()
    {
      try
      {
        using (this.Lock.write())
        {
          _mutex = null;
          _names = null;
          _menu = null;
          _section = null;
          _entriesKey = null;
          _maxCountKey = null;
        }
        base._cleanup();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 4010357980: {0}", e.Message);
      }
    }


    /// <summary>
    /// Return names from the registry.
    /// </summary>
    private string[] _getNamesFromRegistry()
    {
      System.Collections.Generic.List<string> answer = new System.Collections.Generic.List<string>();
      try
      {
        lock (_mutex)
        {
          // Get all the files as a single string.
          string temp = CadKit.Persistence.Registry.Instance.getString(_section, _entriesKey, "");
          if (temp.Length > 0)
          {
            string[] names = temp.Split(new char[] { _delimeter });
            foreach (string name in names)
            {
              // Add only if it exists.
              if (true == System.IO.File.Exists(name))
              {
                answer.Add(name);
              }
            }
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2600544858: {0}", e.Message);
      }
      return answer.ToArray();
    }


    /// <summary>
    /// Called when the menu is being shown.
    /// </summary>
    private void _menuDropDownOpening(object sender, System.EventArgs args)
    {
      lock (_mutex) { this._buildMenu(); }
    }


    /// <summary>
    /// Build the menu.
    /// </summary>
    private void _buildMenu()
    {
      try
      {
        lock (_mutex)
        {
          _menu.DropDownItems.Clear();
          uint count = 1;
          foreach (string name in _names)
          {
            CadKit.Helios.Commands.RecentDocumentCommand recent = new CadKit.Helios.Commands.RecentDocumentCommand(name, count++, CadKit.Helios.Application.Instance.MainForm);
            _menu.DropDownItems.Add(recent.MenuButton);
          }

          CadKit.Tools.Menu.appendSeparatorIfNeeded(_menu);

          System.Windows.Forms.ToolStripMenuItem clearButton = new System.Windows.Forms.ToolStripMenuItem();
          clearButton.Text = "&Clear";
          clearButton.Click += this._clearButtonClicked;
          _menu.DropDownItems.Add(clearButton);
          clearButton.Enabled = (_names.Count > 0);
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2572957450: {0}", e.Message);
      }
    }


    /// <summary>
    /// Called when the clear button is clicked.
    /// </summary>
    private void _clearButtonClicked(object sender, System.EventArgs args)
    {
      try
      {
        lock (_mutex)
        {
          this.clear();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2392966331: {0}", e.Message);
      }
    }


    /// <summary>
    /// Add all the name.
    /// </summary>
    private void _add(string[] names)
    {
      lock (_mutex)
      {
        foreach (string name in names)
        {
          this.add(name);
        }
      }
    }


    /// <summary>
    /// Add the name.
    /// </summary>
    void CadKit.Interfaces.IRecentFileList.add(string name)
    {
      lock (_mutex)
      {
        this.add(name);
      }
    }


    /// <summary>
    /// Add the name.
    /// </summary>
    public void add(string name)
    {
      lock (_mutex)
      {
        if (null == name || 0 == name.Length)
          return;

        // Add only if it exists.
        if (false == System.IO.File.Exists(name))
          return;

        // Add only if it isn't in the queue already.
        if (true == _names.Contains(name))
          return;

        // Add the name.
        _names.Enqueue(name);

        // Trim extra.
        this._trim();

        // Store in registry.
        this.store();
      }
    }


    /// <summary>
    /// Trim extra members.
    /// </summary>
    private void _trim()
    {
      lock (_mutex)
      {
        while (_names.Count > this.MaxCount)
        {
          _names.Dequeue();
        }
      }
    }


    /// <summary>
    /// Remove the name.
    /// </summary>
    void CadKit.Interfaces.IRecentFileList.remove(string name)
    {
      lock (_mutex) { this.remove(name); }
    }


    /// <summary>
    /// Remove the name.
    /// </summary>
    public void remove(string file)
    {
      lock (_mutex)
      {
        // Make a new queue.
        Names names = new Names();

        // Add all current names except given one.
        foreach (string entry in _names)
        {
          if (entry != file)
          {
            names.Enqueue(entry);
          }
        }

        // Set member.
        _names = names;

        // Store in registry.
        this.store();
      }
    }


    /// <summary>
    /// Clear the list.
    /// </summary>
    void CadKit.Interfaces.IRecentFileList.clear()
    {
      lock (_mutex) { this.clear(); }
    }


    /// <summary>
    /// Clear the list.
    /// </summary>
    public void clear()
    {
      lock (_mutex)
      {
        // Clear the names.
        _names.Clear();

        // Store in registry.
        this.store();
      }
    }


    /// <summary>
    /// Get/set the maximum number of entries.
    /// </summary>
    public uint MaxCount
    {
      get { lock (_mutex) { return _maxCount; } }
      set { lock (_mutex) { _maxCount = value; } }
    }


    /// <summary>
    /// Get the menu.
    /// </summary>
    public System.Windows.Forms.ToolStripMenuItem Menu
    {
      get { lock (_mutex) { return _menu; } }
    }


    /// <summary>
    /// Store the entries.
    /// </summary>
    public void store()
    {
      lock (_mutex)
      {
        string[] names = _names.ToArray();
        this._store(names);
      }
    }


    /// <summary>
    /// Store the entries.
    /// </summary>
    private void _store ( string[] names )
    {
      lock (_mutex)
      {
        CadKit.Persistence.Registry.Instance.setUint(_section, _maxCountKey, this.MaxCount);

        string total = "";
        foreach (string name in names)
        {
          total += System.String.Format("{0}{1}", name, _delimeter);
        }
        total = total.TrimEnd(new char[] { _delimeter });
        CadKit.Persistence.Registry.Instance.setString(_section, _entriesKey, total);
      }
    }
  }
}
