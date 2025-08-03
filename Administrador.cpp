#include "administrador.h"
#include <QDataStream>
#include <stdexcept>

Administrador::Administrador(const QString& username, const QString& password) :
    Usuario(username, password) {
}

void Administrador::escribirEnStream(QDataStream& stream) const {
    Usuario::escribirEnStream(stream);
}

void Administrador::leerDesdeStream(QDataStream& stream) {
    Usuario::leerDesdeStream(stream);
}
