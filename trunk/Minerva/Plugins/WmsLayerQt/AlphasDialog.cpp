
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "Minerva/Plugins/WmsLayerQt/AlphasDialog.h"

#include "Usul/Functions/Color.h"
#include "Usul/Math/Vector4.h"
#include "Usul/Strings/Format.h"

#include "QtTools/ColorButton.h"

#include "QtGui/QLabel"
#include "QtGui/QSpinBox"
#include "QtGui/QPainter"

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AlphasDialog::AlphasDialog ( const Alphas& alphas, QWidget *parent ) : BaseClass ( parent ),
  _alphas ( alphas ),
  _model ( new AlphasItemModel ( alphas ) ),
  _delegate ( new AlphasItemDelegate )
{
  this->setupUi ( this );
  
  this->setWindowTitle ( "Edit Alphas" );
  
  _alphasTreeView->setModel ( _model );
  _alphasTreeView->setItemDelegate ( _delegate );
  //_alphasTreeView->setItemDelegateForColumn( 0, _delegate );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AlphasDialog::~AlphasDialog()
{
  delete _model;
  delete _delegate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the extra alphas.
//
///////////////////////////////////////////////////////////////////////////////

AlphasDialog::Alphas AlphasDialog::alphas() const
{
  AlphasItemModel::Opacities opacities ( _model->opacities() );
  AlphasItemModel::Colors colors ( _model->colors() );
  
  Alphas alphas;
  
  for ( unsigned int i = 0; i < colors.size(); ++i )
  {
    QColor color ( colors.at ( i ) );
    alphas[Usul::Functions::Color::pack ( color.red(), color.green(), color.red(), 0 )] = opacities.at ( i );
  }
  
  return alphas;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a row.
//
///////////////////////////////////////////////////////////////////////////////

void AlphasDialog::on_addRowButton_clicked()
{
  QModelIndex index;
  _model->insertRow ( _model->rowCount(), index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a row.
//
///////////////////////////////////////////////////////////////////////////////

void AlphasDialog::on_removeRowButton_clicked()
{
  QModelIndex index ( _alphasTreeView->selectionModel()->currentIndex() );
  _model->removeRow ( index.row(), index.parent() );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AlphasDialog::AlphasItemModel::AlphasItemModel ( const Alphas& alphas, QObject *parent ) : BaseClass ( parent ),
  _colors(),
  _opacities()
{
  for ( Alphas::const_iterator iter = alphas.begin(); iter != alphas.end(); ++iter )
  {
    Usul::Math::Vec4uc color ( Usul::Functions::Color::unpack<Usul::Math::Vec4uc> ( iter->first ) );

    _colors.push_back ( QColor ( color[0], color[1], color[2], 255 ) );
    _opacities.push_back ( iter->second );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AlphasDialog::AlphasItemModel::~AlphasItemModel()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the row count.
//
///////////////////////////////////////////////////////////////////////////////

int AlphasDialog::AlphasItemModel::rowCount ( const QModelIndex &parent ) const
{
  return _colors.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the column count.
//
///////////////////////////////////////////////////////////////////////////////

int AlphasDialog::AlphasItemModel::columnCount ( const QModelIndex &parent ) const
{
  return 2;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data.
//
///////////////////////////////////////////////////////////////////////////////

QVariant AlphasDialog::AlphasItemModel::data ( const QModelIndex& index, int role ) const
{
  if ( false == index.isValid() )
    return QVariant();
  
  if ( Qt::TextAlignmentRole == role )
    return static_cast<int> ( Qt::AlignRight | Qt::AlignVCenter );
  if ( Qt::DisplayRole == role )
  {
    if ( 0 == index.column() )
      return _colors.at ( index.row() );
    if ( 1 == index.column() )
      return _opacities.at ( index.row() );
  }
  
  return QVariant();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool AlphasDialog::AlphasItemModel::setData ( const QModelIndex& index, const QVariant& value, int role )
{
  if ( index.isValid() && role == Qt::EditRole )
  {
    // Color.
    if ( 0 == index.column() )
    {
			QColor color ( value.value<QColor>() );
			_colors[index.row()] = color;

			emit dataChanged ( index, index );
      
      return true;
    }
    
    // Alpha.
    else if ( 1 == index.column() )
    {
      unsigned short alpha ( value.toUInt() );
      _opacities[index.row()] = alpha;
      
      emit dataChanged ( index, index );
      
      return true;
    }
  }
  
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the header data.
//
///////////////////////////////////////////////////////////////////////////////

QVariant AlphasDialog::AlphasItemModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
  if ( Qt::DisplayRole == role )
  {
    if ( 0 == section )
      return "Color";
    if ( 1 == section )
      return "Alpha";
  }
  
  return QVariant();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the index.
//
///////////////////////////////////////////////////////////////////////////////

QModelIndex AlphasDialog::AlphasItemModel::index ( int row, int column, const QModelIndex & parent ) const
{
  return this->hasIndex ( row, column, parent ) ? this->createIndex ( row, column, 0 ) : QModelIndex();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the parent.
//
///////////////////////////////////////////////////////////////////////////////

QModelIndex AlphasDialog::AlphasItemModel::parent ( const QModelIndex & index ) const
{
  return QModelIndex();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the ModelIndex have any children.
//
///////////////////////////////////////////////////////////////////////////////

bool AlphasDialog::AlphasItemModel::hasChildren ( const QModelIndex & parent ) const
{
  return parent.isValid() ? false : ( this->rowCount() > 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the flags.
//
///////////////////////////////////////////////////////////////////////////////

Qt::ItemFlags AlphasDialog::AlphasItemModel::flags ( const QModelIndex &index ) const
{
  Qt::ItemFlags flags ( BaseClass::flags ( index ) );
  
  if ( index.isValid() )
    flags |= Qt::ItemIsEditable;
  
  return flags;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove the row.
//
///////////////////////////////////////////////////////////////////////////////

bool AlphasDialog::AlphasItemModel::removeRow ( int row, const QModelIndex & parent )
{
  if ( row < static_cast<int> ( _colors.size() ) && row < static_cast<int> ( _opacities.size() ) )
  {
    this->beginRemoveRows ( parent, row, row );
    _colors.erase ( _colors.begin() + row );
    _opacities.erase ( _opacities.begin() + row );
    this->endRemoveRows();
    
    return true;
  }
  
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Insert a row.
//
///////////////////////////////////////////////////////////////////////////////

bool AlphasDialog::AlphasItemModel::insertRow ( int row, const QModelIndex & parent )
{
  if ( row <= static_cast<int> ( _colors.size() ) && row <= static_cast<int> ( _opacities.size() ) )
  {
    this->beginInsertRows ( parent, row, row );
    _colors.insert ( _colors.begin() + row, Usul::Functions::Color::pack ( 255, 255, 255, 0 ) );
    _opacities.insert ( _opacities.begin() + row, 255 );
    this->endInsertRows();
    
    return true;
  }
  
  return false;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

AlphasDialog::AlphasItemDelegate::AlphasItemDelegate( QObject *parent ) : BaseClass ( parent )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

AlphasDialog::AlphasItemDelegate::~AlphasItemDelegate()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create an editor.
//
///////////////////////////////////////////////////////////////////////////////

QWidget * AlphasDialog::AlphasItemDelegate::createEditor ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
  if ( 0 == index.column() )
  {
    //return new QLabel ( parent );
		return new QtTools::ColorButton ( parent );
  }
  if ( 1 == index.column() )
  {
    QSpinBox *spinBox ( new QSpinBox ( parent ) );
    spinBox->setRange( 0, 255 );
    return spinBox;
  }
  
  return BaseClass::createEditor ( parent, option, index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state of the editor.
//
///////////////////////////////////////////////////////////////////////////////

void AlphasDialog::AlphasItemDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const
{
  if ( 0 == index.column() )
  {
    QColor color ( index.model()->data ( index, Qt::DisplayRole ).value<QColor>() );
		QtTools::ColorButton* button ( static_cast<QtTools::ColorButton*> ( editor ) );
		button->color ( color );
  }
  else if ( 1 == index.column() )
  {
    unsigned short alpha ( index.model()->data ( index, Qt::DisplayRole ).toUInt() );
    QSpinBox *spinBox ( static_cast< QSpinBox* > ( editor ) );
    spinBox->setValue ( alpha );
  }
  
  else
    BaseClass::setEditorData ( editor, index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state of the model.
//
///////////////////////////////////////////////////////////////////////////////

void AlphasDialog::AlphasItemDelegate::setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
  BaseClass::setModelData ( editor, model, index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Paint.
//
///////////////////////////////////////////////////////////////////////////////

void AlphasDialog::AlphasItemDelegate::paint ( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
#if 0
  if ( 0x0 != painter, 0 == index.column() )
  {
    painter->fillRect ( option.rect, QColor ( 10 * index.row(), 0, 0, 255 ) );
  }
  
  else
#endif
    BaseClass::paint ( painter, option, index );
}
