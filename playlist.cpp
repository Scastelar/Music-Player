#include "Playlist.h"
#include <QDataStream>

Playlist::Playlist() : id(0), idUsuario(0),
    fechaCreacion(QDateTime::currentDateTime()){}

Playlist::Playlist(int id, const QString& nombre, int idUsuario) :
    id(id), nombre(nombre), idUsuario(idUsuario),
    fechaCreacion(QDateTime::currentDateTime()) {}

// Getters
int Playlist::getId() const { return id; }
QString Playlist::getNombre() const { return nombre; }
int Playlist::getIdUsuario() const { return idUsuario; }
QDateTime Playlist::getFechaCreacion() const { return fechaCreacion; }
QList<int> Playlist::getCanciones() const { return canciones; }
int Playlist::getCantidadCanciones() const { return canciones.size(); }

// Setters
void Playlist::setId(int id) { this->id = id; }
void Playlist::setNombre(const QString& nombre) {this->nombre = nombre;}
void Playlist::setIdUsuario(int id) { this->idUsuario = id; }

// Gestión de canciones
void Playlist::agregarCancion(int idCancion) {
    if (!canciones.contains(idCancion)) {
        canciones.append(idCancion);
    }
}

void Playlist::eliminarCancion(int idCancion) {
    if (canciones.removeAll(idCancion) > 0) {
    }
}

bool Playlist::contieneCancion(int idCancion) const {
    return canciones.contains(idCancion);
}

void Playlist::moverCancion(int posActual, int nuevaPos) {
    if (posActual >= 0 && posActual < canciones.size() &&
        nuevaPos >= 0 && nuevaPos < canciones.size()) {
        canciones.move(posActual, nuevaPos);
    }
}

void Playlist::limpiarPlaylist() {
    if (!canciones.isEmpty()) {
        canciones.clear();
    }
}

// Serialización
void Playlist::escribirEnStream(QDataStream& stream) const {
    stream << id << nombre << idUsuario
           << fechaCreacion <<  canciones;
}

void Playlist::leerDesdeStream(QDataStream& stream) {
    stream >> id >> nombre >> idUsuario
        >> fechaCreacion  >> canciones;
}


// Operadores de serialización
QDataStream& operator<<(QDataStream& out, const Playlist& playlist) {
    playlist.escribirEnStream(out);
    return out;
}

QDataStream& operator>>(QDataStream& in, Playlist& playlist) {
    playlist.leerDesdeStream(in);
    return in;
}
