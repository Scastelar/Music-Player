#ifndef USUARIO_H
#define USUARIO_H

#include <QString>
#include <QDateTime>

class Usuario {
protected:
    int id;  // Cambiado de QString a int
    QString username;
    QString nombreReal;
    QString contrasena;  // Ahora será cifrada
    QString rutaImagen;
    bool estado;  // true=activo, false=eliminado
    QDateTime fechaRegistro;

public:
    Usuario(const QString& nombreUsuario, const QString& contrasena);
    virtual ~Usuario() = default;

    // Metodos virtuales puros (clase abstracta)
    virtual QString getTipo() const = 0;
    virtual QString getUsername() const = 0;

    // Getters
    int getId() const { return id; }  // Cambiado a int
    QString getNombreUsuario() const { return username; }
    QString getNombreReal() const { return nombreReal; }
    QString getRutaImagen() const { return rutaImagen; }
    QDateTime getFechaRegistro() const { return fechaRegistro; }
    bool estaActivo() const { return estado; }

    bool verificarContrasena(const QString& contrasena) const {
        // Aquí debería implementarse la verificación de contraseña cifrada
        return this->contrasena == contrasena;
    }

    // Setters
    void setRutaImagen(const QString& ruta) { rutaImagen = ruta; }
    void setEstado(bool estado) { this->estado = estado; }

    // Metodos de serializacion
    virtual void escribirEnStream(QDataStream& stream) const;
    virtual void leerDesdeStream(QDataStream& stream);

    friend class Cuentas; // Permitir acceso a clase Cuentas
};

#endif // USUARIO_H
