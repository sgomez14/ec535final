/********************************************************************************
** Form generated from reading UI file 'rfidgui.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RFIDGUI_H
#define UI_RFIDGUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RFIDGui
{
public:
    QWidget *centralwidget;
    QLabel *scanned_item_label;
    QLabel *mode_label;
    QLineEdit *item_message;
    QLineEdit *mode_message;
    QPushButton *pushButton;
    QLabel *enter_item_label;
    QLineEdit *enter_item_msg;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *RFIDGui)
    {
        if (RFIDGui->objectName().isEmpty())
            RFIDGui->setObjectName(QString::fromUtf8("RFIDGui"));
        RFIDGui->resize(480, 272);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(RFIDGui->sizePolicy().hasHeightForWidth());
        RFIDGui->setSizePolicy(sizePolicy);
        centralwidget = new QWidget(RFIDGui);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        scanned_item_label = new QLabel(centralwidget);
        scanned_item_label->setObjectName(QString::fromUtf8("scanned_item_label"));
        scanned_item_label->setGeometry(QRect(30, 60, 101, 31));
        QFont font;
        font.setPointSize(10);
        scanned_item_label->setFont(font);
        mode_label = new QLabel(centralwidget);
        mode_label->setObjectName(QString::fromUtf8("mode_label"));
        mode_label->setGeometry(QRect(30, 10, 101, 31));
        mode_label->setFont(font);
        item_message = new QLineEdit(centralwidget);
        item_message->setObjectName(QString::fromUtf8("item_message"));
        item_message->setGeometry(QRect(150, 60, 301, 41));
        item_message->setReadOnly(true);
        mode_message = new QLineEdit(centralwidget);
        mode_message->setObjectName(QString::fromUtf8("mode_message"));
        mode_message->setGeometry(QRect(150, 10, 301, 41));
        QFont font1;
        font1.setPointSize(12);
        mode_message->setFont(font1);
        mode_message->setReadOnly(true);
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName(QString::fromUtf8("pushButton"));
        pushButton->setGeometry(QRect(180, 170, 221, 61));
        pushButton->setFont(font1);
        enter_item_label = new QLabel(centralwidget);
        enter_item_label->setObjectName(QString::fromUtf8("enter_item_label"));
        enter_item_label->setGeometry(QRect(10, 110, 141, 41));
        enter_item_label->setFont(font);
        enter_item_msg = new QLineEdit(centralwidget);
        enter_item_msg->setObjectName(QString::fromUtf8("enter_item_msg"));
        enter_item_msg->setGeometry(QRect(150, 110, 301, 41));
        enter_item_msg->setFont(font);
        enter_item_msg->setReadOnly(false);
        RFIDGui->setCentralWidget(centralwidget);
        menubar = new QMenuBar(RFIDGui);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 480, 21));
        RFIDGui->setMenuBar(menubar);
        statusbar = new QStatusBar(RFIDGui);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        RFIDGui->setStatusBar(statusbar);

        retranslateUi(RFIDGui);

        QMetaObject::connectSlotsByName(RFIDGui);
    } // setupUi

    void retranslateUi(QMainWindow *RFIDGui)
    {
        RFIDGui->setWindowTitle(QCoreApplication::translate("RFIDGui", "RFIDGui", nullptr));
        scanned_item_label->setText(QCoreApplication::translate("RFIDGui", "Scanned Item", nullptr));
        mode_label->setText(QCoreApplication::translate("RFIDGui", "Current Mode", nullptr));
        pushButton->setText(QCoreApplication::translate("RFIDGui", "Change Mode", nullptr));
        enter_item_label->setText(QCoreApplication::translate("RFIDGui", "Enter Item Message", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RFIDGui: public Ui_RFIDGui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RFIDGUI_H
