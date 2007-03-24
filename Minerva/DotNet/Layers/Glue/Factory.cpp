
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2006, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Created by: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Factory.h"
#include "LineLayerGlue.h"
#include "PointLayerGlue.h"
#include "PointTimeLayerGlue.h"
#include "PolygonLayerGlue.h"
#include "PolygonTimeLayerGlue.h"
#include "RLayerGlue.h"
#include "SingleColorFunctor.h"
#include "GradientColorFunctor.h"
#include "Connection.h"

#include "Minerva/Core/DB/Info.h"
#include "Minerva/Core/Layers/LineLayer.h"

#include "Usul/Strings/ManagedToNative.h"

using namespace DT::Minerva::Glue;

CadKit::Interfaces::ILayer^ Factory::create( System::IntPtr pointer )
{
  ::Minerva::Core::Layers::Layer::RefPtr layer ( reinterpret_cast < ::Minerva::Core::Layers::Layer * > ( pointer.ToPointer() ) );

  if( layer.valid() )
  {
    if( ::Minerva::Core::Layers::LineLayer* lineLayer = dynamic_cast < ::Minerva::Core::Layers::LineLayer* > ( layer.get() ) )
      return gcnew LineLayerGlue( lineLayer );
    if( ::Minerva::Core::Layers::PointLayer* pointLayer = dynamic_cast < ::Minerva::Core::Layers::PointLayer* > ( layer.get() ) )
      return gcnew PointLayerGlue( pointLayer );
    if( ::Minerva::Core::Layers::PointTimeLayer* pointTimeLayer = dynamic_cast < ::Minerva::Core::Layers::PointTimeLayer* > ( layer.get() ) )
      return gcnew PointTimeLayerGlue( pointTimeLayer );
    if( ::Minerva::Core::Layers::PolygonLayer* polygonLayer = dynamic_cast < ::Minerva::Core::Layers::PolygonLayer* > ( layer.get() ) )
      return gcnew PolygonLayerGlue( polygonLayer );
    if( ::Minerva::Core::Layers::PolygonTimeLayer* polygonTimeLayer = dynamic_cast < ::Minerva::Core::Layers::PolygonTimeLayer* > ( layer.get() ) )
      return gcnew PolygonTimeLayerGlue ( polygonTimeLayer );
    if( ::Minerva::Core::Layers::RLayer* rLayer = dynamic_cast < ::Minerva::Core::Layers::RLayer* > ( layer.get() ) )
      return gcnew RLayerGlue ( rLayer );
  }

  return nullptr;
}

CadKit::Interfaces::ILayer^ Factory::create( System::String^ datatable, DT::Minerva::Interfaces::IDatabaseConnection^ connection )
{
  try
  {
    if( nullptr != datatable && nullptr != connection )
    {
      DT::Minerva::Glue::Connection^ c = safe_cast < DT::Minerva::Glue::Connection^ > ( connection );
      if( nullptr != c )
      {
        typedef ::Minerva::Core::DB::Info DbInfo;
        DbInfo::RefPtr info ( new DbInfo ( c->nativeConnection() ) );
        std::string table ( Usul::Strings::convert ( datatable ) );

        DT::Minerva::Glue::LayerGlue ^layer = nullptr;
        if( info->isPointTimeTable ( table ) )
          layer = gcnew PointTimeLayerGlue();
        else if ( info->isPolygonTimeTable ( table ) )
          layer = gcnew PolygonTimeLayerGlue ();
        else if ( info->isPointTable ( table ) )
          layer = gcnew PointLayerGlue();
        else if ( info->isLineTable ( table ) )
          layer = gcnew LineLayerGlue();
        else if ( info->isPolygonTable ( table ) )
          layer = gcnew PolygonLayerGlue();

        if( nullptr != layer )
        {
          layer->DataSource = connection;
          layer->Tablename = datatable;

          return layer;
        }
      }
    }
  }
  catch( System::Exception^ e )
  {
    System::Console::WriteLine( "Error 255989532: {0}", e->Message );
  }

  return nullptr;
}


BaseColorFunctor ^ Factory::createColorFunctor ( ::Minerva::Core::Functors::BaseColorFunctor * nativePtr )
{
  if( 0x0 != nativePtr )
  {
    if ( ::Minerva::Core::Functors::SingleColorFunctor *single = dynamic_cast < ::Minerva::Core::Functors::SingleColorFunctor * > ( nativePtr ) )
      return gcnew SingleColorFunctor ( single );
    if( ::Minerva::Core::Functors::GradientColorFunctor *gradient = dynamic_cast < ::Minerva::Core::Functors::GradientColorFunctor* > ( nativePtr ) )
      return gcnew GradientColorFunctor ( gradient );
  }

  return nullptr;
}
