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
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QSlider>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GUIClass
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_1;
    QToolButton *OpenButton;
    QToolButton *SaveButton;
    QToolButton *LeftButton;
    QToolButton *RightButton;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QWidget *viewBox_1;
    QVBoxLayout *verticalLayout_3;
    QTableWidget *tableWidget;
    QHBoxLayout *horizontalLayout_3;
    QFormLayout *formLayout;
    QToolButton *ConvexHullButton;
    QToolButton *FineTuningButton;
    QToolButton *ExportButton;
    QSpacerItem *horizontalSpacer_2;
    QGridLayout *gridLayout;
    QSlider *CannyHighThresholdSlider;
    QSlider *CannyLowThresholdSlider;
    QLabel *CannyLowThresholdLabel;
    QLabel *CannyHighThresholdLabel;
    QHBoxLayout *horizontalLayout_4;
    QWidget *viewBox_2;
    QVBoxLayout *verticalLayout_2;
    QWidget *viewBox_3;
    QVBoxLayout *verticalLayout_4;
    QMenuBar *menuBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *GUIClass)
    {
        if (GUIClass->objectName().isEmpty())
            GUIClass->setObjectName(QString::fromUtf8("GUIClass"));
        GUIClass->resize(964, 798);
        centralWidget = new QWidget(GUIClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setSpacing(2);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(9, -1, -1, -1);
        horizontalLayout_1 = new QHBoxLayout();
        horizontalLayout_1->setSpacing(0);
        horizontalLayout_1->setObjectName(QString::fromUtf8("horizontalLayout_1"));
        horizontalLayout_1->setSizeConstraint(QLayout::SetDefaultConstraint);
        OpenButton = new QToolButton(centralWidget);
        OpenButton->setObjectName(QString::fromUtf8("OpenButton"));
        QFont font;
        font.setPointSize(15);
        OpenButton->setFont(font);

        horizontalLayout_1->addWidget(OpenButton);

        SaveButton = new QToolButton(centralWidget);
        SaveButton->setObjectName(QString::fromUtf8("SaveButton"));
        SaveButton->setFont(font);

        horizontalLayout_1->addWidget(SaveButton);

        LeftButton = new QToolButton(centralWidget);
        LeftButton->setObjectName(QString::fromUtf8("LeftButton"));
        LeftButton->setFont(font);

        horizontalLayout_1->addWidget(LeftButton);

        RightButton = new QToolButton(centralWidget);
        RightButton->setObjectName(QString::fromUtf8("RightButton"));
        RightButton->setFont(font);

        horizontalLayout_1->addWidget(RightButton);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_1->addItem(horizontalSpacer);

        horizontalLayout_1->setStretch(0, 2);
        horizontalLayout_1->setStretch(1, 2);
        horizontalLayout_1->setStretch(2, 1);
        horizontalLayout_1->setStretch(3, 1);

        verticalLayout->addLayout(horizontalLayout_1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        viewBox_1 = new QWidget(centralWidget);
        viewBox_1->setObjectName(QString::fromUtf8("viewBox_1"));
        verticalLayout_3 = new QVBoxLayout(viewBox_1);
        verticalLayout_3->setSpacing(2);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        verticalLayout_3->setContentsMargins(1, 1, 1, 1);

        horizontalLayout_2->addWidget(viewBox_1);

        tableWidget = new QTableWidget(centralWidget);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));

        horizontalLayout_2->addWidget(tableWidget);

        horizontalLayout_2->setStretch(0, 1);

        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setSizeConstraint(QLayout::SetDefaultConstraint);
        formLayout = new QFormLayout();
        formLayout->setSpacing(6);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        ConvexHullButton = new QToolButton(centralWidget);
        ConvexHullButton->setObjectName(QString::fromUtf8("ConvexHullButton"));
        ConvexHullButton->setFont(font);

        formLayout->setWidget(0, QFormLayout::LabelRole, ConvexHullButton);

        FineTuningButton = new QToolButton(centralWidget);
        FineTuningButton->setObjectName(QString::fromUtf8("FineTuningButton"));
        FineTuningButton->setFont(font);

        formLayout->setWidget(0, QFormLayout::FieldRole, FineTuningButton);

        ExportButton = new QToolButton(centralWidget);
        ExportButton->setObjectName(QString::fromUtf8("ExportButton"));
        ExportButton->setFont(font);

        formLayout->setWidget(1, QFormLayout::LabelRole, ExportButton);


        horizontalLayout_3->addLayout(formLayout);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        CannyHighThresholdSlider = new QSlider(centralWidget);
        CannyHighThresholdSlider->setObjectName(QString::fromUtf8("CannyHighThresholdSlider"));
        CannyHighThresholdSlider->setOrientation(Qt::Horizontal);
        CannyHighThresholdSlider->setTickPosition(QSlider::TicksBothSides);
        CannyHighThresholdSlider->setTickInterval(10);

        gridLayout->addWidget(CannyHighThresholdSlider, 1, 0, 1, 1);

        CannyLowThresholdSlider = new QSlider(centralWidget);
        CannyLowThresholdSlider->setObjectName(QString::fromUtf8("CannyLowThresholdSlider"));
        QFont font1;
        font1.setPointSize(16);
        CannyLowThresholdSlider->setFont(font1);
        CannyLowThresholdSlider->setSingleStep(10);
        CannyLowThresholdSlider->setOrientation(Qt::Horizontal);
        CannyLowThresholdSlider->setTickPosition(QSlider::TicksBothSides);
        CannyLowThresholdSlider->setTickInterval(10);

        gridLayout->addWidget(CannyLowThresholdSlider, 0, 0, 1, 1);

        CannyLowThresholdLabel = new QLabel(centralWidget);
        CannyLowThresholdLabel->setObjectName(QString::fromUtf8("CannyLowThresholdLabel"));

        gridLayout->addWidget(CannyLowThresholdLabel, 0, 1, 1, 1);

        CannyHighThresholdLabel = new QLabel(centralWidget);
        CannyHighThresholdLabel->setObjectName(QString::fromUtf8("CannyHighThresholdLabel"));

        gridLayout->addWidget(CannyHighThresholdLabel, 1, 1, 1, 1);


        horizontalLayout_3->addLayout(gridLayout);

        horizontalLayout_3->setStretch(0, 1);
        horizontalLayout_3->setStretch(2, 5);

        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        viewBox_2 = new QWidget(centralWidget);
        viewBox_2->setObjectName(QString::fromUtf8("viewBox_2"));
        verticalLayout_2 = new QVBoxLayout(viewBox_2);
        verticalLayout_2->setSpacing(2);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(1, 1, 1, 1);

        horizontalLayout_4->addWidget(viewBox_2);

        viewBox_3 = new QWidget(centralWidget);
        viewBox_3->setObjectName(QString::fromUtf8("viewBox_3"));
        verticalLayout_4 = new QVBoxLayout(viewBox_3);
        verticalLayout_4->setSpacing(2);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        verticalLayout_4->setContentsMargins(1, 1, 1, 1);

        horizontalLayout_4->addWidget(viewBox_3);


        verticalLayout->addLayout(horizontalLayout_4);

        verticalLayout->setStretch(0, 1);
        verticalLayout->setStretch(1, 1);
        verticalLayout->setStretch(2, 1);
        verticalLayout->setStretch(3, 1);
        GUIClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(GUIClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 964, 22));
        GUIClass->setMenuBar(menuBar);
        statusBar = new QStatusBar(GUIClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        GUIClass->setStatusBar(statusBar);

        retranslateUi(GUIClass);

        QMetaObject::connectSlotsByName(GUIClass);
    } // setupUi

    void retranslateUi(QMainWindow *GUIClass)
    {
        GUIClass->setWindowTitle(QCoreApplication::translate("GUIClass", "GUI", nullptr));
        OpenButton->setText(QCoreApplication::translate("GUIClass", "Open(W)", nullptr));
#if QT_CONFIG(shortcut)
        OpenButton->setShortcut(QCoreApplication::translate("GUIClass", "W", nullptr));
#endif // QT_CONFIG(shortcut)
        SaveButton->setText(QCoreApplication::translate("GUIClass", "Save(S)", nullptr));
#if QT_CONFIG(shortcut)
        SaveButton->setShortcut(QCoreApplication::translate("GUIClass", "S", nullptr));
#endif // QT_CONFIG(shortcut)
        LeftButton->setText(QCoreApplication::translate("GUIClass", "<", nullptr));
#if QT_CONFIG(shortcut)
        LeftButton->setShortcut(QCoreApplication::translate("GUIClass", "Left", nullptr));
#endif // QT_CONFIG(shortcut)
        RightButton->setText(QCoreApplication::translate("GUIClass", ">", nullptr));
#if QT_CONFIG(shortcut)
        RightButton->setShortcut(QCoreApplication::translate("GUIClass", "Right", nullptr));
#endif // QT_CONFIG(shortcut)
        ConvexHullButton->setText(QCoreApplication::translate("GUIClass", "Covex Hull(Z)", nullptr));
#if QT_CONFIG(shortcut)
        ConvexHullButton->setShortcut(QCoreApplication::translate("GUIClass", "Z", nullptr));
#endif // QT_CONFIG(shortcut)
        FineTuningButton->setText(QCoreApplication::translate("GUIClass", "Fine-tuning", nullptr));
#if QT_CONFIG(shortcut)
        FineTuningButton->setShortcut(QCoreApplication::translate("GUIClass", "X", nullptr));
#endif // QT_CONFIG(shortcut)
        ExportButton->setText(QCoreApplication::translate("GUIClass", "Export(Enter)", nullptr));
#if QT_CONFIG(shortcut)
        ExportButton->setShortcut(QCoreApplication::translate("GUIClass", "Return", nullptr));
#endif // QT_CONFIG(shortcut)
        CannyLowThresholdLabel->setText(QCoreApplication::translate("GUIClass", "CannyLowThreshold = ?", nullptr));
        CannyHighThresholdLabel->setText(QCoreApplication::translate("GUIClass", "CannyHighThreshold = ?", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GUIClass: public Ui_GUIClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_H
