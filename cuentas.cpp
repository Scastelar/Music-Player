#include "Cuentas.h"
#include "Estandar.h"
#include "Administrador.h"
#include <QDataStream>
#include <QFileInfo>
#include <QDir>
#include <QCryptographicHash>
#include <QDebug>

Cuentas::Cuentas() : idUsuarioActual(-1), ultimoId(0) {
    // Crear directorio de usuarios si no existe
    QDir dir;
    if (!dir.exists(CARPETA_USUARIOS)) {
        dir.mkpath(CARPETA_USUARIOS);
    }

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
    const QStringList archivos = {ARCHIVO_INDICES};

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
    // Cargar índices primero
    QFile indicesFile(ARCHIVO_INDICES);
    if (indicesFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&indicesFile);
        in >> indices;
    }

    // Cargar usuarios estándar usando índices si existen
    QString usuariosFilePath = CARPETA_USUARIOS + "usuarios.dat";
    QFile usuariosFile(usuariosFilePath);
    if (usuariosFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&usuariosFile);

        if (!indices.empty()) {
            // Carga optimizada usando índices
            for (const auto& pair : indices) {
                if (!pair.second.esArtista) {
                    usuariosFile.seek(pair.second.posicionArchivo);
                    Estandar* usuario = new Estandar("", "", "", "");
                    usuario->leerDesdeStream(in);

                    if (usuario->getId() > ultimoId) {
                        ultimoId = usuario->getId();
                    }

                    tablaUsuarios[usuario->getNombreUsuario()] = usuario;
                    tablaUsuariosPorId[usuario->getId()] = usuario;
                }
            }
        } else {
            // Carga secuencial (para primera vez o si no hay índices)
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
    }

    // Cargar artistas/administradores
    QString artistasFilePath = CARPETA_USUARIOS + "artistas.dat";
    QFile artistasFile(artistasFilePath);
    if (artistasFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&artistasFile);

        if (!indices.empty()) {
            // Carga optimizada usando índices
            for (const auto& pair : indices) {
                if (pair.second.esArtista) {
                    artistasFile.seek(pair.second.posicionArchivo);
                    Administrador* usuario = new Administrador("", "", "", "", "");
                    usuario->leerDesdeStream(in);

                    if (usuario->getId() > ultimoId) {
                        ultimoId = usuario->getId();
                    }

                    tablaUsuarios[usuario->getNombreUsuario()] = usuario;
                    tablaUsuariosPorId[usuario->getId()] = usuario;
                }
            }
        } else {
            // Carga secuencial (para primera vez o si no hay índices)
            while (!in.atEnd()) {
                Administrador* admin = new Administrador("", "","","","");
                admin->leerDesdeStream(in);

                if (admin->getId() > ultimoId) {
                    ultimoId = admin->getId();
                }

                tablaUsuarios[admin->getNombreUsuario()] = admin;
                tablaUsuariosPorId[admin->getId()] = admin;
            }
        }
    }
}

