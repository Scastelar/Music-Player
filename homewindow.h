#ifndef HOMEWINDOW_H
#define HOMEWINDOW_H

#include <QToolButton>
#include <QGridLayout>
#include <QMediaPlayer>
#include <QAudioOutput>   // For Qt 6
#include <QFileSystemWatcher>
#include <QMainWindow>
#include "cuentas.h"

namespace Ui {
class HomeWindow;
}

class HomeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HomeWindow(QWidget *parent, Cuentas& manejo);
    ~HomeWindow();

    void setIcono(QToolButton* boton, ushort unicode, int size);
    void conectarMenu();

private slots:
    void on_toolButton_Playlist_clicked();

    void on_toolButton_home_clicked();

    void on_lineEdit_editingFinished();

    void loadSongs();
    void playSong(const Cancion& cancion);


    void on_toolButton_play_clicked();

    void durationChanged(qint64 duration);

    void positionChanged(qint64 progress);

    void on_toolButton_Volume_clicked();

    void on_horizontalSlider_Audio_Volume_valueChanged(int value);

    void on_toolButton_upload_5_clicked();

    void onCancionesFileChanged(const QString &path);

private:
    Ui::HomeWindow *ui;
    Cuentas* manejo;
    QString Montserrat = "Montserrat";
    QString MaterialIcons = "Material-Icons";
    QMediaPlayer *media;
    QAudioOutput* audioOutput;

    QFileSystemWatcher *fileWatcher;

    bool isMuted = false;
    bool isPaused = false;
    void updateduration(qint64 duration);
    qint64 Mduration;


    void setupGrid();
    void clearGrid();
    QGridLayout *m_gridLayout;

    Usuario* usuario;

    QAction* editarDatos;
    QAction* verEstadisticas;
    QAction* cerrarSesion;
};

#endif // HOMEWINDOW_H
