
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

namespace CadKit.Persistence
{
  public class Registry : System.Object
  {
    /// <summary>
    /// Constructor for registry class.
    /// </summary>
    private Registry()
    {
      this.read();
    }

    ~Registry()
    {
      this.write();
    }

    /// <summary>
    /// Return file name.
    /// </summary>
    private string _file()
    {
      lock ( _mutex )
      {
        string program = System.Reflection.Assembly.GetEntryAssembly().GetName().Name; // Handles ".vshost"
        string file = System.Windows.Forms.Application.UserAppDataPath + "\\" + program + ".xml";
        return file;
      }
    }

    /// <summary>
    /// Single instance.
    /// </summary>
    public static Registry Instance
    {
      get
      {
        lock ( "CadKit.Persistence.Registry.Instance" )
        {
          if (null == _instance)
            _instance = new Registry();
          return _instance;
        }
      }
    }

    /// <summary>
    /// Read the registry.
    /// </summary>
    public void read()
    {
      try
      {
        lock (_mutex)
        {
          System.Xml.XmlDocument doc = new System.Xml.XmlDocument();
          doc.Load(this._file());

          System.Xml.XmlNode root = doc.FirstChild;
          if (null == root)
            return;

          System.Xml.XmlNodeList kids = root.ChildNodes;
          for (int i = 0; i < kids.Count; ++i)
          {
            System.Xml.XmlNode node = kids[i];
            string name = node.Name;
            Section section = new Section();
            _sections[name] = section;

            System.Xml.XmlNodeList grandKids = node.ChildNodes;
            if (null != grandKids)
            {
              for (int j = 0; j < grandKids.Count; ++j)
              {
                node = grandKids[j];
                string key = node.Name;
                string value = node.InnerText;
                section[key] = value;
              }
            }
          }
        }
      }
      catch ( System.IO.FileNotFoundException )
      {
        // This is expected.
      }
      catch ( System.Exception e )
      {
        System.Console.WriteLine( "Error 1204264394: {0}", e.Message );
      }
    }

    /// <summary>
    /// Write registry to file.
    /// </summary>
    public void write()
    {
      try
      {
        lock (_mutex)
        {
          System.Xml.XmlDocument doc = new System.Xml.XmlDocument();
          System.Xml.XmlElement root = doc.CreateElement("registry");
          doc.AppendChild(root);

          foreach (string section in _sections.Keys)
          {
            Section hash = this.getSectionHash(section);
            System.Xml.XmlElement element = doc.CreateElement(section);
            root.AppendChild(element);
            foreach (string key in hash.Keys)
            {
              string value = hash[key].ToString();
              System.Xml.XmlElement child = doc.CreateElement(key);
              child.InnerText = value;
              element.AppendChild(child);
            }
          }

          doc.Save(this._file());
        }
      }
      catch ( System.Exception e )
      {
        System.Console.WriteLine( "Error {0}: {1}", 2112056395, e.Message );
      }
    }

    /// <summary>
    /// Get the string.
    /// </summary>
    public string getString( string section, string key, string defaultValue )
    {
      lock (_mutex)
      {
        Section hash = this.getSectionHash(section);
        try
        {
          return hash[key];
        }
        catch (System.Collections.Generic.KeyNotFoundException)
        {
          hash[key] = defaultValue;
          return defaultValue;
        }
      }
    }

    /// <summary>
    /// Set the string.
    /// </summary>
    public void setString( string section, string key, string value )
    {
      lock (_mutex)
      {
        Section hash = this.getSectionHash(section);
        hash[key] = value;
      }
    }

    /// <summary>
    /// Get the value.
    /// </summary>
    public int getInt( string section, string key, int defaultValue )
    {
      lock (_mutex)
      {
        return System.Convert.ToInt32(this.getString(section, key, defaultValue.ToString()));
      }
    }

    /// <summary>
    /// Set the value.
    /// </summary>
    public void setInt( string section, string key, int value )
    {
      lock (_mutex)
      {
        this.getSectionHash(section)[key] = value.ToString();
      }
    }

    /// <summary>
    /// Get the value.
    /// </summary>
    public bool getBool( string section, string key, bool defaultValue )
    {
      lock (_mutex)
      {
        return System.Convert.ToBoolean(this.getString(section, key, defaultValue.ToString()));
      }
    }

    /// <summary>
    /// Set the value.
    /// </summary>
    public void setBool( string section, string key, bool value )
    {
      lock (_mutex)
      {
        this.getSectionHash(section)[key] = value.ToString();
      }
    }

    /// <summary>
    /// Get the value.
    /// </summary>
    public System.Drawing.Color getColor( string section, string key, System.Drawing.Color defaultValue )
    {
      lock (_mutex)
      {
        System.Drawing.Color color = new System.Drawing.Color();
        color = defaultValue;
        string temp = this.getString(section, key, defaultValue.Name);
        if (null != temp)
        {
          System.Drawing.Color named = System.Drawing.Color.FromName(temp);
          if (0 == named.A && 0 == named.R && 0 == named.G && 0 == named.B)
          {
            int a = color.A;
            int r = color.R;
            int g = color.G;
            int b = color.B;

            string[] parts = temp.Split(new char[] { ' ' });
            if (4 == parts.Length)
            {
              a = System.Convert.ToByte(parts[0]);
              r = System.Convert.ToByte(parts[1]);
              g = System.Convert.ToByte(parts[2]);
              b = System.Convert.ToByte(parts[3]);
            }
            color = System.Drawing.Color.FromArgb(a, r, g, b);
          }
          else
          {
            color = named;
          }
        }
        return color;
      }
    }

    /// <summary>
    /// Set the value.
    /// </summary>
    public void setColor( string section, string key, System.Drawing.Color value )
    {
      lock (_mutex)
      {
        if (null != value)
        {
          if (true == value.IsNamedColor)
          {
            this.getSectionHash(section)[key] = value.Name;
          }
          else
          {
            string temp = System.String.Format("{0} {1} {2} {3}", value.A, value.R, value.G, value.B);
            this.getSectionHash(section)[key] = temp;
          }
        }
      }
    }

    /// <summary>
    /// Get the section hash.
    /// </summary>
    public Section getSectionHash( string section )
    {
      lock (_mutex)
      {
        Section hash = null;
        try
        {
          hash = _sections[section];
        }
        catch (System.Collections.Generic.KeyNotFoundException)
        {
          hash = null;
        }
        if (null == hash)
        {
          hash = new Section();
          _sections[section] = hash;
        }
        return hash;
      }
    }

    /// <summary>
    /// Data members.
    /// </summary>
    private static Registry _instance = null;
    public class Section : System.Collections.Generic.Dictionary<string, string> { }
    public class Sections : System.Collections.Generic.Dictionary<string, Section> { }
    private Sections _sections = new Sections();
    private object _mutex = new object();
  }
}
