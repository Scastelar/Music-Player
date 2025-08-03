#ifndef ESTANDAR_H
#define ESTANDAR_H

#include "usuario.h"

class Estandar : public Usuario {
private:
    QString email;
public:
    Estandar(const QString& username, const QString& password, const QString& nombreReal,
             const QString& email, const QString& rutaImagen = "");

    // Implementación de metodos virtuales
    QString getTipo() const override { return "ESTANDAR"; }
    QString getUsername() const override { return username; }

    //Metodos propios

    QString getEmail() const { return email; }  // Nuevo getter

    void setEmail(const QString& email) { this->email = email; }

    //Metodos de serializacion
    void escribirEnStream(QDataStream& stream) const override;
    void leerDesdeStream(QDataStream& stream) override;
};

#endif // ESTANDAR_H
