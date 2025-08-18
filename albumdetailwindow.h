#ifndef ALBUMDETAILWINDOW_H
#define ALBUMDETAILWINDOW_H

#include <QWidget>
#include <QListWidget>
#include "Album.h"
#include "Cuentas.h"

class AlbumDetailWindow : public QWidget {
    Q_OBJECT
public:
    explicit AlbumDetailWindow(const Album& album, Cuentas* cuentas, QWidget* parent = nullptr);

signals:
    void solicitarReproduccionCancion(Cancion* cancion);
   void solicitarReproduccionAlbum(const QList<Cancion*>& canciones, int indiceInicial);
    void agregarCancionAlAlbum(const Album* album);

public slots:
    void setRandomMode(bool enabled) { m_randomMode = enabled; }
    void reproducirSiguiente();
    void reproducirAnterior();

private slots:
    void onItemClicked(QListWidgetItem* item);
    void onAgregarCancionClicked();

private:
    void setupUI();
    void loadSongs();

    bool m_randomMode = false;

    QListWidget* m_songsList;
    QList<Cancion*> m_cancionesAlbum;
    int m_indiceActual = -1;
    const Album& m_album;
    Cuentas* m_cuentas;
};

#endif // ALBUMDETAILWINDOW_H
