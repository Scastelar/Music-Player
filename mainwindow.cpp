#include "mainwindow.h"
#include "homewindow.h"
#include "artistawindow.h"
#include "ui_mainwindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

QString rutaImagen="";

MainWindow::MainWindow(QWidget *parent, Cuentas& manejo)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , manejo(&manejo) // Guardamos la dirección de manejo que nos pasó main()
{
    ui->setupUi(this);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initPopup(){
    QDialog* popup = new QDialog();
    popup->setWindowTitle("Tipo de Registro");
    popup->setFixedSize(300, 150);
    popup->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    QVBoxLayout* layout = new QVBoxLayout(popup);

    QLabel* mensaje = new QLabel("¿Que tipo de cuenta quieres registrar?", popup);
    mensaje->setAlignment(Qt::AlignCenter);

    QHBoxLayout* botonesLayout = new QHBoxLayout();
    QPushButton* btnSi = new QPushButton("Estandar", popup);
    QPushButton* btnNo = new QPushButton("Administrativo", popup);

    botonesLayout->addWidget(btnSi);
    botonesLayout->addWidget(btnNo);

    layout->addWidget(mensaje);
    layout->addLayout(botonesLayout);

    popup->setStyleSheet(
        "QDialog {"
        "  background-color: #ffffff;"
        "  border-radius: 15px;"
        "}"
        "QLabel {"
        "  color: #333;"
        "  font-size: 14px;"
        "}"
        "QPushButton {"
        "  background-color: #333;"
        "  color: white;"
        "  border: none;"
        "  padding: 6px 12px;"
        "  border-radius: 6px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #233;"
        "}"
        );

    connect(btnSi, &QPushButton::clicked, [popup]() {
        // Acción para "Sí"
        qDebug() << "Usuario eligió estandar";
        popup->accept(); // o popup->close();
    });

    connect(btnNo, &QPushButton::clicked, [popup]() {
        // Acción para "No"
        qDebug() << "Usuario eligió administrativo";
        popup->reject(); // o popup->close();
    });

    int respuesta = popup->exec();

    if (respuesta == QDialog::Accepted) {
        ui->stackedWidget->setCurrentIndex(1);
    } else {
        ui->stackedWidget->setCurrentIndex(2);
    }

    ui->lineEdit_3->setEchoMode(QLineEdit::Password);
    ui->lineEdit_7->setEchoMode(QLineEdit::Password);
    isCensurada = true;
}

void MainWindow::resetearCampos(){

    rutaImagen = "";
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_5->clear();
    ui->lineEdit_6->clear();
    ui->lineEdit_7->clear();
    ui->lineEdit_8->clear();
    ui->textEdit_2->clear();
    ui->imgLbl->clear();
    ui->imgLbl_2->clear();
    ui->logLbl->clear();
    ui->logLbl2->clear();
    isCensurada = true;
}

void MainWindow::on_regButton_clicked()
{
    initPopup();
}

void MainWindow::on_loginButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_loadButton1_clicked()
{
    rutaImagen = QFileDialog::getOpenFileName(this, "Seleccionar imagen de perfil", "/Users/compu/Pictures/archivos", "Imagenes (*.jpg *.jpeg *.png)");

    if (!rutaImagen.isEmpty()) {
        QPixmap avatar(rutaImagen);

        // Escalar al tamaño del QLabel
        QPixmap avatarEscalado = avatar.scaled(
            ui->imgLbl->size(),
            //Qt::KeepAspectRatio,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation   // para que no se vea pixelado
            );

        ui->imgLbl->setPixmap(avatarEscalado);
    }
}


void MainWindow::on_loadButton2_clicked()
{
    rutaImagen = QFileDialog::getOpenFileName(this, "Seleccionar imagen de perfil", "/Users/compu/Pictures/archivos", "Imagenes (*.jpg *.jpeg *.png)");

    if (!rutaImagen.isEmpty()) {
        QPixmap avatar(rutaImagen);

        // Escalar al tamaño del QLabel
        QPixmap avatarEscalado = avatar.scaled(
            ui->imgLbl_2->size(),
            //Qt::KeepAspectRatio,
            Qt::IgnoreAspectRatio,
            Qt::SmoothTransformation   // para que no se vea pixelado
            );

        ui->imgLbl_2->setPixmap(avatarEscalado);
    }
}


void MainWindow::on_backButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    resetearCampos();
}


void MainWindow::on_backButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    resetearCampos();
}


