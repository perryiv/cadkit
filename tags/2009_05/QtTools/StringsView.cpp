
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#include "QtTools/StringsView.h"

#include "QtGui/QLineEdit"

using namespace QtTools;

///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

StringsView::StringsView ( QWidget* parent ) : BaseClass ( parent ),
  _model ( new OptionsItemModel ),
  _delegate ( new OptionsItemDelegate )
{  
  this->setModel ( _model );
  this->setItemDelegate ( _delegate );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

StringsView::~StringsView()
{
  delete _model;
  delete _delegate;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add an item.
//
///////////////////////////////////////////////////////////////////////////////

void StringsView::addItem ( const std::string& column0, const std::string& column1 )
{
  if ( 0x0 != _model )
    _model->addRow ( column0, column1 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the current selected row.
//
///////////////////////////////////////////////////////////////////////////////

int StringsView::currentRow() const
{
  QModelIndex index ( this->selectionModel()->currentIndex() );
  return index.row();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Remove a row.
//
///////////////////////////////////////////////////////////////////////////////

void StringsView::removeRow ( int row )
{
  if ( 0x0 != _model )
  {
    _model->removeRow ( row, QModelIndex() );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the options.
//
///////////////////////////////////////////////////////////////////////////////

StringsView::Items StringsView::items() const
{
  Items options;
  
  if ( 0x0 != _model )
  {
    typedef OptionsItemModel::Values Values;
    Values values ( _model->values() );
  
    for ( Values::const_iterator iter = values.begin(); iter != values.end(); ++iter )
    {
      options.push_back ( *iter );
    }
    
  }
  
  return options;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Constructor.
//
///////////////////////////////////////////////////////////////////////////////

StringsView::OptionsItemModel::OptionsItemModel ( QObject *parent ) : BaseClass ( parent ),
  _values()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

StringsView::OptionsItemModel::~OptionsItemModel()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Add a row.
//
///////////////////////////////////////////////////////////////////////////////

void StringsView::OptionsItemModel::addRow ( const std::string& column0, const std::string& column1 )
{
  this->beginInsertRows ( QModelIndex(), this->rowCount(), this->rowCount() );
  _values.push_back ( Value ( column0, column1 ) );
  this->endInsertRows();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the row count.
//
///////////////////////////////////////////////////////////////////////////////

int StringsView::OptionsItemModel::rowCount ( const QModelIndex &parent ) const
{
  return _values.size();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the column count.
//
///////////////////////////////////////////////////////////////////////////////

int StringsView::OptionsItemModel::columnCount ( const QModelIndex &parent ) const
{
  return 2;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the data.
//
///////////////////////////////////////////////////////////////////////////////

QVariant StringsView::OptionsItemModel::data ( const QModelIndex& index, int role ) const
{
  if ( false == index.isValid() )
    return QVariant();
  
  if ( Qt::TextAlignmentRole == role )
    return static_cast<int> ( Qt::AlignRight | Qt::AlignVCenter );
  if ( Qt::DisplayRole == role )
  {
    if ( 0 == index.column() )
      return QString ( _values.at ( index.row() ).first.c_str() );
    if ( 1 == index.column() )
      return QString ( _values.at ( index.row() ).second.c_str() );
  }
  
  return QVariant();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the data.
//
///////////////////////////////////////////////////////////////////////////////

bool StringsView::OptionsItemModel::setData ( const QModelIndex& index, const QVariant& value, int role )
{
  if ( index.isValid() && role == Qt::EditRole )
  {
    // Color.
    if ( 0 == index.column() )
    {
			std::string s ( value.toString().toStdString() );
      _values[index.row()].first = s;
      
			emit dataChanged ( index, index );
      
      return true;
    }
    
    // Value.
    else if ( 1 == index.column() )
    {
      std::string s ( value.toString().toStdString() );
      _values[index.row()].second = s;
      
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

QVariant StringsView::OptionsItemModel::headerData ( int section, Qt::Orientation orientation, int role ) const
{
  if ( Qt::DisplayRole == role )
  {
    if ( 0 == section )
      return "Key";
    if ( 1 == section )
      return "Value";
  }
  
  return QVariant();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the index.
//
///////////////////////////////////////////////////////////////////////////////

QModelIndex StringsView::OptionsItemModel::index ( int row, int column, const QModelIndex & parent ) const
{
  return this->hasIndex ( row, column, parent ) ? this->createIndex ( row, column, 0 ) : QModelIndex();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Get the parent.
//
///////////////////////////////////////////////////////////////////////////////

QModelIndex StringsView::OptionsItemModel::parent ( const QModelIndex & index ) const
{
  return QModelIndex();
}


///////////////////////////////////////////////////////////////////////////////
//
//  Does the ModelIndex have any children.
//
///////////////////////////////////////////////////////////////////////////////

bool StringsView::OptionsItemModel::hasChildren ( const QModelIndex & parent ) const
{
  return parent.isValid() ? false : ( this->rowCount() > 0 );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Return the flags.
//
///////////////////////////////////////////////////////////////////////////////

Qt::ItemFlags StringsView::OptionsItemModel::flags ( const QModelIndex &index ) const
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

bool StringsView::OptionsItemModel::removeRow ( int row, const QModelIndex & parent )
{
  if ( row < static_cast<int> ( _values.size() ) )
  {
    this->beginRemoveRows ( parent, row, row );
    _values.erase ( _values.begin() + row );
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

bool StringsView::OptionsItemModel::insertRow ( int row, const QModelIndex & parent )
{
  if ( row <= static_cast<int> ( _values.size() ) )
  {
    this->beginInsertRows ( parent, row, row );
    _values.insert ( _values.begin() + row, Value ( "key", "value" ) );
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

StringsView::OptionsItemDelegate::OptionsItemDelegate( QObject *parent ) : BaseClass ( parent )
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Destructor.
//
///////////////////////////////////////////////////////////////////////////////

StringsView::OptionsItemDelegate::~OptionsItemDelegate()
{
}


///////////////////////////////////////////////////////////////////////////////
//
//  Create an editor.
//
///////////////////////////////////////////////////////////////////////////////

QWidget * StringsView::OptionsItemDelegate::createEditor ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
  if ( 0 == index.column() || 1 == index.column() )
  {
		return new QLineEdit ( parent );
  }
  
  return BaseClass::createEditor ( parent, option, index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state of the editor.
//
///////////////////////////////////////////////////////////////////////////////

void StringsView::OptionsItemDelegate::setEditorData( QWidget *editor, const QModelIndex &index ) const
{
  if ( 0 == index.column() || 1 == index.column() )
  {
    QString text ( index.model()->data ( index, Qt::DisplayRole ).toString() );
		QLineEdit* lineEdit ( static_cast<QLineEdit*> ( editor ) );
		lineEdit->setText ( text );
  }
  else
    BaseClass::setEditorData ( editor, index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Set the state of the model.
//
///////////////////////////////////////////////////////////////////////////////

void StringsView::OptionsItemDelegate::setModelData ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const
{
	if ( 0 == index.column() || 1 == index.column() )
	{
		QLineEdit* lineEdit ( static_cast<QLineEdit*> ( editor ) );
		model->setData ( index, lineEdit->text(), Qt::EditRole );
	}
	else
		BaseClass::setModelData ( editor, model, index );
}


///////////////////////////////////////////////////////////////////////////////
//
//  Paint.
//
///////////////////////////////////////////////////////////////////////////////

void StringsView::OptionsItemDelegate::paint ( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
  BaseClass::paint ( painter, option, index );
}
