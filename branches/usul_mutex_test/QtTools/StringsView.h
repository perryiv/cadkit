
///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2008, Arizona State University
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//  Author: Adam Kubach
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __QT_TOOLS_STRINGS_VIEW_H__
#define __QT_TOOLS_STRINGS_VIEW_H__

#include "QtTools/Export.h"

#include "QtGui/QTreeView"
#include "QtGui/QItemDelegate"

#include <map>
#include <string>

namespace QtTools {

class QT_TOOLS_EXPORT StringsView : public QTreeView
{
public:
  typedef QTreeView BaseClass;
  typedef std::vector<std::pair<std::string, std::string> > Items;
  
  StringsView ( QWidget *parent = 0x0 );
  virtual ~StringsView();

  /// Add an item.
  void     addItem ( const std::string& column0, const std::string& column1 );

  /// Get the current selected row.
  int      currentRow() const;

  /// Get all the items.
  Items    items() const;

  /// Remove a row.
  void     removeRow ( int );
  
private:
  
  // Custom item model.
  class OptionsItemModel : public QAbstractItemModel
  {
  public:
    typedef QAbstractItemModel BaseClass;
    typedef std::pair<std::string,std::string> Value;
    typedef std::vector<Value> Values;
    
    OptionsItemModel ( QObject *parent = 0x0 );
    virtual ~OptionsItemModel();
    
    Values values() const { return _values; }

    // Add a row to the end.
    void  addRow ( const std::string& column0, const std::string& column1 );
    
    virtual QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    
    virtual Qt::ItemFlags flags ( const QModelIndex &index ) const;
    
    virtual int rowCount ( const QModelIndex &parent = QModelIndex() ) const;
    virtual int columnCount ( const QModelIndex &parent = QModelIndex() ) const;
    
    virtual QVariant data ( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    virtual bool     setData ( const QModelIndex& index, const QVariant& value, int role );
    
    virtual bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    
    virtual QModelIndex parent ( const QModelIndex & index ) const;
    
    virtual bool insertRow ( int row, const QModelIndex & parent = QModelIndex() );
    virtual bool removeRow ( int row, const QModelIndex & parent = QModelIndex() );
    
  private:
    Values _values;
  };
  
  // Custom item delegate.
  class OptionsItemDelegate : public QItemDelegate
  {
  public:
    typedef QItemDelegate BaseClass;
    
    OptionsItemDelegate( QObject *parent = 0x0 );
    virtual ~OptionsItemDelegate();
    
    virtual QWidget *createEditor  ( QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const;
    virtual void     setEditorData ( QWidget *editor, const QModelIndex &index ) const;
    virtual void     setModelData  ( QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const;
    virtual void     paint ( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  };
  
  OptionsItemModel *_model;
  QAbstractItemDelegate *_delegate;
};

}

#endif // __QT_TOOLS_STRING_VIEW_H__