void MainWindow::on_crearButton1_clicked()
{
    //CUENTAS ESTANDAR
    QString nombre = ui->lineEdit->text().toLower();
    QString nombreReal = ui->lineEdit_2->text().toLower();
    QString password = ui->lineEdit_3->text().toLower();
    QString email = ui->lineEdit_4->text().toLower();
    QString img = rutaImagen;

    if (nombre.isEmpty() || password.isEmpty() || nombreReal.isEmpty() || img.isEmpty() || email.isEmpty() ){
        QMessageBox::critical(nullptr,"Error","Debes llenar los campos necesarios",QMessageBox::Ok);
        return;
    }

    if (manejo->existeUsuario(nombre)){
        QMessageBox::critical(nullptr,"Error","Este usuario ya existe, elige otro nombre",QMessageBox::Ok);
    } else {
        manejo->crearUsuarioNormal(nombre,password,nombreReal,email,img);
        QMessageBox::information(nullptr,"Exito","Tu cuenta ha sido registrada!",QMessageBox::Ok);
        resetearCampos();
        Usuario* userActual = manejo->autenticar(nombre,password);
        manejo->setIdUsuarioActual(userActual->getId());
        HomeWindow* h = new HomeWindow(nullptr,*manejo);
        this->close();
        h->show();
    }
}

void MainWindow::on_crearButton2_clicked()
{
    //CUENTAS ADMINISTRADOR
    QString nombre = ui->lineEdit_5->text().toLower();
    QString nombreReal = ui->lineEdit_6->text().toLower();
    QString password = ui->lineEdit_7->text().toLower();
    QString pais = ui->lineEdit_8->text().toLower();
    QString genero =  ui->comboBox->currentText();
    QString desc = ui->textEdit_2->toPlainText();
    QString img = rutaImagen;

    if (nombre.isEmpty() || password.isEmpty() || img.isEmpty()){
        QMessageBox::critical(nullptr,"Error","Debes llenar los campos necesarios",QMessageBox::Ok);
        return;
    }

    if (manejo->existeUsuario(nombre)){
        QMessageBox::critical(nullptr,"Error","Este usuario ya existe, elige otro nombre",QMessageBox::Ok);
    } else {
        manejo->crearArtista(nombre,password,pais,genero,nombreReal,desc,img);
        QMessageBox::information(nullptr,"Exito","Tu cuenta ha sido registrada!",QMessageBox::Ok);
        resetearCampos();
        Usuario* userActual = manejo->autenticar(nombre,password);
        manejo->setIdUsuarioActual(userActual->getId());
        ArtistaWindow* h = new ArtistaWindow(nullptr,*manejo);
        this->close();
        h->show();
    }

}

void MainWindow::on_loginButton_2_clicked()
{
    QString user = ui->logLbl->text().toLower();
    QString pass = ui->logLbl2->text().toLower();

    if (user.isEmpty() || pass.isEmpty()){
        QMessageBox::critical(nullptr,"Error","Debes llenar los campos necesarios",QMessageBox::Ok);
        return;
    } else {
        Usuario* userActual = manejo->autenticar(user,pass);
        manejo->setIdUsuarioActual(userActual->getId());
        if (userActual==nullptr || !userActual->estaActivo()){
            QMessageBox::critical(nullptr,"Error","Usuario invalido.",QMessageBox::Ok);
            return;
        } else {
            QMessageBox::information(nullptr,"yay","Ingresaste!",QMessageBox::Ok);

            if (userActual->getTipo()=="ADMIN"){
                ArtistaWindow* h = new ArtistaWindow(nullptr,*manejo);
                this->close();
                h->show();
            }else {
            HomeWindow* h = new HomeWindow(nullptr,*manejo);
             this->close();
            h->show();
            }
        }
    }
}


void MainWindow::on_backButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}


void MainWindow::on_toolButton_clicked()
{
    if (isCensurada){
        isCensurada = false;
        ui->lineEdit_3->setEchoMode(QLineEdit::Normal);
    } else {
        isCensurada = true;
        ui->lineEdit_3->setEchoMode(QLineEdit::Password);
    }
}


void MainWindow::on_toolButton_2_clicked()
{
    if (isCensurada){
        isCensurada = false;
        ui->lineEdit_7->setEchoMode(QLineEdit::Normal);
    } else {
        isCensurada = true;
        ui->lineEdit_7->setEchoMode(QLineEdit::Password);
    }
}

