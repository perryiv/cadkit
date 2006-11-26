
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Commands
{
  public abstract class Command : CadKit.Interfaces.ICommand
  {
    /// <summary>
    /// Data members.
    /// </summary>
    private object _mutex = new object();
    protected object _caller = null;
    protected System.Windows.Forms.ToolStripMenuItem _menuButton = null;
    protected System.Windows.Forms.ToolStripButton _toolButton = null;
    protected string _text = "Default Text";
    protected System.Drawing.Image _menuIcon = null;
    protected System.Drawing.Image _toolIcon = null;
    protected System.Windows.Forms.Keys _keys = 0;

    /// <summary>
    /// Can the command be undone?
    /// </summary>
    bool CadKit.Interfaces.ICommand.canUndo()
    {
      return this._canUndo();
    }

    /// <summary>
    /// Can the command be undone?
    /// </summary>
    protected virtual bool _canUndo()
    {
      return false;
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    void CadKit.Interfaces.ICommand.execute()
    {
      this._execute();
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    protected abstract void _execute();

    /// <summary>
    /// Undo the command.
    /// </summary>
    void CadKit.Interfaces.ICommand.undo()
    {
      this._undo();
    }

    /// <summary>
    /// Undo the command.
    /// </summary>
    protected void _undo()
    {
    }

    /// <summary>
    /// Determine if the button should be enabled.
    /// </summary>
    protected virtual bool _shouldBeEnabled()
    {
      return true;
    }

    /// <summary>
    /// Get the menu button.
    /// </summary>
    public System.Windows.Forms.ToolStripMenuItem MenuButton
    {
      get
      {
        lock (this.Mutex)
        {
          if (null == _menuButton)
          {
            _menuButton = new System.Windows.Forms.ToolStripMenuItem();
            _menuButton.Text = _text;
            _menuButton.Click += this._onButtonClicked;
            _menuButton.Paint += this._onButtonPaint;
            _menuButton.ShortcutKeys = _keys;
            _menuButton.Image = _menuIcon;
            _menuButton.Enabled = false;
          }
          return _menuButton;
        }
      }
      set
      {
        lock (this.Mutex)
        {
          _menuButton = value;
        }
      }
    }

    /// <summary>
    /// Get the toolbar button.
    /// </summary>
    public System.Windows.Forms.ToolStripButton ToolButton
    {
      get
      {
        lock (this.Mutex)
        {
          if (null == _toolButton)
          {
            _toolButton = new System.Windows.Forms.ToolStripButton();
            _toolButton.Click += this._onButtonClicked;
            _toolButton.Paint += this._onButtonPaint;
            _toolButton.Image = _toolIcon;
            _toolButton.Enabled = false;
          }
          return _toolButton;
        }
      }
      set
      {
        lock (this.Mutex)
        {
          _toolButton = value;
        }
      }
    }

    /// <summary>
    /// Called when the button is pressed.
    /// </summary>
    private void _onButtonClicked(object sender, System.EventArgs args)
    {
      try
      {
        lock (this.Mutex)
        {
          this._execute();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1887520459: {0}", e.Message);
      }
    }

    /// <summary>
    /// Called when the button is about to be shown.
    /// </summary>
    private void _onButtonPaint(object sender, System.Windows.Forms.PaintEventArgs args)
    {
      try
      {
        lock (this.Mutex)
        {
          System.Windows.Forms.ToolStripItem item = sender as System.Windows.Forms.ToolStripItem;
          if (null != item)
            item.Enabled = this._shouldBeEnabled();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2046322558: {0}", e.Message);
      }
    }

    /// <summary>
    /// Get the mutex.
    /// </summary>
    protected object Mutex
    {
      get { return _mutex; }
    }
  }
}
