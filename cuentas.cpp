#include "Cuentas.h"
#include "Estandar.h"
#include "Administrador.h"
#include <QDataStream>
#include <QFileInfo>
#include <QDebug>

Cuentas::Cuentas() : idUsuarioActual(-1), ultimoId(0) {
    inicializarArchivos();
    cargarUsuariosDesdeArchivo();
}

Cuentas::~Cuentas() {
    guardarUsuariosEnArchivo();
    // Liberar memoria de todos los usuarios
    for (auto& par : tablaUsuarios) {
        delete par.second;
    }
}

void Cuentas::inicializarArchivos() {
    const QStringList archivos = {ARCHIVO_USUARIOS, ARCHIVO_ARTISTAS, ARCHIVO_INDICES};

    for (const QString& nombreArchivo : archivos) {
        QFile archivo(nombreArchivo);
        if (!archivo.exists()) {
            if (archivo.open(QIODevice::WriteOnly)) {
                archivo.close();
                qDebug() << "Archivo creado:" << nombreArchivo;
            } else {
                qWarning() << "Error al crear archivo:" << nombreArchivo << archivo.errorString();
            }
        }
    }
}

void Cuentas::cargarUsuariosDesdeArchivo() {
    // Primero cargar índices si existen
    QFile indicesFile(ARCHIVO_INDICES);
    if (indicesFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&indicesFile);
        in >> indices;
    }

    // Cargar usuarios estándar
    QFile usuariosFile(ARCHIVO_USUARIOS);
    if (usuariosFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&usuariosFile);
        while (!in.atEnd()) {
            Estandar* usuario = new Estandar("", "", "", "");
            usuario->leerDesdeStream(in);

            if (usuario->getId() > ultimoId) {
                ultimoId = usuario->getId();
            }

            tablaUsuarios[usuario->getNombreUsuario()] = usuario;
            tablaUsuariosPorId[usuario->getId()] = usuario;
        }
    }

    // Cargar artistas/administradores
    QFile artistasFile(ARCHIVO_ARTISTAS);
    if (artistasFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&artistasFile);
        while (!in.atEnd()) {
            Administrador* admin = new Administrador("", "");
            admin->leerDesdeStream(in);

            if (admin->getId() > ultimoId) {
                ultimoId = admin->getId();
            }

            tablaUsuarios[admin->getNombreUsuario()] = admin;
            tablaUsuariosPorId[admin->getId()] = admin;
        }
    }
}

void Cuentas::guardarUsuariosEnArchivo() {
    // Guardar usuarios estándar
    QFile usuariosFile(ARCHIVO_USUARIOS);
    if (usuariosFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&usuariosFile);
        for (auto& pair : tablaUsuariosPorId) {
            if (pair.second->getTipo() == "ESTANDAR") {
                pair.second->escribirEnStream(out);
            }
        }
    }

    // Guardar artistas/administradores
    QFile artistasFile(ARCHIVO_ARTISTAS);
    if (artistasFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&artistasFile);
        for (auto& pair : tablaUsuariosPorId) {
            if (pair.second->getTipo() == "ADMIN") {
                pair.second->escribirEnStream(out);
            }
        }
    }

    // Guardar índices
    QFile indicesFile(ARCHIVO_INDICES);
    if (indicesFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&indicesFile);
        out << indices;
    }
}

int Cuentas::generarNuevoId() {
    return ++ultimoId;
}

Usuario* Cuentas::autenticar(const QString& nombreUsuario, const QString& contrasena) {
    auto it = tablaUsuarios.find(nombreUsuario);
    if (it != tablaUsuarios.end() && it->second->verificarContrasena(contrasena) && it->second->estaActivo()) {
        idUsuarioActual = it->second->getId();
        return it->second;
    }
    return nullptr;
}

void Cuentas::cerrarSesion() {
    idUsuarioActual = -1;
}

Usuario* Cuentas::getIdUsuarioActual() const {
    if (idUsuarioActual == -1) return nullptr;
    auto it = tablaUsuariosPorId.find(idUsuarioActual);
    return (it != tablaUsuariosPorId.end()) ? it->second : nullptr;
}

bool Cuentas::crearUsuarioNormal(const QString& nombreUsuario, const QString& contrasena,
                                 const QString& nombreReal, const QString& email,
                                 const QString& rutaImagen) {
    if (existeUsuario(nombreUsuario)) return false;

    Estandar* nuevoUsuario = new Estandar(nombreUsuario, contrasena, nombreReal, email, rutaImagen);
    nuevoUsuario->id = generarNuevoId();
    nuevoUsuario->fechaRegistro = QDateTime::currentDateTime();
    nuevoUsuario->estado = true;

    tablaUsuarios[nombreUsuario] = nuevoUsuario;
    tablaUsuariosPorId[nuevoUsuario->getId()] = nuevoUsuario;

    // Guardar inmediatamente en archivo
    QFile file(ARCHIVO_USUARIOS);
    if (file.open(QIODevice::Append)) {
        QDataStream out(&file);
        nuevoUsuario->escribirEnStream(out);
        return true;
    }

    return false;
}

bool Cuentas::crearArtista(const QString& nombreArtistico, const QString& contrasena,
                           const QString& pais, const QString& genero,
                           const QString& nombreReal, const QString& email,
                           const QString& rutaImagen) {
    if (existeUsuario(nombreArtistico)) return false;

    Administrador* nuevoArtista = new Administrador(nombreArtistico, contrasena, pais, genero);
    nuevoArtista->id = generarNuevoId();
    nuevoArtista->nombreReal = nombreReal;
    nuevoArtista->rutaImagen = rutaImagen;
    nuevoArtista->fechaRegistro = QDateTime::currentDateTime();
    nuevoArtista->estado = true;

    tablaUsuarios[nombreArtistico] = nuevoArtista;
    tablaUsuariosPorId[nuevoArtista->getId()] = nuevoArtista;

    // Guardar en archivo de artistas
    QFile file(ARCHIVO_ARTISTAS);
    if (file.open(QIODevice::Append)) {
        QDataStream out(&file);
        nuevoArtista->escribirEnStream(out);
        return true;
    }

    delete nuevoArtista;  // Si falla el guardado, liberar memoria
    return false;
}

Usuario* Cuentas::buscarUsuario(const QString& nombreUsuario) const {
    auto it = tablaUsuarios.find(nombreUsuario);
    return (it != tablaUsuarios.end()) ? it->second : nullptr;
}

bool Cuentas::existeUsuario(const QString& nombreUsuario) const {
    return tablaUsuarios.find(nombreUsuario) != tablaUsuarios.end();
}

bool Cuentas::desactivarCuenta(const QString& nombreUsuario) {
    Usuario* usuario = buscarUsuario(nombreUsuario);
    if (usuario && usuario->estaActivo()) {
        usuario->estado = false;

        // Reescribir todos los usuarios para actualizar el estado
        guardarUsuariosEnArchivo();
        return true;
    }
    return false;
}

// Sobrecarga del operador para serializar IndiceUsuario
QDataStream& operator<<(QDataStream& out, const Cuentas::IndiceUsuario& indice) {
    out << indice.nombreUsuario << indice.posicionArchivo << indice.esArtista;
    return out;
}

// Sobrecarga del operador para deserializar IndiceUsuario
QDataStream& operator>>(QDataStream& in, Cuentas::IndiceUsuario& indice) {
    in >> indice.nombreUsuario >> indice.posicionArchivo >> indice.esArtista;
    return in;
}
