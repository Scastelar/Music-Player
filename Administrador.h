#ifndef ADMINISTRADOR_H
#define ADMINISTRADOR_H

#include "usuario.h"

class Administrador : public Usuario {
public:
    Administrador(const QString& username, const QString& password);

    // Implementación de metodos virtuales
    QString getTipo() const override { return "ADMIN"; }
    QString getUsername() const override { return username; }

    void escribirEnStream(QDataStream& stream) const override;
    void leerDesdeStream(QDataStream& stream) override;
};

#endif // ADMINISTRADOR_H
