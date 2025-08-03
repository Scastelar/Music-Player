#include "homewindow.h"
#include "ui_homewindow.h"

HomeWindow::HomeWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::HomeWindow)
{
    ui->setupUi(this);

    ui->verticalFrame->setFixedWidth(200);

    this->setStyleSheet(R"(
    QWidget {
        background-color: rgb(40,40,40);
        color: white;
        font-family: 'Arial';
    }
    QToolButton {
        border: none;
    }
    QToolButton:hover {
        background-color: #DDDDDD;
        border-radius: 10px;
    }
    QLineEdit {
        border: 1px solid #CCC;
        border-radius: 8px;
        padding: 5px 10px;
        background-color: white;
    }
)");


}

HomeWindow::~HomeWindow()
{
    delete ui;
}
