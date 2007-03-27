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

namespace CV
{

 enum SinterState { COMMAND, DATA };

 class SinterAppData : public vpr::SerializableObject
   {
   public:
     virtual vpr::ReturnStatus readObject  ( vpr::ObjectReader* reader )
     {
        vpr::BufferObjectReader* bufreader = 
          static_cast < vpr::BufferObjectReader* > ( reader );
        
        vpr::Uint32 str_len = bufreader->readUint32();
        
        if ( !_data.empty() ) _data.clear();
        
        for(unsigned i = 0; i < str_len; ++i)
        {
          _data += (char)(*bufreader->readRaw(1));
        }

        return vpr::ReturnStatus::Succeed;
     }

     virtual vpr::ReturnStatus writeObject ( vpr::ObjectWriter* writer )
     {
        vpr::BufferObjectWriter* bufwriter = 
          static_cast < vpr::BufferObjectWriter* > ( writer );
        
        bufwriter->writeUint32(_data.size());
        
        for(unsigned i = 0; i < _data.length(); ++i)
        {
          bufwriter->writeRaw((vpr::Uint8*)&(_data[i]),1);
        }
        
        return vpr::ReturnStatus::Succeed;
     }

   public:
     std::string _data;
   };
   
}; 

#endif
