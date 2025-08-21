#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "ListaReproduccion.h"

class Playlist : public ListaReproduccion {
    int idUsuario;

public:
    Playlist(int id = 0, const QString& nombre = "", int usuarioId = 0, const QString& portada = "")
        : ListaReproduccion(id, nombre, portada), idUsuario(usuarioId) {}

    int getIdUsuario() const { return idUsuario; }
    void setIdUsuario(int id) { idUsuario = id; }

    QString getTipoString() const override { return "PLAYLIST"; }

    friend QDataStream& operator<<(QDataStream& out, const Playlist& pl);
    friend QDataStream& operator>>(QDataStream& in, Playlist& pl);
};
#endif
