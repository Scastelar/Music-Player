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
    const QStringList archivos = {ARCHIVO_USUARIOS, ARCHIVO_ARTISTAS, ARCHIVO_INDICES,ARCHIVO_CALIFICACIONES,ARCHIVO_REPRODUCCIONES};

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
    QFile usuariosFile(ARCHIVO_USUARIOS);
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
    QFile artistasFile(ARCHIVO_ARTISTAS);
    if (artistasFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&artistasFile);

        if (!indices.empty()) {
            // Carga optimizada usando índices
            for (const auto& pair : indices) {
                if (!pair.second.esArtista) {
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
    QFile usuariosFile(ARCHIVO_USUARIOS);
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
    QFile artistasFile(ARCHIVO_ARTISTAS);
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
    // 1. Biblioteca personal
    QString bibliotecaFile = QString("biblioteca_%1.dat").arg(userId);
    QFile bFile(bibliotecaFile);
    if (bFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&bFile);
        // Cabecera vacía - el archivo existe pero sin contenido inicial
        bFile.close();
    }

    // 2. Listas de reproducción
    QString listasFile = QString("listas_%1.dat").arg(userId);
    QFile lFile(listasFile);
    if (lFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&lFile);
        // Cabecera vacía
        lFile.close();
    }

    // 3. Canciones favoritas
    QString favFile = QString("favoritos_%1.dat").arg(userId);
    QFile fFile(favFile);
    if (fFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&fFile);
        // Cabecera vacía
        fFile.close();
    }
}

void Cuentas::crearArchivosAdministrador(int userId) {
    // 1. Álbums
    QString albumsFile = QString("albums_%1.dat").arg(userId);
    QFile aFile(albumsFile);
    if (aFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&aFile);
        // Cabecera vacía
        aFile.close();
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
    // Verificar si el ID existe en la tabla
    if (nuevoId == -1) {
        idUsuarioActual = -1; // Permite cerrar sesión
        return true;
    }

    auto it = tablaUsuariosPorId.find(nuevoId);
    if (it != tablaUsuariosPorId.end()) {
        idUsuarioActual = nuevoId;
        return true;
    }

    return false; // El ID no existe
}

bool Cuentas::crearUsuarioNormal(const QString& nombreUsuario, const QString& contrasena,
                                 const QString& nombreReal, const QString& email,
                                 const QString& rutaImagen) {
    if (existeUsuario(nombreUsuario)) return false;

    Estandar* nuevoUsuario = new Estandar(nombreUsuario, contrasena, nombreReal, email);
    nuevoUsuario->rutaImagen = rutaImagen;
    nuevoUsuario->id = generarNuevoId();
    nuevoUsuario->fechaRegistro = QDateTime::currentDateTime();
    nuevoUsuario->estado = true;

    tablaUsuarios[nombreUsuario] = nuevoUsuario;
    tablaUsuariosPorId[nuevoUsuario->getId()] = nuevoUsuario;

    crearArchivosUsuarioEstandar(nuevoUsuario->getId());

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
                           const QString& nombreReal, const QString& descripcion,
                           const QString& rutaImagen) {
    if (existeUsuario(nombreArtistico)) return false;

    Administrador* nuevoArtista = new Administrador(nombreArtistico, contrasena, pais, genero,descripcion);
    nuevoArtista->id = generarNuevoId();
    nuevoArtista->nombreReal = nombreReal;
    nuevoArtista->rutaImagen = rutaImagen;
    nuevoArtista->fechaRegistro = QDateTime::currentDateTime();
    nuevoArtista->estado = true;

    tablaUsuarios[nombreArtistico] = nuevoArtista;
    tablaUsuariosPorId[nuevoArtista->getId()] = nuevoArtista;

    crearArchivosAdministrador(nuevoArtista->getId());


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

// Implementación de los operadores para IndiceUsuario
QDataStream& operator<<(QDataStream& out, const IndiceUsuario& indice) {
    out << indice.nombreUsuario << indice.posicionArchivo << indice.esArtista;
    return out;
}

QDataStream& operator>>(QDataStream& in, IndiceUsuario& indice) {
    in >> indice.nombreUsuario >> indice.posicionArchivo >> indice.esArtista;
    return in;
}

// Implementación de los operadores para unordered_map
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
