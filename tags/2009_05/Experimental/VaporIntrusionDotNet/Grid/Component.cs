///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author(s): Jeff Conner
//
///////////////////////////////////////////////////////////////////////////////

using System;
using System.Collections.Generic;
using System.Text;

using System.ComponentModel;
using System.Data;
using System.Drawing;

namespace VaporIntrusionDotNet
{
  public class Component
  {
    //*************************************************************************************
    //
    // Member Variables
    //
    //*************************************************************************************

    private string _name;
    private string _value;
    private Color _color;
    private Point _point;

    //*************************************************************************************
    //
    // Empty Constructor
    //
    //*************************************************************************************

    public Component()
    {
      _name = "";
      _value = "";
      _point = new Point(0, 0);
      _color = Color.Red;
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

    public string name
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

    public string value
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


    //*************************************************************************
    // 
    // Get/Set the color
    //
    //*************************************************************************

    public Color color
    {
      get
      {
        return _color;
      }
      set
      {
        _color = value;
      }
    }


    //*************************************************************************
    // 
    // Get/Set the point
    //
    //*************************************************************************

    public Point point
    {
      get
      {
        return _point;
      }
      set
      {
        _point = value;
      }
    }





  }
}
