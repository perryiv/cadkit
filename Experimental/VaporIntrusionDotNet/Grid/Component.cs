using System;
using System.Collections.Generic;
using System.Text;

namespace VaporIntrusionDotNet
{
  class Component
  {
    //*************************************************************************************
    //
    // Member Variables
    //
    //*************************************************************************************

    private string _name;
    private string _value;

    //*************************************************************************************
    //
    // Empty Constructor
    //
    //*************************************************************************************

    public Component()
    {
      _name = "";
      _value = "";
    }

    //*************************************************************************************
    //
    // Value Constructor
    //
    //*************************************************************************************

    public Component( string name, string value)
    {
      _name = name;
      _value = value;
    }

    //*************************************************************************
    // 
    // Get/Set the name
    //
    //*************************************************************************

    public string Name
    {
      get
      {
        return _name;
      }
      set
      {
        _name = value;
      }
    }

    //*************************************************************************
    // 
    // Get/Set the value
    //
    //*************************************************************************

    public string Value
    {
      get
      {
        return _value;
      }
      set
      {
        _value = value;
      }
    }


  }
}
