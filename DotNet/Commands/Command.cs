
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
    /// Delegates.
    /// </summary>
    private delegate void InvalidateToolbarDelegate();


    /// <summary>
    /// Data members.
    /// </summary>
    private CadKit.Threads.Tools.Lock _lock = new CadKit.Threads.Tools.Lock();
    protected object _caller = null;
    protected System.Windows.Forms.ToolStripMenuItem _menuButton = null;
    protected System.Windows.Forms.ToolStripButton _toolButton = null;
    protected string _text = "Default Text";
    protected string _toolTipText = "";
    protected System.Drawing.Image _menuIcon = null;
    protected System.Drawing.Image _toolIcon = null;
    protected System.Windows.Forms.Keys _keys = 0;


    /// <summary>
    /// Constructor.
    /// </summary>
    protected Command()
    {
    }


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
    /// Determine if the button is checked.
    /// </summary>
    protected virtual bool _isChecked()
    {
      return false;
    }


    /// <summary>
    /// Is there a menu button?
    /// </summary>
    public bool HasMenuButton
    {
      get
      {
        using (this.Lock.read())
        {
          return (null != _menuButton);
        }
      }
    }


    /// <summary>
    /// Is there a tool bar button?
    /// </summary>
    public bool HasToolButton
    {
      get
      {
        using (this.Lock.read())
        {
          return (null != _toolButton);
        }
      }
    }


    /// <summary>
    /// Get the owner of the tool button, which may be null.
    /// </summary>
    public System.Windows.Forms.ToolStrip ToolButtonOwner
    {
      get
      {
        if (false == this.HasToolButton)
        {
          return null;
        }
        using (this.Lock.read())
        {
          return _toolButton.Owner;
        }
      }
    }


    /// <summary>
    /// Get the menu button.
    /// </summary>
    public System.Windows.Forms.ToolStripMenuItem MenuButton
    {
      get
      {
        if (false == this.HasMenuButton)
        {
          this._createMenuButton();
        }
        using (this.Lock.read())
        {
          return _menuButton;
        }
      }
      set
      {
        using (this.Lock.write())
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
        if (false == this.HasToolButton)
        {
          this._createToolButton();
        }
        using (this.Lock.read())
        {
          return _toolButton;
        }
      }
      set
      {
        using (this.Lock.write())
        {
          _toolButton = value;
        }
      }
    }


    /// <summary>
    /// Create the menu button.
    /// </summary>
    private void _createMenuButton()
    {
      System.Windows.Forms.ToolStripMenuItem menuButton = new System.Windows.Forms.ToolStripMenuItem();
      menuButton.Text = _text;
      menuButton.Click += this._onButtonClicked;
      menuButton.Paint += this._onButtonPaint;
      menuButton.ShortcutKeys = _keys;
      menuButton.Image = _menuIcon;
      menuButton.Enabled = false;
      this.MenuButton = menuButton;
    }


    /// <summary>
    /// Create the tool bar button.
    /// </summary>
    private void _createToolButton()
    {
      _toolButton = new System.Windows.Forms.ToolStripButton();
      _toolButton.Click += this._onButtonClicked;
      _toolButton.Click += this._onToolButtonClicked;
      _toolButton.Paint += this._onButtonPaint;
      _toolButton.Paint += this._onToolButtonPaint;
      _toolButton.Image = _toolIcon;
      _toolButton.Enabled = false;
      _toolButton.ToolTipText = this._toolTipText;
      this.ToolButton = _toolButton;
    }


    /// <summary>
    /// Called when the button is pressed.
    /// </summary>
    private void _onButtonClicked(object sender, System.EventArgs args)
    {
      try
      {
        this._execute();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 1887520459: {0}", e.Message);
      }
    }


    /// <summary>
    /// Called when the button is pressed.
    /// </summary>
    private void _onToolButtonClicked(object sender, System.EventArgs args)
    {
      try
      {
        System.Windows.Forms.ToolStripButton button = sender as System.Windows.Forms.ToolStripButton;
        if ((null != button) && (null != button.Owner))
        {
          // Update the appearance of all toolbar buttons, including this one.
          button.Owner.Invalidate();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2568530481: {0}", e.Message);
      }
    }


    /// <summary>
    /// Called when the button is about to be shown.
    /// </summary>
    private void _onButtonPaint(object sender, System.Windows.Forms.PaintEventArgs args)
    {
      try
      {
        System.Windows.Forms.ToolStripItem item = sender as System.Windows.Forms.ToolStripItem;
        if (null != item)
        {
          item.Enabled = this._shouldBeEnabled();
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 2046322558: {0}", e.Message);
      }
    }


    /// <summary>
    /// Called when the button is about to be shown.
    /// </summary>
    private void _onToolButtonPaint(object sender, System.Windows.Forms.PaintEventArgs args)
    {
      try
      {
        System.Windows.Forms.ToolStripButton button = sender as System.Windows.Forms.ToolStripButton;
        if (null != button)
        {
          button.CheckState = (this._isChecked()) ? System.Windows.Forms.CheckState.Checked : System.Windows.Forms.CheckState.Unchecked;
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3858697618: {0}", e.Message);
      }
    }


    /// <summary>
    /// Get the lock.
    /// </summary>
    protected CadKit.Threads.Tools.Lock Lock
    {
      get { return _lock; }
    }


    /// <summary>
    /// Called when the active view changes.
    /// </summary>
    protected void _onActiveViewChanged(CadKit.Interfaces.IDocumentView oldView, CadKit.Interfaces.IDocumentView newView)
    {
      try
      {
        this._invalidateToolButton();
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3503365250: {0}", e.Message);
      }
    }


    /// <summary>
    /// Invalidate the tool button.
    /// </summary>
    private void _invalidateToolButton()
    {
      try
      {
        // Do not call this.ToolButton! We do not want to make one.
        if (true == this.HasToolButton)
        {
          if (null != this.ToolButtonOwner)
          {
            if (true == this.ToolButtonOwner.InvokeRequired)
            {
              this.ToolButtonOwner.BeginInvoke(new InvalidateToolbarDelegate(this._invalidateToolButton));
            }
            else
            {
              this.ToolButton.Invalidate();
            }
          }
        }
      }
      catch (System.Exception e)
      {
        System.Console.WriteLine("Error 3875510231: {0}", e.Message);
      }
    }
  }
}
