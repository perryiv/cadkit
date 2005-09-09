///////////////////////////////////////////////////////////////////////////////
////
////  SinterPoint class to allow remote control of cadviewer
////
/////////////////////////////////////////////////////////////////////////////////

#ifndef _CV_SINTER_APP_DATA_CLASS_H_
#define _CV_SINTER_APP_DATA_CLASS_H_

# include "sinter/SinterPoint.h"
# include <vpr/IO/SerializableObject.h>
# include <vpr/IO/ObjectReader.h>
# include <vpr/IO/ObjectWriter.h>
# include <plugins/ApplicationDataManager/UserData.h>

namespace CV{

  enum SinterFileState { NOTHING, RECEIVE, DONE };

  class SinterAppData : public vpr::SerializableObject
    {
    public:
      virtual vpr::ReturnStatus readObject  ( vpr::ObjectReader* reader )
      {
        _data = reader->readString();
        _state = reader->readUint32();
        return vpr::ReturnStatus::Succeed;
      }

      virtual vpr::ReturnStatus writeObject ( vpr::ObjectWriter* writer )
      {
        writer->writeString ( _data );
        writer->writeUint32 ( _state );
        return vpr::ReturnStatus::Succeed;
      }
    public:
      std::string _data;
      int _state;
    };

};

#endif
