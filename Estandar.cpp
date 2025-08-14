#include "estandar.h"
#include <QDataStream>
#include <QFile>
#include <QDateTime>
#include <random>

Estandar::Estandar(const QString& username, const QString& password,
                   const QString& nombreReal, const QString& email) :
    Usuario(username, password),
    email(email){
    tipo="ESTANDAR";
    this->nombreReal = nombreReal;
}

void Estandar::escribirEnStream(QDataStream& stream) const {
    Usuario::escribirEnStream(stream);
    stream << email << tipo;
}

void Estandar::leerDesdeStream(QDataStream& stream) {
    Usuario::leerDesdeStream(stream);
    stream >> email >>  tipo;
}

QDataStream& operator<<(QDataStream& out, const Estandar& usuario) {
    usuario.escribirEnStream(out);
    return out;
}

QDataStream& operator>>(QDataStream& in, Estandar& usuario) {
   usuario.leerDesdeStream(in);
    return in;
}
