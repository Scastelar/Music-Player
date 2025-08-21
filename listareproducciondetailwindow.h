// ListaReproduccionDetailWindow.h
#ifndef LISTA_REPRODUCCION_WINDOW_H
#define LISTA_REPRODUCCION_WINDOW_H

#include "Album.h"
#include "Playlist.h"
#include "Cuentas.h"
#include <QWidget>
#include <QListWidget>
#include <QLabel>

class ListaReproduccionDetailWindow : public QWidget {
    Q_OBJECT

public:
    explicit ListaReproduccionDetailWindow(ListaReproduccion& lista, Cuentas* cuentas, QWidget* parent = nullptr);

signals:
    void solicitarReproduccionAlbum(QList<Cancion*> canciones, int startIndex);
    void solicitarReproduccionCancion(Cancion* cancion);
    void editarCancionClicked(int cancionId);
    void eliminarCancionClicked(int cancionId, int listaId);
    void listaModificada(ListaReproduccion& lista);
    void agregarCancionALista(ListaReproduccion* lista);

private slots:
    void onEditTitleClicked();
    void onEditCoverClicked();
    void onItemClicked(QListWidgetItem* item);
    void onAgregarCancionClicked();

public slots:
    void setRandomMode(bool enabled) { m_randomMode = enabled; }
    void reproducirSiguiente();
    void reproducirAnterior();

private:
    void setupUI();
    void loadSongs();
    void updateCoverImage();

    ListaReproduccion& m_lista;
    Cuentas* m_cuentas;

    QLabel* m_coverLabel;
    QLabel* m_titleLabel;
    QListWidget* m_songsList;

    QList<Cancion*> m_canciones;
    int m_indiceActual = -1;
    bool m_randomMode = false;
};

#endif
