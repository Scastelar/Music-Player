#ifndef USUARIO_H
#define USUARIO_H

#include <QString>
#include <QDateTime>
#include <QCryptographicHash>
#include <QDir>
#include "cancion.h"

class Usuario {
protected:
    int id;
    QString username;
    QString nombreReal;
    QString contrasena;  // Ahora cifrada
    QString rutaImagen;
    QString tipo;
    bool estado;
    QDateTime fechaRegistro;

    QString hashContrasena(const QString& contrasena) const {
        return QString(QCryptographicHash::hash(
                           (contrasena + QString::number(id)).toUtf8(),
                           QCryptographicHash::Sha256).toHex());
    }

public:
    Usuario(const QString& nombreUsuario, const QString& contrasena);
    virtual ~Usuario() = default;


    // Getters
    QString getNombreUsuario() const { return username; }
    int getId() const { return id; }
    QString getNombreReal() const { return nombreReal; }
    QString getRutaImagen() const { return rutaImagen; }
    QDateTime getFechaRegistro() const { return fechaRegistro; }
    bool estaActivo() const { return estado; }
    QString getTipo() const { return tipo; }

    bool verificarContrasena(const QString& contrasena) const {
        return this->contrasena == hashContrasena(contrasena);
    }

    // Setters
    void setRutaImagen(const QString& ruta) { rutaImagen = ruta; }
    void setEstado(bool estado) { this->estado = estado; }
    void setContrasena(const QString& nuevaContrasena) {
        contrasena = hashContrasena(nuevaContrasena);
    }

    // Métodos de serialización
    virtual void escribirEnStream(QDataStream& stream) const;
    virtual void leerDesdeStream(QDataStream& stream);

    friend class Cuentas;
};

#endif // USUARIO_H
