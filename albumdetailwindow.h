#ifndef ALBUMDETAILWINDOW_H
#define ALBUMDETAILWINDOW_H

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include "Album.h"
#include "Cuentas.h"

class AlbumDetailWindow : public QWidget {
    Q_OBJECT
public:
    explicit AlbumDetailWindow(Album& album, Cuentas* cuentas, QWidget* parent = nullptr);

signals:
    void solicitarReproduccionCancion(Cancion* cancion);
   void solicitarReproduccionAlbum(const QList<Cancion*>& canciones, int indiceInicial);
    void agregarCancionAlAlbum( Album* album);

   void albumModificado( Album& album);
   void editarCancionClicked(int cancionId);
   void eliminarCancionClicked(int cancionId, int albumId);


public slots:
    void setRandomMode(bool enabled) { m_randomMode = enabled; }
    void reproducirSiguiente();
    void reproducirAnterior();

    void updateCoverImage();
    void onEditAlbumTitleClicked();
    void onEditAlbumCoverClicked();

private slots:
    void onItemClicked(QListWidgetItem* item);
    void onAgregarCancionClicked();

private:
    void setupUI();
    void loadSongs();

    bool m_randomMode = false;
    QLabel* m_coverLabel;
    QLabel* m_albumTitleLabel;
    QLabel* m_albumsetNombre;

    QListWidget* m_songsList;
    QList<Cancion*> m_cancionesAlbum;
    int m_indiceActual = -1;
    Album& m_album;
    Cuentas* m_cuentas;
};

#endif // ALBUMDETAILWINDOW_H
