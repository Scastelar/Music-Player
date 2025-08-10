#include "usuario.h"
#include <QDataStream>

Usuario::Usuario(const QString& nombreUsuario, const QString& contrasena) :
    username(nombreUsuario),
    contrasena(contrasena),  // En producción debería cifrarse aquí
    estado(true),
    fechaRegistro(QDateTime::currentDateTime()) {
    // ID será asignado por la clase Cuentas
    id = -1; // Valor temporal hasta que se asigne uno real
}

void Usuario::escribirEnStream(QDataStream& stream) const {
    stream << id << username << contrasena << nombreReal
           << rutaImagen << estado << fechaRegistro;
}

void Usuario::leerDesdeStream(QDataStream& stream) {
    stream >> id >> username >> contrasena >> nombreReal
        >> rutaImagen >> estado >> fechaRegistro;
}
