
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Interfaces
{
  public class Filter
  {
    public Filter(string text, string extensions)
    {
      _text = text;
      _extensions = extensions;
    }
    public string Text { get { lock (_mutex) { return _text; } } }
    public string Extensions { get { lock (_mutex) { return _extensions; } } }
    private string _text = null;
    private string _extensions = null;
    private object _mutex = new object();
  }

  public class Filters : System.Collections.Generic.List<Filter> { }
}
