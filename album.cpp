#include "Album.h"
#include <QDataStream>

Album::Album() : id(0), idArtista(0), tipo(determinarTipo(0)), estado(true), portada(""),
    fechaCreacion(QDateTime::currentDateTime()) {}

Album::Album(int id, const QString& nombre, int idArtista, const QString& rutaPortada = "") :
    id(id), nombre(nombre), idArtista(idArtista), tipo(determinarTipo(0)), estado(true), portada(rutaPortada),
    fechaCreacion(QDateTime::currentDateTime()) {}

// Método privado para determinar el tipo basado en el número de canciones
Album::Tipo Album::determinarTipo(int numCanciones) const {
    if (numCanciones <= 1) {
        return Tipo::SINGLE;
    } else if (numCanciones >= 2 && numCanciones <= 6) {
        return Tipo::EP;
    } else {
        return Tipo::ALBUM;
    }
}

QString Album::getTipoString() const {
    switch(tipo) {
    case Tipo::ALBUM: return "Álbum";
    case Tipo::EP: return "EP";
    case Tipo::SINGLE: return "Single";
    default: return "Desconocido";
    }
}

// Gestión de canciones - ahora actualiza el tipo automáticamente
void Album::agregarCancion(int idCancion) {
    if (!canciones.contains(idCancion)) {
        canciones.append(idCancion);
        tipo = determinarTipo(canciones.size());
    }
}

void Album::eliminarCancion(int idCancion) {
    canciones.removeAll(idCancion);
    tipo = determinarTipo(canciones.size());
}

bool Album::contieneCancion(int idCancion) const {
    return canciones.contains(idCancion);
}

// La validación ahora simplemente verifica que el tipo coincida con el número de canciones
bool Album::validarTipo() const {
    return tipo == determinarTipo(canciones.size());
}

// Serialización (sin cambios)
void Album::escribirEnStream(QDataStream& stream) const {
    stream << id << nombre << idArtista
           << static_cast<int>(tipo) << fechaCreacion << canciones << estado << portada;
}

void Album::leerDesdeStream(QDataStream& stream) {
    int tipoInt;
    stream >> id >> nombre >> idArtista >> tipoInt >> fechaCreacion >> canciones >> estado >> portada;
    tipo = static_cast<Tipo>(tipoInt);
}

QDataStream& operator<<(QDataStream& out, const Album& album) {
    album.escribirEnStream(out);
    return out;
}

QDataStream& operator>>(QDataStream& in, Album& album) {
    album.leerDesdeStream(in);
    return in;
}
