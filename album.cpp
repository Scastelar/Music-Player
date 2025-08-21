#include "Album.h"

QDataStream& operator<<(QDataStream& out, const Album& album) {
    // Serializar primero la base
    out << static_cast<const ListaReproduccion&>(album);
    out << album.idArtista
        << static_cast<qint8>(album.tipo); // guardar como byte
    return out;
}

QDataStream& operator>>(QDataStream& in, Album& album) {
    in >> static_cast<ListaReproduccion&>(album);

    qint8 tipoTmp;
    in >> album.idArtista >> tipoTmp;
    album.tipo = static_cast<Album::Tipo>(tipoTmp);

    return in;
}
