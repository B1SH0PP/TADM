/********************************************************************************
** Form generated from reading UI file 'Gui.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUI_H
#define UI_GUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GuiClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *GuiClass)
    {
        if (GuiClass->objectName().isEmpty())
            GuiClass->setObjectName(QString::fromUtf8("GuiClass"));
        GuiClass->resize(600, 400);
        menuBar = new QMenuBar(GuiClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        GuiClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(GuiClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        GuiClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(GuiClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        GuiClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(GuiClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        GuiClass->setStatusBar(statusBar);

        retranslateUi(GuiClass);

        QMetaObject::connectSlotsByName(GuiClass);
    } // setupUi

    void retranslateUi(QMainWindow *GuiClass)
    {
        GuiClass->setWindowTitle(QCoreApplication::translate("GuiClass", "Gui", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GuiClass: public Ui_GuiClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_H
