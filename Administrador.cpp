#include "administrador.h"
#include <QDataStream>
#include <stdexcept>

Administrador::Administrador(const QString& username, const QString& password,const QString& nombre, const QString& pais, const QString& genero, const QString& desc) :
    Usuario(username, password)
    , pais(pais), genero(genero), descripcion(desc)

{
    tipo="ADMIN";
    nombreReal = nombre;
}

void Administrador::escribirEnStream(QDataStream& stream) const {
    Usuario::escribirEnStream(stream);
    stream << pais << genero << descripcion << tipo;
}

void Administrador::leerDesdeStream(QDataStream& stream) {
    Usuario::leerDesdeStream(stream);
    stream >> pais >> genero >> descripcion  >> tipo;
}

QDataStream& operator<<(QDataStream& out, const Administrador& usuario) {
    usuario.escribirEnStream(out);
    return out;
}

QDataStream& operator>>(QDataStream& in, Administrador& usuario) {
    usuario.leerDesdeStream(in);
    return in;
}
