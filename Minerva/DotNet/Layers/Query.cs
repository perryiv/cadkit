
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Decision Theater at Arizona State University
//  All rights reserved.
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

namespace DT.Minerva.Layers.Controls
{
  [
  System.Serializable()
  ]
  public class Query :
    System.Runtime.Serialization.ISerializable
  {
    public Query()
    {
    }

    public Query(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
    }

    void System.Runtime.Serialization.ISerializable.GetObjectData(System.Runtime.Serialization.SerializationInfo info, System.Runtime.Serialization.StreamingContext context)
    {
    }


    public string TheQuery
    {
      get
      {
        return "";
      }
    }


    /// <summary>
    /// 
    /// </summary>
    public virtual string Number
    {
      get { return "NA"; }
    }


    /// <summary>
    /// 
    /// </summary>
    public virtual string Min
    {
      get { return "NA"; }
    }


    /// <summary>
    /// 
    /// </summary>
    public virtual string Max
    {
      get { return "NA"; }
    }
  }
}
