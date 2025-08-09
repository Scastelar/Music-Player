#include "cancion.h"
#include <QFile>
#include <QDataStream>
#include <QIODevice>

Cancion::Cancion() : id(0), estado(true), fechaRegistro(QDateTime::currentDateTime()) {}

Cancion::Cancion(const QString& titulo, const QString& genero, const QString& categoria,
                 const QString& rutaPortada, const QString& rutaAudio, const QString& artista) :
    titulo(titulo),
    artista(artista),
    genero(genero),
    categoria(categoria),
    rutaPortada(rutaPortada),
    estado(true),
    rutaAudio(rutaAudio),
    fechaRegistro(QDateTime::currentDateTime())
{
    // Generar ID único usando hash de los datos de la canción
    QString hashData = titulo + artista + genero + categoria + rutaAudio + fechaRegistro.toString(Qt::ISODate);
    QByteArray hash = QCryptographicHash::hash(hashData.toUtf8(), QCryptographicHash::Sha1);
    id = qAbs(qHash(hash));

    // TODO: Calcular duración del archivo de audio (puedes implementar esto después)
    duracion = "00:00"; // Valor temporal
}

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

bool Cancion::existeCancion(int id, const QString& nombreArchivo) {
    QList<Cancion> canciones = cargarDesdeArchivo(nombreArchivo);
    for (const Cancion& cancion : canciones) {
        if (cancion.getId() == id) {
            return true;
        }
    }
    return false;
}

bool Cancion::eliminarCancion(int id, const QString& nombreArchivo) {
    QList<Cancion> canciones = cargarDesdeArchivo(nombreArchivo);
    QList<Cancion> cancionesActualizadas;
    bool encontrada = false;

    for (Cancion cancion : canciones) {
        if (cancion.getId() != id) {
            cancionesActualizadas.append(cancion);
        } else {
            encontrada = true;
        }
    }

    if (encontrada) {
        QFile file(nombreArchivo);
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);
            for (const Cancion& cancion : cancionesActualizadas) {
                out << cancion;
            }
            file.close();
            return true;
        }
    }

    return false;
}

QDataStream& operator<<(QDataStream& out, const Cancion& cancion) {
    out << cancion.id
        << cancion.titulo
        << cancion.artista
        << cancion.genero
        << cancion.categoria
        << cancion.duracion
        << cancion.rutaPortada
        << cancion.estado
        << cancion.rutaAudio
        << cancion.fechaRegistro;
    return out;
}

QDataStream& operator>>(QDataStream& in, Cancion& cancion) {
    in >> cancion.id
        >> cancion.titulo
        >> cancion.artista
        >> cancion.genero
        >> cancion.categoria
        >> cancion.duracion
        >> cancion.rutaPortada
        >> cancion.estado
        >> cancion.rutaAudio
        >> cancion.fechaRegistro;
    return in;
}
