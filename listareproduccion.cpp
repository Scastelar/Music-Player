#include "ListaReproduccion.h"

QDataStream& operator<<(QDataStream& out, const ListaReproduccion& lista) {
    out << lista.id
        << lista.nombre
        << lista.canciones
        << lista.portada
        << lista.fechaCreacion;
    return out;
}

QDataStream& operator>>(QDataStream& in, ListaReproduccion& lista) {
    in >> lista.id
        >> lista.nombre
        >> lista.canciones
        >> lista.portada
        >> lista.fechaCreacion;
    return in;
}
