
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Plugins.Windows.OutputWindow
{
  public class ScopedDocumentStyle : System.IDisposable
  {
    private WeifenLuo.WinFormsUI.DockPanel _panel = null;
    WeifenLuo.WinFormsUI.DocumentStyles _style = WeifenLuo.WinFormsUI.DocumentStyles.DockingWindow;

    public ScopedDocumentStyle(WeifenLuo.WinFormsUI.DockPanel panel)
      : base()
    {
      _panel = panel;
      if (null != _panel)
      {
        _style = _panel.DocumentStyle;
      }
    }

    public void Dispose()
    {
      if (null != _panel && null != _style)
      {
        _panel.DocumentStyle = _style;
      }
    }
  }
}
