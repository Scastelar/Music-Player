#include "Cancion.h"
#include <QDataStream>


Cancion::Cancion() : id(0), duracion(0), fechaRegistro(QDateTime::currentDateTime()) {}

Cancion::Cancion(int id, const QString& titulo, const QString& artista,
                 const QString& album, const QString& genero, const QString& categoria,
                 const QString& rutaAudio, const QString& rutaPortada) :
    id(id), titulo(titulo), artista(artista), album(album),
    genero(genero), categoria(categoria), rutaAudio(rutaAudio),
    rutaPortada(rutaPortada), estado(true),fechaRegistro(QDateTime::currentDateTime()) {}


void Cancion::guardarEnArchivo(const QString& nombreArchivo) const {
    QFile file(nombreArchivo);
    if (file.open(QIODevice::Append)) {
        QDataStream out(&file);
        out << *this;
        file.close();
    }
}

QList<Cancion> Cancion::cargarDesdeArchivo(const QString& nombreArchivo) {
    QList<Cancion> canciones;
    QFile file(nombreArchivo);

    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        while (!in.atEnd()) {
            Cancion cancion;
            in >> cancion;
            canciones.append(cancion);
        }
        file.close();
    }

    return canciones;
}


// Serialización
void Cancion::escribirEnStream(QDataStream& stream) const {
    stream << id << titulo << artista << album << genero
           << duracion << estado << rutaAudio << rutaPortada << fechaRegistro;
}

void Cancion::leerDesdeStream(QDataStream& stream) {
    stream >> id >> titulo >> artista >> album >> genero
        >> duracion >> estado >> rutaAudio >> rutaPortada >> fechaRegistro;
}

QDataStream& operator<<(QDataStream& out, const Cancion& cancion) {
    cancion.escribirEnStream(out);
    return out;
}

QDataStream& operator>>(QDataStream& in, Cancion& cancion) {
    cancion.leerDesdeStream(in);
    return in;
}