void Cuentas::guardarUsuariosEnArchivo() {
    indices.clear(); // Limpiar índices antes de regenerarlos

    // Guardar usuarios estándar
    QString usuariosFilePath = CARPETA_USUARIOS + "usuarios.dat";
    QFile usuariosFile(usuariosFilePath);
    if (usuariosFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&usuariosFile);
        for (auto& pair : tablaUsuariosPorId) {
            if (pair.second->getTipo() == "ESTANDAR") {
                IndiceUsuario indice;
                indice.nombreUsuario = pair.second->getNombreUsuario();
                indice.posicionArchivo = usuariosFile.pos();
                indice.esArtista = false;
                indices[pair.second->getNombreUsuario()] = indice;

                pair.second->escribirEnStream(out);
            }
        }
    }

    // Guardar artistas/administradores
    QString artistasFilePath = CARPETA_USUARIOS + "artistas.dat";
    QFile artistasFile(artistasFilePath);
    if (artistasFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&artistasFile);
        for (auto& pair : tablaUsuariosPorId) {
            if (pair.second->getTipo() == "ADMIN") {
                IndiceUsuario indice;
                indice.nombreUsuario = pair.second->getNombreUsuario();
                indice.posicionArchivo = artistasFile.pos();
                indice.esArtista = true;
                indices[pair.second->getNombreUsuario()] = indice;

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

void Cuentas::crearArchivosUsuarioEstandar(int userId) {
    QString userDir = QString("%1%2/").arg(CARPETA_USUARIOS).arg(userId);
    QDir dir;
    dir.mkpath(userDir);

    // Crear archivos para el usuario
    QStringList archivos = {
        "biblioteca_" + QString::number(userId) + ".dat",
        "listas_" + QString::number(userId) + ".dat",
        "favoritos_" + QString::number(userId) + ".dat"
    };

    for (const QString& archivo : archivos) {
        QFile file(userDir + archivo);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
        }
    }
}

void Cuentas::crearArchivosAdministrador(int userId) {
    QString userDir = QString("%1%2/").arg(CARPETA_USUARIOS).arg(userId);
    QDir dir;
    dir.mkpath(userDir);

    // Crear archivos para el administrador
    QStringList archivos = {
        "albumes_" + QString::number(userId) + ".dat",
        "catalogo_" + QString::number(userId) + ".dat"
    };

    for (const QString& archivo : archivos) {
        QFile file(userDir + archivo);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
        }
    }
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

bool Cuentas::setIdUsuarioActual(int nuevoId) {
    if (nuevoId == -1) {
        idUsuarioActual = -1;
        return true;
    }

    auto it = tablaUsuariosPorId.find(nuevoId);
    if (it != tablaUsuariosPorId.end()) {
        idUsuarioActual = nuevoId;
        return true;
    }

    return false;
}

bool Cuentas::crearUsuarioNormal(const QString& nombreUsuario, const QString& contrasena,
                                 const QString& nombreReal, const QString& email,
                                 const QString& rutaImagen) {
    if (existeUsuario(nombreUsuario)) return false;

    Estandar* nuevoUsuario = new Estandar(nombreUsuario, contrasena, nombreReal, email);
    nuevoUsuario->id = generarNuevoId();
    nuevoUsuario->rutaImagen = rutaImagen;
    nuevoUsuario->fechaRegistro = QDateTime::currentDateTime();
    nuevoUsuario->estado = true;
    nuevoUsuario->setContrasena(contrasena); // Esto generará el hash

    tablaUsuarios[nombreUsuario] = nuevoUsuario;
    tablaUsuariosPorId[nuevoUsuario->getId()] = nuevoUsuario;

    crearArchivosUsuarioEstandar(nuevoUsuario->getId());

    // Guardar inmediatamente en archivo
    QString usuariosFilePath = CARPETA_USUARIOS + "usuarios.dat";
    QFile file(usuariosFilePath);
    if (file.open(QIODevice::Append)) {
        QDataStream out(&file);
        nuevoUsuario->escribirEnStream(out);

        // Actualizar índices
        IndiceUsuario indice;
        indice.nombreUsuario = nombreUsuario;
        indice.posicionArchivo = file.pos() - sizeof(nuevoUsuario); // Aproximación
        indice.esArtista = false;
        indices[nombreUsuario] = indice;

        return true;
    }

    return false;
}

bool Cuentas::crearArtista(const QString& nombreArtistico, const QString& contrasena,
                           const QString& pais, const QString& genero,
                           const QString& nombreReal, const QString& descripcion,
                           const QString& rutaImagen) {
    if (existeUsuario(nombreArtistico)) return false;

    Administrador* nuevoArtista = new Administrador(nombreArtistico, contrasena, pais, genero, descripcion);
    nuevoArtista->id = generarNuevoId();
    nuevoArtista->nombreReal = nombreReal;
    nuevoArtista->rutaImagen = rutaImagen;
    nuevoArtista->fechaRegistro = QDateTime::currentDateTime();
    nuevoArtista->estado = true;
    nuevoArtista->setContrasena(contrasena); // Esto generará el hash

    tablaUsuarios[nombreArtistico] = nuevoArtista;
    tablaUsuariosPorId[nuevoArtista->getId()] = nuevoArtista;

    crearArchivosAdministrador(nuevoArtista->getId());

    // Guardar en archivo de artistas
    QString artistasFilePath = CARPETA_USUARIOS + "artistas.dat";
    QFile file(artistasFilePath);
    if (file.open(QIODevice::Append)) {
        QDataStream out(&file);
        nuevoArtista->escribirEnStream(out);

        // Actualizar índices
        IndiceUsuario indice;
        indice.nombreUsuario = nombreArtistico;
        indice.posicionArchivo = file.pos() - sizeof(nuevoArtista); // Aproximación
        indice.esArtista = true;
        indices[nombreArtistico] = indice;

        return true;
    }

    delete nuevoArtista;
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
        guardarUsuariosEnArchivo();
        return true;
    }
    return false;
}

QDataStream& operator<<(QDataStream& out, const IndiceUsuario& indice) {
    out << indice.nombreUsuario << indice.posicionArchivo << indice.esArtista;
    return out;
}

QDataStream& operator>>(QDataStream& in, IndiceUsuario& indice) {
    in >> indice.nombreUsuario >> indice.posicionArchivo >> indice.esArtista;
    return in;
}

QDataStream& operator<<(QDataStream& out, const std::unordered_map<QString, IndiceUsuario>& map) {
    out << static_cast<quint32>(map.size());
    for (const auto& pair : map) {
        out << pair.first << pair.second;
    }
    return out;
}

QDataStream& operator>>(QDataStream& in, std::unordered_map<QString, IndiceUsuario>& map) {
    map.clear();
    quint32 size;
    in >> size;
    for (quint32 i = 0; i < size; ++i) {
        QString key;
        IndiceUsuario value;
        in >> key >> value;
        map[key] = value;
    }
    return in;
}
