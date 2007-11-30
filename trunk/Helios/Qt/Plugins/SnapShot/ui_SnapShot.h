/********************************************************************************
** Form generated from reading ui file 'SnapShot.ui'
**
** Created: Sat Nov 24 16:55:18 2007
**      by: Qt User Interface Compiler version 4.2.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_SNAPSHOT_H
#define UI_SNAPSHOT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

class Ui_SnapShot
{
public:
    QVBoxLayout *vboxLayout;
    QSpacerItem *spacerItem;
    QHBoxLayout *hboxLayout;
    QLabel *label;
    QDoubleSpinBox *_frameScale;
    QHBoxLayout *hboxLayout1;
    QLabel *label_2;
    QSpinBox *_numSamples;
    QHBoxLayout *hboxLayout2;
    QSpacerItem *spacerItem1;
    QPushButton *_snapShotButton;
    QSpacerItem *spacerItem2;
    QSpacerItem *spacerItem3;

    void setupUi(QWidget *SnapShot)
    {
    SnapShot->setObjectName(QString::fromUtf8("SnapShot"));
    vboxLayout = new QVBoxLayout(SnapShot);
    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
    spacerItem = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem);

    hboxLayout = new QHBoxLayout();
    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
    label = new QLabel(SnapShot);
    label->setObjectName(QString::fromUtf8("label"));
    label->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    hboxLayout->addWidget(label);

    _frameScale = new QDoubleSpinBox(SnapShot);
    _frameScale->setObjectName(QString::fromUtf8("_frameScale"));
    _frameScale->setValue(1);

    hboxLayout->addWidget(_frameScale);


    vboxLayout->addLayout(hboxLayout);

    hboxLayout1 = new QHBoxLayout();
    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
    label_2 = new QLabel(SnapShot);
    label_2->setObjectName(QString::fromUtf8("label_2"));
    label_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

    hboxLayout1->addWidget(label_2);

    _numSamples = new QSpinBox(SnapShot);
    _numSamples->setObjectName(QString::fromUtf8("_numSamples"));
    _numSamples->setMinimum(1);
    _numSamples->setValue(4);

    hboxLayout1->addWidget(_numSamples);


    vboxLayout->addLayout(hboxLayout1);

    hboxLayout2 = new QHBoxLayout();
    hboxLayout2->setObjectName(QString::fromUtf8("hboxLayout2"));
    spacerItem1 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout2->addItem(spacerItem1);

    _snapShotButton = new QPushButton(SnapShot);
    _snapShotButton->setObjectName(QString::fromUtf8("_snapShotButton"));

    hboxLayout2->addWidget(_snapShotButton);

    spacerItem2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    hboxLayout2->addItem(spacerItem2);


    vboxLayout->addLayout(hboxLayout2);

    spacerItem3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    vboxLayout->addItem(spacerItem3);


    retranslateUi(SnapShot);

    QSize size(246, 170);
    size = size.expandedTo(SnapShot->minimumSizeHint());
    SnapShot->resize(size);


    QMetaObject::connectSlotsByName(SnapShot);
    } // setupUi

    void retranslateUi(QWidget *SnapShot)
    {
    SnapShot->setWindowTitle(QApplication::translate("SnapShot", "SnapShot", 0, QApplication::UnicodeUTF8));
    label->setText(QApplication::translate("SnapShot", "Frame Scale", 0, QApplication::UnicodeUTF8));
    label_2->setText(QApplication::translate("SnapShot", "Number of Samples", 0, QApplication::UnicodeUTF8));
    _snapShotButton->setText(QApplication::translate("SnapShot", "Snap Shot", 0, QApplication::UnicodeUTF8));
    Q_UNUSED(SnapShot);
    } // retranslateUi

};

namespace Ui {
    class SnapShot: public Ui_SnapShot {};
} // namespace Ui

#endif // UI_SNAPSHOT_H
