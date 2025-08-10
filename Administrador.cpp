#include "administrador.h"
#include <QDataStream>
#include <stdexcept>

Administrador::Administrador(const QString& username, const QString& password, const QString& pais, const QString& genero, const QString& desc) :
    Usuario(username, password)
    , pais(pais), genero(genero), descripcion(desc)

{
    tipo="ADMIN";
}

void Administrador::escribirEnStream(QDataStream& stream) const {
    Usuario::escribirEnStream(stream);
    stream << pais << genero << descripcion << tipo;
}

void Administrador::leerDesdeStream(QDataStream& stream) {
    Usuario::leerDesdeStream(stream);
    stream >> pais >> genero >> descripcion  >> tipo;
}

void Administrador::agregarCancionCatalogo(const QString& titulo, const QString& genero,
                                           const QString& categoria, const QString& rutaPortada,
                                           const QString& rutaAudio) {
    Cancion nuevaCancion(titulo, genero, categoria, rutaPortada, rutaAudio, this->getNombreUsuario());
    nuevaCancion.guardarEnArchivo("canciones.dat");
}

QList<Cancion> Administrador::obtenerCancionesCatalogo() const {
    return Cancion::cargarDesdeArchivo("canciones.dat");
}

bool Administrador::eliminarCancionCatalogo(int idCancion) {
    return Cancion::eliminarCancion(idCancion, "canciones.dat");
}
