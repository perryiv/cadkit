
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Helios
{
  public class RecentFiles : CadKit.Interfaces.IRecentFileList
  {
    /// <summary>
    /// Local types.
    /// </summary>
    private class Entry
    {
      private static int _count = 0;
      private int _num = _count++;
      private string _name = null;
      public Entry(string name) { _name = name; }
      public string Name { get { return _name; } }
      public int Num { get { return _num; } }
    }
    private class CompareTime : System.Collections.Generic.IComparer<Entry>
    {
      public int Compare(Entry left, Entry right)
      {
        return (left.Num - right.Num);
      }
    }
    private class Entries : System.Collections.Generic.List<Entry> { }

    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();
    private Entries _entries = new Entries();
    private uint _maxCount = 10;
    private System.Windows.Forms.ToolStripMenuItem _menu = new System.Windows.Forms.ToolStripMenuItem();
    private string _section = "RecentFiles";
    private string _key = "Entries";
    private char _delimeter = ';';

    /// <summary>
    /// Constructor.
    /// </summary>
    public RecentFiles()
    {
      string temp = CadKit.Persistence.Registry.Instance.getString(_section, _key, "");
      if (temp.Length > 0)
      {
        string[] names = temp.Split(new char[] { _delimeter });
        foreach (string name in names)
        {
          this.add(name);
        }
      }

      _menu.Text = "Recent &Files";
      _menu.DropDownOpening += this._menuDropDownOpening;
      this._buildMenu();
    }

    /// <summary>
    /// Called when the menu is being shown.
    /// </summary>
    private void _menuDropDownOpening(object sender, System.EventArgs args)
    {
      this._buildMenu();
    }

    /// <summary>
    /// Build the menu.
    /// </summary>
    private void _buildMenu()
    {
      try
      {
        _menu.DropDownItems.Clear();
        for (int i = 0; i < _entries.Count; ++i)
        {
          CadKit.Helios.Commands.RecentDocumentCommand recent = new CadKit.Helios.Commands.RecentDocumentCommand(_entries[i].Name, (uint)i + 1, CadKit.Helios.Application.Instance.MainForm);
          _menu.DropDownItems.Add(recent.MenuButton);
        }

        CadKit.Tools.Menu.appendSeparatorIfNeeded(_menu);

        System.Windows.Forms.ToolStripMenuItem clearButton = new System.Windows.Forms.ToolStripMenuItem();
        clearButton.Text = "&Clear";
        clearButton.Click += this._clearButtonClicked;
        _menu.DropDownItems.Add(clearButton);
        clearButton.Enabled = (_entries.Count > 0);
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
        if (null != name && name.Length > 0)
        {
          _entries.Add(new Entry(name));
          _entries.Sort(new CompareTime());

          // Trim extra.
          if ((uint)_entries.Count > this.MaxCount)
          {
            _entries.RemoveRange((int)this.MaxCount, _entries.Count - (int)this.MaxCount);
          }

          this._store();
        }
      }
    }

    /// <summary>
    /// Remove the name.
    /// </summary>
    void CadKit.Interfaces.IRecentFileList.remove(string name)
    {
      lock (_mutex)
      {
        this.remove(name);
      }
    }

    /// <summary>
    /// Remove the name.
    /// </summary>
    public void remove(string name)
    {
      lock (_mutex)
      {
        foreach (Entry entry in _entries)
        {
          if (entry.Name == name)
          {
            _entries.Remove(entry);
            this._store();
            return;
          }
        }
      }
    }

    /// <summary>
    /// Clear the list.
    /// </summary>
    void CadKit.Interfaces.IRecentFileList.clear()
    {
      lock (_mutex)
      {
        this.clear();
      }
    }

    /// <summary>
    /// Clear the list.
    /// </summary>
    public void clear()
    {
      lock (_mutex)
      {
        _entries.Clear();
        this._store();
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
    private void _store()
    {
      lock (_mutex)
      {
        string total = "";
        foreach (Entry entry in _entries)
        {
          total += System.String.Format("{0}{1}", entry.Name, _delimeter);
        }
        total = total.TrimEnd(new char[] { _delimeter });
        if (total.Length > 0)
        {
          CadKit.Persistence.Registry.Instance.setString(_section, _key, total);
        }
      }
    }
  }
}

//Need a queue instead of a sorted list.
