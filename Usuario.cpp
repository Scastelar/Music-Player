#include "usuario.h"
#include <QDataStream>

Usuario::Usuario(const QString& nombreUsuario, const QString& contrasenaPlana) :
    username(nombreUsuario),
    estado(true),
    contrasenaTemporal(true), // Marcar como no hasheada
    fechaRegistro(QDateTime::currentDateTime())
{
    id = -1;
    contrasena = contrasenaPlana;
    contrasenaHash = contrasenaPlana; // Guardar temporalmente sin hashear
}

void Usuario::escribirEnStream(QDataStream& stream) const {
    Q_ASSERT(!contrasenaTemporal); // Asegurar que está hasheada
    stream << id << username << contrasenaHash
           << nombreReal << rutaImagen << estado << fechaRegistro;
}

void Usuario::leerDesdeStream(QDataStream& stream) {
    stream >> id >> username >> contrasenaHash
        >> nombreReal >> rutaImagen >> estado >> fechaRegistro;
    contrasenaTemporal = false; // Los datos leídos ya están hasheados
}

QDataStream& operator<<(QDataStream& out, const Usuario& usuario) {
   usuario.escribirEnStream(out);
    return out;
}

QDataStream& operator>>(QDataStream& in, Usuario& usuario) {
    usuario.leerDesdeStream(in);
    return in;
}
