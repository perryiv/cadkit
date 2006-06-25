
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Commands
{
  public abstract class Command
  {
    /// <summary>
    /// Data members.
    /// </summary>
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
    public virtual bool canUndo()
    {
      return false;
    }

    /// <summary>
    /// Can the command be redone?
    /// </summary>
    public virtual bool canRedo()
    {
      return false;
    }

    /// <summary>
    /// Execute the command.
    /// </summary>
    public abstract void execute();

    /// <summary>
    /// Determine of the button should be enabled.
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
        if (null == _menuButton)
        {
          _menuButton = new System.Windows.Forms.ToolStripMenuItem();
          _menuButton.Text = _text;
          _menuButton.Click += this._onButtonClicked;
          _menuButton.Paint += this._onButtonPaint;
          _menuButton.ShortcutKeys = _keys;
          _menuButton.Image = _menuIcon;
        }
        return _menuButton;
      }
      set { _menuButton = value; }
    }

    /// <summary>
    /// Get the toolbar button.
    /// </summary>
    public System.Windows.Forms.ToolStripButton ToolButton
    {
      get
      {
        if (null == _toolButton)
        {
          _toolButton = new System.Windows.Forms.ToolStripButton();
          _toolButton.Click += this._onButtonClicked;
          _toolButton.Paint += this._onButtonPaint;
          _toolButton.Image = _toolIcon;
        }
        return _toolButton;
      }
      set { _toolButton = value; }
    }

    /// <summary>
    /// Called when the button is pressed.
    /// </summary>
    private void _onButtonClicked(object sender, System.EventArgs e)
    {
      this.execute();
      CadKit.Commands.History.instance().AddLast(this);
    }

    /// <summary>
    /// Called when the button is about to be shown.
    /// </summary>
    private void _onButtonPaint(object sender, System.Windows.Forms.PaintEventArgs e)
    {
      System.Windows.Forms.ToolStripItem item = sender as System.Windows.Forms.ToolStripItem;
      if (null != item)
        item.Enabled = this._shouldBeEnabled();
    }
  }
}
