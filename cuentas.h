#ifndef CUENTAS_H
#define CUENTAS_H

#include "usuario.h"
#include "Estandar.h"
#include "Administrador.h"
#include <QFile>
#include <QDir>
#include <unordered_map>
#include <QDataStream>
#include <QCryptographicHash>
#include <limits>

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


// Hash personalizado para QString
struct QStringHash {
    size_t operator()(const QString& str) const {
        const double A = 0.6180339887;
        size_t hash = 0;
        for (QChar ch : str) {
            hash = hash * 31 + ch.unicode();
        }
        double val = hash * A;
        val -= static_cast<int>(val);
        return static_cast<size_t>(val * std::numeric_limits<size_t>::max());
    }
};

// Hash personalizado para int
struct IntHash {
    size_t operator()(int key) const {
        const double A = 0.6180339887;
        double val = key * A;
        val -= static_cast<int>(val);
        return static_cast<size_t>(val * std::numeric_limits<size_t>::max());
    }
};

class Cuentas {
private:
    mutable std::unordered_map<QString, Usuario*, QStringHash> tablaUsuarios;
    std::unordered_map<int, Usuario*, IntHash> tablaUsuariosPorId;
    std::unordered_map<QString, IndiceUsuario> indices;
    int idUsuarioActual;
    int ultimoId = 0;

    const QString CARPETA_USUARIOS = "usuarios/";
    const QString ARCHIVO_INDICES = "indices.dat";

    QString obtenerRutaArchivoUsuario(int userId, const QString& tipo) const {
        return CARPETA_USUARIOS + QString::number(userId) + "/" + tipo + "_" + QString::number(userId) + ".dat";
    }

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
