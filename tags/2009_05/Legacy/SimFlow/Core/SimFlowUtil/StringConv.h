
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Auhor(s): Aashish Chaudhary
//
///////////////////////////////////////////////////////////////////////////////

#include <string>
#include <sstream>

namespace SimFlow
{
	namespace SimFlowUtil
	{
		namespace String
		{			
			std::string convertToString( const float& val )
			{
				std::ostringstream buffer;
				buffer << val;
				std::string str = buffer.str();
				return str;
			}

			template< class TYPE > 			
			TYPE convert( TYPE type, const char* text )
			{
				std::string str( text );
				std::istringstream buffer( str );
				buffer >> type;
				return type;
			}

			float convertToFloat( const std::string& val )
			{
				float fl = 0.0;
				std::istringstream buffer ( val );
				buffer >> fl;
				return fl;
			}

			int convertToInt( const std::string& val )
			{				
				int in = 0;
				std::istringstream buffer ( val );
				buffer >> in;
				return in;
			}

			std::string stringcpy( const char* constCharArray, int max_size )
			{
				std::string returnString;
				char* charArray = new char[max_size];
				bool flag = false;

				//std::cerr << " array size is: " << sizeof( charArray ) << std::endl; 
				//std::cerr << " array size is: " << sizeof( constCharArray ) << std::endl; 
				
				for( int i=0; i < ( max_size -1 ); ++i)
				{
					if( constCharArray[i] != '\0' && flag == false )
					{
						charArray[i] = constCharArray[i];
					}

					if( constCharArray[i] == 0)
					{
						flag = true;
					}

					if( flag == true )
					{
						charArray[i] = ' ';
					}
				}
				charArray[max_size-1] = '\0';

				returnString = std::string(charArray);
				delete charArray;
				
				return returnString;
			}			
		}
	}
}

