
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Perry L Miller IV
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
//  Pixel format class.
//
///////////////////////////////////////////////////////////////////////////////

#include "DotNet/Glue/PixelFormat.h"

#include "Usul/System/LastError.h"
#include "Usul/Bits/Bits.h"
#include "Usul/Errors/Assert.h"
#include "Usul/Math/Transpose.h"
#include "Usul/Math/MinMax.h"

#include <limits>
#include <sstream>
#include <list>

using namespace CadKit::OpenGL::Glue;


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

PixelFormat::PixelFormat ( DeviceContext ^dc, unsigned int index ) : BaseClass(),
  _pfd   ( new PixelFormat::Descriptor ),
  _index ( ( index <= static_cast<unsigned int> ( std::numeric_limits<int>::max() ) ) ? static_cast<int> ( index ) : 0 ),
  _dc    ( dc )
{
  if ( nullptr == _dc )
    throw gcnew System::ArgumentException ( "Error 2360416570: invalid device context" );

  _dc->reference();

  try
  {
    this->_init();
  }

  catch ( ... )
  {
    this->_cleanup();
    throw;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

PixelFormat::~PixelFormat()
{
  System::Diagnostics::Debug::Assert ( 0x0 == _pfd );
  System::Diagnostics::Debug::Assert ( 0 == _index );
  System::Diagnostics::Debug::Assert ( nullptr == _dc );
  System::Diagnostics::Debug::Assert ( true == this->IsClean );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Clean up the resources.
//
///////////////////////////////////////////////////////////////////////////////

void PixelFormat::_cleanup()
{
  PixelFormat::Descriptor *pfd ( _pfd );
  DeviceContext ^dc = _dc;

  _pfd = 0x0;
  _index = 0;
  _dc = nullptr;

  if ( 0x0 == pfd )
    delete pfd;

  if ( nullptr != dc )
    dc->dereference();

  BaseClass::_cleanup();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the index.
//
///////////////////////////////////////////////////////////////////////////////

unsigned int PixelFormat::index()
{
  return _index;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the descriptor.
//
///////////////////////////////////////////////////////////////////////////////

const PixelFormat::Descriptor &PixelFormat::descriptor()
{
  return *_pfd;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Initialize.
//
///////////////////////////////////////////////////////////////////////////////

void PixelFormat::_init()
{
  System::Diagnostics::Debug::Assert ( 0x0 != _pfd );

  // Initialize the descriptor.
  ::memset ( _pfd, 0, sizeof ( Descriptor ) );

  // Get the last pixel format index.
  const int lastIndex ( this->_lastIndex() );

  // If the given format is out of range, use the default. Throws if it fails.
  if ( _index <= 0 || _index > lastIndex )
    _index = this->_default();

  // Set the format. Throws if it fails.
  this->_set();

  // Need to create a palette?
  USUL_ASSERT ( false == Usul::Bits::has ( _pfd->dwFlags, PFD_NEED_PALETTE ) );
}


///////////////////////////////////////////////////////////////////////////////
//
// Set the pixel format.
//
///////////////////////////////////////////////////////////////////////////////

void PixelFormat::_set()
{
  System::Diagnostics::Debug::Assert ( nullptr != _dc );
  System::Diagnostics::Debug::Assert ( 0x0 != _pfd );

  Usul::System::LastError::init();
  if ( FALSE == ::SetPixelFormat ( _dc->hdc(), _index, _pfd ) )
  {
    std::ostringstream out;
    out << "Error 4059039722: Failed to set pixel format. " << Usul::System::LastError::message();
    System::String ^temp = gcnew System::String ( out.str().c_str() );
    throw gcnew System::Exception ( temp );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Describe the pixel format.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  int describe ( HDC hdc, int index, PixelFormat::Descriptor &pfd )
  {
    int lastIndex ( ::DescribePixelFormat ( hdc, index, sizeof ( PixelFormat::Descriptor ), &pfd ) );
    return lastIndex; // Zero means it did not find a matching format.
  }
}


///////////////////////////////////////////////////////////////////////////////
//
// Get the maximum index.
//
///////////////////////////////////////////////////////////////////////////////

int PixelFormat::_lastIndex()
{
  System::Diagnostics::Debug::Assert ( nullptr != _dc );
  System::Diagnostics::Debug::Assert ( 0x0 != _pfd );

  const int firstIndex ( 1 );
  int lastIndex ( Detail::describe ( _dc->hdc(), firstIndex, *_pfd ) );
  return lastIndex;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the default format.
//
///////////////////////////////////////////////////////////////////////////////

int PixelFormat::_default()
{
  System::Diagnostics::Debug::Assert ( nullptr != _dc );
  System::Diagnostics::Debug::Assert ( 0x0 != _pfd );

  // Initialize the descriptor.
  ::memset ( _pfd, 0, sizeof ( Descriptor ) );
  _pfd->nVersion   = 1;
  _pfd->dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  _pfd->iPixelType = PFD_TYPE_RGBA;
  _pfd->cColorBits = 32;
  _pfd->cDepthBits = 32;
  _pfd->cAccumBits = 32;
  _pfd->iLayerType = PFD_MAIN_PLANE;

  // Ask the system to choose the closest match.
  int index ( ::ChoosePixelFormat ( _dc->hdc(), _pfd ) );
  if ( 0 == index )
    throw gcnew System::Exception ( "Error 1071206026: failed to choose default pixel format" );
  return index;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert value to string with requested padding.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class T > std::string toString ( const T &t, unsigned int num, char fill )
  {
    std::ostringstream out;
    out << t;
    std::string s = out.str();
    if ( s.size() < num )
      s = std::string ( num - s.size(), fill ) + s;
    return s;
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add column names.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class Table > void addColumnNames ( Table &t )
  {
    typedef typename Table::value_type Row;
    t.push_back ( Row() );
    Row &row = t.back();
    row.reserve ( 30 );
    row.push_back ( "Index" );

    row.push_back ( "Draw to Window" );
    row.push_back ( "Draw to Bitmap" );
    row.push_back ( "Support GDI" );
    row.push_back ( "Support OpenGL" );
    row.push_back ( "Generic Accel" );
    row.push_back ( "Generic Format" );
    row.push_back ( "Double Buffer" );
    row.push_back ( "Stereo" );
    row.push_back ( "Need Palette" );
    row.push_back ( "Need System Palette" );
    row.push_back ( "Swap Exchange" );
    row.push_back ( "Swap Copy" );
    row.push_back ( "Swap Layer Buffers" );
    row.push_back ( "Support DirectDraw" );

    row.push_back ( "Pixel Type" );
    row.push_back ( "Layer Type" );

    row.push_back ( "Color Bits" );
    row.push_back ( "Accum Bits" );
    row.push_back ( "Depth Bits" );
    row.push_back ( "Stencil Bits" );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a table row.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class Table > void addRow ( int index, const PixelFormat::Descriptor &pfd, Table &t )
  {
    typedef typename Table::value_type Row;

    const std::string YES ( "*" );
    const std::string NO  ( ""  );

    t.push_back ( Row() );
    Row &row = t.back();
    row.push_back ( Detail::toString ( index, 2, '0' ) );

    row.push_back ( ( Usul::Bits::has ( pfd.dwFlags,    PFD_DRAW_TO_WINDOW      ) ) ? YES : NO );
    row.push_back ( ( Usul::Bits::has ( pfd.dwFlags,    PFD_DRAW_TO_BITMAP      ) ) ? YES : NO );
    row.push_back ( ( Usul::Bits::has ( pfd.dwFlags,    PFD_SUPPORT_GDI         ) ) ? YES : NO );
    row.push_back ( ( Usul::Bits::has ( pfd.dwFlags,    PFD_SUPPORT_OPENGL      ) ) ? YES : NO );
    row.push_back ( ( Usul::Bits::has ( pfd.dwFlags,    PFD_GENERIC_ACCELERATED ) ) ? YES : NO );
    row.push_back ( ( Usul::Bits::has ( pfd.dwFlags,    PFD_GENERIC_FORMAT      ) ) ? YES : NO );
    row.push_back ( ( Usul::Bits::has ( pfd.dwFlags,    PFD_DOUBLEBUFFER        ) ) ? YES : NO );
    row.push_back ( ( Usul::Bits::has ( pfd.dwFlags,    PFD_STEREO              ) ) ? YES : NO );
    row.push_back ( ( Usul::Bits::has ( pfd.dwFlags,    PFD_NEED_PALETTE        ) ) ? YES : NO );
    row.push_back ( ( Usul::Bits::has ( pfd.dwFlags,    PFD_NEED_SYSTEM_PALETTE ) ) ? YES : NO );
    row.push_back ( ( Usul::Bits::has ( pfd.dwFlags,    PFD_SWAP_EXCHANGE       ) ) ? YES : NO );
    row.push_back ( ( Usul::Bits::has ( pfd.dwFlags,    PFD_SWAP_COPY           ) ) ? YES : NO );
    row.push_back ( ( Usul::Bits::has ( pfd.dwFlags,    PFD_SWAP_LAYER_BUFFERS  ) ) ? YES : NO );
    row.push_back ( ( Usul::Bits::has ( pfd.dwFlags,    PFD_SUPPORT_DIRECTDRAW  ) ) ? YES : NO );

    row.push_back ( ( PFD_TYPE_RGBA == pfd.iPixelType ) ? "RGBA" : "Index" );

    switch ( pfd.iLayerType )
    {
      case PFD_MAIN_PLANE:     row.push_back ( "Main" );     break;
      case PFD_OVERLAY_PLANE:  row.push_back ( "Overlay" );  break;
      case PFD_UNDERLAY_PLANE: row.push_back ( "Underlay" ); break;
      default:                 row.push_back ( "Unknown" );  break;
    }

    row.push_back ( Detail::toString ( static_cast < unsigned short > ( pfd.cColorBits   ), 2, '0' ) );
    row.push_back ( Detail::toString ( static_cast < unsigned short > ( pfd.cAccumBits   ), 2, '0' ) );
    row.push_back ( Detail::toString ( static_cast < unsigned short > ( pfd.cDepthBits   ), 2, '0' ) );
    row.push_back ( Detail::toString ( static_cast < unsigned short > ( pfd.cStencilBits ), 2, '0' ) );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove empty rows.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class Table > void removeEmptyRows ( Table &t )
  {
    typedef typename Table::value_type Row;
    std::list<unsigned int> emptyRows;
    for ( unsigned int i = 0; i < t.size(); ++i )
    {
      const Row &row = t[i];
      USUL_ASSERT ( row.size() == t.front().size() );
      bool emptyRow ( true );

      // Skip first element, it's the name.
      for ( unsigned int j = 1; j < row.size(); ++j )
      {
        const std::string &s = row[j];
        if ( false == s.empty() )
          emptyRow = false;
      }

      if ( true == emptyRow )
        emptyRows.push_back ( i );
    }

    // Erase rows from last to first.
    while ( false == emptyRows.empty() )
    {
      t.erase ( t.begin() + emptyRows.back() );
      emptyRows.pop_back();
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove empty columns.
//
///////////////////////////////////////////////////////////////////////////////

namespace Detail
{
  template < class Table > void removeEmptyColumns ( Table &t )
  {
    typedef typename Table::value_type Row;
    Usul::Math::transpose ( t );
    Detail::removeEmptyRows ( t );
    Usul::Math::transpose ( t );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Query available formsts.
//
///////////////////////////////////////////////////////////////////////////////

void PixelFormat::query ( DeviceContext ^dc, PixelFormat::Table &table )
{
  if ( nullptr == dc && 0x0 == dc->hdc() )
    return;

  // Clear the table right away.
  table.clear();

  // Get the number of formats.
  Descriptor pfd;
  ::memset ( &pfd, 0, sizeof ( Descriptor ) );
  const int firstIndex ( 1 );
  const int last ( Detail::describe ( dc->hdc(), firstIndex, pfd ) );
  if ( last <= 0 )
    return;

  // Reserve space.
  table.reserve ( last + 1 );

  // Add column names.
  Detail::addColumnNames ( table );

  // Add rows of values.
  for ( int i = 0; i < last; ++i )
  {
    int index ( i + 1 );
    Detail::describe ( dc->hdc(), index, pfd );
    Detail::addRow ( index, pfd, table );
  }

  // Remove empty columns.
  Detail::removeEmptyColumns ( table );
}




///////////////////////////////////////////////////////////////////////////////
//
//  Query available formsts.
//
///////////////////////////////////////////////////////////////////////////////

PixelFormat::DataTable ^PixelFormat::query ( DeviceContext ^dc )
{
  Table table;
  query ( dc, table );

  System::Data::DataTable ^data = gcnew System::Data::DataTable();

  // Column names.
  if ( false == table.empty() )
  {
    const Row &row = table.front();
    for ( Row::const_iterator i = row.begin(); i != row.end(); ++i )
    {
      System::Data::DataColumn ^column = gcnew System::Data::DataColumn();
      column->DataType = System::Type::GetType( "System.String" );
      column->ColumnName = gcnew System::String ( i->c_str() );
      column->ReadOnly = true;
      data->Columns->Add ( column );
    }
  }

  // Add the rows.
  for ( unsigned int i = 1; i < table.size(); ++i )
  {
    System::Data::DataRow ^row = data->NewRow();
    const Row &r = table.at(i);
    System::Diagnostics::Debug::Assert ( row->ItemArray->Length == r.size() );
    for ( unsigned int j = 0; j < r.size(); ++j )
    {
      row[j] = gcnew System::String ( r.at(j).c_str() );
    }
    data->Rows->Add ( row );
  }

  // Return the table.
  return data;
}
