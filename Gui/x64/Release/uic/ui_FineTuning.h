/********************************************************************************
** Form generated from reading UI file 'FineTuning.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FINETUNING_H
#define UI_FINETUNING_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FineTuningClass
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QToolButton *AddMask;
    QToolButton *RemoveMask;
    QSpacerItem *verticalSpacer;
    QWidget *FineTuningWidget;
    QVBoxLayout *verticalLayout;

    void setupUi(QWidget *FineTuningClass)
    {
        if (FineTuningClass->objectName().isEmpty())
            FineTuningClass->setObjectName(QString::fromUtf8("FineTuningClass"));
        FineTuningClass->resize(600, 400);
        horizontalLayout = new QHBoxLayout(FineTuningClass);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(1, 1, 1, 1);
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        AddMask = new QToolButton(FineTuningClass);
        AddMask->setObjectName(QString::fromUtf8("AddMask"));

        verticalLayout_2->addWidget(AddMask);

        RemoveMask = new QToolButton(FineTuningClass);
        RemoveMask->setObjectName(QString::fromUtf8("RemoveMask"));

        verticalLayout_2->addWidget(RemoveMask);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout_2);

        FineTuningWidget = new QWidget(FineTuningClass);
        FineTuningWidget->setObjectName(QString::fromUtf8("FineTuningWidget"));
        verticalLayout = new QVBoxLayout(FineTuningWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);

        horizontalLayout->addWidget(FineTuningWidget);

        horizontalLayout->setStretch(0, 1);
        horizontalLayout->setStretch(1, 10);

        retranslateUi(FineTuningClass);

        QMetaObject::connectSlotsByName(FineTuningClass);
    } // setupUi

    void retranslateUi(QWidget *FineTuningClass)
    {
        FineTuningClass->setWindowTitle(QCoreApplication::translate("FineTuningClass", "FineTuning", nullptr));
        AddMask->setText(QCoreApplication::translate("FineTuningClass", "Add Mask", nullptr));
#if QT_CONFIG(shortcut)
        AddMask->setShortcut(QCoreApplication::translate("FineTuningClass", "1", nullptr));
#endif // QT_CONFIG(shortcut)
        RemoveMask->setText(QCoreApplication::translate("FineTuningClass", "Remove Mask", nullptr));
#if QT_CONFIG(shortcut)
        RemoveMask->setShortcut(QCoreApplication::translate("FineTuningClass", "2", nullptr));
#endif // QT_CONFIG(shortcut)
    } // retranslateUi

};

namespace Ui {
    class FineTuningClass: public Ui_FineTuningClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FINETUNING_H
