#ifndef HOME_H
#define HOME_H

#include <QMainWindow>
#include <QToolButton>
#include <QLineEdit>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class Home : public QMainWindow
{
    Q_OBJECT
public:
    Home(QWidget *parent = nullptr);
private:
    QWidget *central;
    QToolButton *btnInicio, *btnFavoritos, *btnPlaylist, *btnConfig;
    QToolButton *btnPerfil, *btnAjustes;
    QLineEdit *buscador;
    QScrollArea *scrollArea;
};

#endif // HOME_H





