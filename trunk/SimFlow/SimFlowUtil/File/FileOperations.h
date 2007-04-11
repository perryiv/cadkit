///////////////////////////////////////////////////////////////////////////////
//
// Helper functions to perform operatin on files / dirs.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FILE_FILE_OPERATIONS_H__
#define __FILE_FILE_OPERATIONS_H__

#include <iostream>
#include <stdexcept>

#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/algorithm/string/predicate.hpp"

namespace fs = boost::filesystem;

namespace SimFlow
{
	namespace SimFlowUtil
	{
		namespace File
		{
			void print( const std::string& text )
			{			
				std::string message( text );
				std::cout << message << std::endl;
			}

			bool exists( const std::string& path )
			{
        // bool b = true;
				
        fs::path fPath = fs::path( path, fs::native );
				return fs::exists( fPath  );

        /*
				if( b )
				{
					b = fs::is_directory( fPath );
				}
				else
				{
					b = false;
				}

				return b;
        */
			}

      bool isDirectory( const std::string& path )
      {
        bool b = exists( path );

        if( b )
        {
          return fs::is_directory( path );
        }

        return b;
      }

			bool create( const std::string& path )
			{
				if( exists( path ) ) return true;
				fs::path fPath( path, fs::native );
				return fs::create_directory( fPath );		
			}

			std::string combine( const std::string& path1, const std::string& path2 )
			{
				if( ( !exists( path1 ) ) && ( !exists ( path2 ) ) )
					throw std::runtime_error( std::string( "One or more invalid paths " ) );
				fs::path fPath1( path1, fs::native );
				fs::path fPath2( path2, fs::native );
				fPath1 = fPath1 / fPath2.leaf();
				return fPath1.string();			
			}

			///////////////////////////////////////////////////////////////////////////
			//
			// Helper funtion to get the content of a dir ( using boost::filesystem ). 		
			//
			///////////////////////////////////////////////////////////////////////////
			template< class T >
			bool findFile( const std::string& dirPath, const std::string& ext, T& container, bool fullPath=true )
			{
        if( !exists( dirPath ) ) return false;
				fs::path path( dirPath, fs::native );
				return findFile( path, ext, container, fullPath );
			}

			template< class T >
			bool findFile( fs::path& path, const std::string& ext, T& container, bool fullPath=true )
			{
				fs::directory_iterator dirItr( path );
				fs::directory_iterator endItr;

				print( std::string( "[Directory] " ) + path.string() );
				for( ; dirItr != endItr; ++dirItr )
				{
					try
					{
						if( fs::is_directory( *dirItr ) )
						{
							print( std::string( "[in_directory]  " ) + path.string() );
							findFile( *dirItr, ext, container );
							print( std::string( "[out_directory] " ) + path.string() );
						}
						else
						{
							//if( strcmp( dirItr->leaf().substr( ( dirItr->leaf().size() - 3  ), 3 ).c_str(), ext.c_str() )  == 0 )
							if( boost::algorithm::iends_with(  dirItr->leaf(), ext ) )
							{
								print( std::string( "[File] " ) + dirItr->leaf() );
								if( fullPath )
								{
									container.insert( container.end(), dirItr->native_file_string() );			
								}
								else
								{
									container.insert( container.end(), dirItr->leaf() );
								}
							}
						}
					}
					catch( std::exception& ex )
					{
						std::cerr << "[ERROR:ERRORNO] " << dirItr->leaf() << " " << ex.what() << std::endl;
					}		
				}
				print( std::string( "[/Directory] " ) + path.string() );
				return true;
			}
		}	

		///////////////////////////////////////////////////////////////////////////
		//
		// Helper funtion to get the content of a dir. 
		//
		///////////////////////////////////////////////////////////////////////////
		template< class T >
		void findFile( const std::string &d, const std::string& ext, T& container )
		{
			#if _WIN32
				
			  // Initialize.
			  HANDLE handle = 0x0;

			  // Safely...
			  try
			  {
				// For convenience.
				const std::string dir ( d + std::string("\\") );
				const std::string path ( dir + "*." + ext );

				// Get the first file in the directory.
				WIN32_FIND_DATA data;
				char dirSpec[MAX_PATH];
				strcpy_s( dirSpec, path.c_str() );
				handle = FindFirstFile( dirSpec, &data );
				if ( INVALID_HANDLE_VALUE == handle )
				  return;

				// Push the first file onto the list.			
				// @Todo: Check for multi-byte/unicode string.
				container.insert ( container.end(), dir + data.cFileName );			

				// Push any additional files.
				// @Todo: Check for multi-byte/unicode string.
				while ( ::FindNextFile ( handle, &data ) )
				{
					container.insert ( container.end(), dir + data.cFileName );				
				}
			  }

			  // Catch all exceptions.
			  catch ( ... )
			  {
				// Close the directory handle and re-throw.
				if ( handle )
				  ::FindClose ( handle );
				throw;
			  }

			  // Close the directory handle.
			  if ( handle )	::FindClose ( handle );

			//#else			
				struct dirent *dp;

				// Get directory stream pointer
				if ( DIR * dirp = opendir ( d.c_str() ) )
				{
					// Loop over directory entries
					struct dirent dirresult;
				    
					while ( !readdir_r ( dirp, &dirresult, &dp ) && dp )
					{
						// Get name
						std::string name ( dp->d_name );

						if ( Usul::File::extension ( name ) == ext )
						{
							// Add to list
							names.insert ( names.end(), name );
						}
					}
					closedir ( dirp );
				}
			#endif		
		}	
	}
}

#endif // __FILE_FILE_OPERATIONS_H__

