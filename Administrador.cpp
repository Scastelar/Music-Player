#include "administrador.h"
#include <QDataStream>
#include <stdexcept>

Administrador::Administrador(const QString& username, const QString& password, const QString& pais, const QString& genero, const QString& desc) :
    Usuario(username, password)
    , pais(pais), genero(genero), descripcion(desc)

{
}

void Administrador::escribirEnStream(QDataStream& stream) const {
    Usuario::escribirEnStream(stream);
    stream << pais << genero << descripcion;
}

void Administrador::leerDesdeStream(QDataStream& stream) {
    Usuario::leerDesdeStream(stream);
    stream >> pais >> genero >> descripcion;
}
