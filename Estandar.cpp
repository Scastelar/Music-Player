#include "estandar.h"
#include <QDataStream>

Estandar::Estandar(const QString& username, const QString& password,
                   const QString& nombreReal, const QString& email,
                   const QString& rutaImagen) :
    Usuario(username, password),
    email(email) {
    this->nombreReal = nombreReal;
    this->rutaImagen = rutaImagen;
}

void Estandar::escribirEnStream(QDataStream& stream) const {
    Usuario::escribirEnStream(stream);
    stream << email;
}

void Estandar::leerDesdeStream(QDataStream& stream) {
    Usuario::leerDesdeStream(stream);
    stream >> email;
}
