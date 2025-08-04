#ifndef CUENTAS_H
#define CUENTAS_H

#include "usuario.h"
#include "Estandar.h"
#include "Administrador.h"
#include <QFile>
#include <unordered_map>
#include <QDataStream>

struct IndiceUsuario {
    QString nombreUsuario;
    qint64 posicionArchivo;
    bool esArtista;
};

// Declaración de los operadores de sobrecarga para IndiceUsuario
QDataStream& operator<<(QDataStream& out, const IndiceUsuario& indice);
QDataStream& operator>>(QDataStream& in, IndiceUsuario& indice);

// Declaración de los operadores para unordered_map
QDataStream& operator<<(QDataStream& out, const std::unordered_map<QString, IndiceUsuario>& map);
QDataStream& operator>>(QDataStream& in, std::unordered_map<QString, IndiceUsuario>& map);

class Cuentas {
private:
    mutable std::unordered_map<QString, Usuario*> tablaUsuarios;
    std::unordered_map<int, Usuario*> tablaUsuariosPorId;  // Cambiado a int como clave
    std::unordered_map<QString, IndiceUsuario> indices;
    int idUsuarioActual;
    int ultimoId = 0;  // Para generar IDs únicos

    const QString ARCHIVO_USUARIOS = "usuarios.dat";
    const QString ARCHIVO_ARTISTAS = "artistas.dat";
    const QString ARCHIVO_INDICES = "indices.dat";
    const QString ARCHIVO_CALIFICACIONES = "calificaciones.dat";
    const QString ARCHIVO_REPRODUCCIONES = "reproducciones.dat";

    void crearArchivosUsuarioEstandar(int userId);
    void crearArchivosAdministrador(int userId);
    void inicializarArchivos();
    void cargarUsuariosDesdeArchivo();
    void guardarUsuarioEnArchivo(Usuario* usuario);
    void guardarUsuariosEnArchivo();
    int generarNuevoId();  // Genera IDs únicos

 public:
    Cuentas();
    ~Cuentas();

    // Autenticacion
    Usuario* autenticar(const QString& nombreUsuario, const QString& contrasena);
    void cerrarSesion();
    Usuario* getIdUsuarioActual() const;
    bool setIdUsuarioActual(int nuevoId);

    // Gestion de cuentas
    bool crearUsuarioNormal(const QString& nombreUsuario, const QString& contrasena,
                            const QString& nombreReal, const QString& email,
                            const QString& rutaImagen = "");

    bool crearArtista(const QString& nombreArtistico, const QString& contrasena,
                      const QString& pais, const QString& genero,
                      const QString& nombreReal, const QString& email,
                      const QString& rutaImagen = "");

    bool desactivarCuenta(const QString& nombreUsuario);

    // Busqueda
    Usuario* buscarUsuario(const QString& nombreUsuario) const;
    bool existeUsuario(const QString& nombreUsuario) const;
};

#endif // CUENTAS_H
