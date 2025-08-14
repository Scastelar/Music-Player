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
    QString contrasena;  // Contrasena sin hash
    QString rutaImagen;
    QString tipo;
    bool estado;
    QDateTime fechaRegistro;

    QString contrasenaHash;  // Cambiar nombre para mayor claridad
    bool contrasenaTemporal; // Bandera para controlar el hashing

    QString calcularHashContrasena(const QString& contrasenaPlana) const {
        Q_ASSERT(id != -1); // Asegurar que el ID esté establecido
        return QCryptographicHash::hash(
                   (contrasenaPlana + QString::number(id)).toUtf8(),
                   QCryptographicHash::Sha256).toHex();
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
    QString getContrasena() const { return (contrasena);}

    bool verificarContrasena(const QString& contrasenaPlana) const {
        if (contrasenaTemporal) {
            qWarning() << "Advertencia: Verificando contraseña con ID no establecido";
            return false;
        }
        return contrasenaHash == calcularHashContrasena(contrasenaPlana);
    }

    void setContrasena(const QString& nuevaContrasena) {
        if (id == -1) {
            contrasenaHash = nuevaContrasena;
            contrasenaTemporal = true;
        } else {
            contrasenaHash = calcularHashContrasena(nuevaContrasena);
            contrasenaTemporal = false;
        }
    }

    // Setters
    void setId(const int& id) {
        this->id = id;
        if (contrasenaTemporal) {
            contrasenaHash = calcularHashContrasena(contrasenaHash);
            contrasenaTemporal = false;
        }
    }
    void setNombreUsuario(const QString& user) { username = user; }
    void setNombreReal(const QString& name){ nombreReal = name; }
    void setRutaImagen(const QString& ruta) { rutaImagen = ruta; }
    void setEstado(bool estado) { this->estado = estado; }


    // Métodos de serialización
    virtual void escribirEnStream(QDataStream& stream) const;
    virtual void leerDesdeStream(QDataStream& stream);

    friend class Cuentas;
    friend QDataStream& operator<<(QDataStream& out, const Usuario& usuario);
    friend QDataStream& operator>>(QDataStream& in, Usuario& usuario);
};

#endif // USUARIO_H
