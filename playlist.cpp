#include "Playlist.h"

QDataStream& operator<<(QDataStream& out, const Playlist& pl) {
    out << static_cast<const ListaReproduccion&>(pl);
    out << pl.idUsuario;
    return out;
}

QDataStream& operator>>(QDataStream& in, Playlist& pl) {
    in >> static_cast<ListaReproduccion&>(pl);
    in >> pl.idUsuario;
    return in;
}
