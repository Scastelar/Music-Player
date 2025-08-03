#ifndef CUENTAS_H
#define CUENTAS_H

#include "usuario.h"
#include <QFile>
#include <unordered_map>

class Cuentas {
private:
    mutable std::unordered_map<QString, Usuario*> tablaUsuarios;
    std::unordered_map<int, Usuario*> tablaUsuariosPorId;  // Cambiado a int como clave
    int idUsuarioActual;
    int ultimoId = 0;  // Para generar IDs únicos

    const QString ARCHIVO_USUARIOS = "usuarios.dat";
    const QString ARCHIVO_ARTISTAS = "artistas.dat";
    const QString ARCHIVO_INDICES = "indices.dat";


    void cargarUsuariosDesdeArchivo();
    void guardarUsuarioEnArchivo(Usuario* usuario);
    int generarNuevoId();  // Genera IDs únicos

public:
    Cuentas();
    ~Cuentas();

    // Autenticacion
    Usuario* autenticar(const QString& nombreUsuario, const QString& contrasena);
    void cerrarSesion();

    // Gestion de cuentas
    bool crearUsuarioNormal(const QString& nombreUsuario, const QString& contrasena,
                            const QString& nombreReal, const QString& email,
                            const QString& rutaImagen = "");

    bool desactivarCuenta(const QString& nombreUsuario);

    // Busqueda
    Usuario* buscarUsuario(const QString& nombreUsuario) const;
    bool existeUsuario(const QString& nombreUsuario) const;
};

#endif // CUENTAS_H
