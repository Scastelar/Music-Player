#include "Cuentas.h"
#include "Estandar.h"
#include "Administrador.h"
#include <QDataStream>
#include <QFileInfo>
#include <QDir>
#include <QCryptographicHash>
#include <QDebug>

// -------------------------------
// Constructores y destructor
// -------------------------------

Cuentas::Cuentas() : idUsuarioActual(-1), ultimoIdUsuario(0), ultimoIdCancion(0), ultimoIdLista(0) {
    QDir dir;
    if (!dir.exists(CARPETA_USUARIOS)) {
        dir.mkpath(CARPETA_USUARIOS);
    }
    inicializarArchivos();
    cargarUsuariosDesdeArchivo();
    cargarCancionesDesdeArchivo();
    cargarAlbumesDesdeArchivo();
    cargarPlaylistsDesdeArchivo();
}

Cuentas::~Cuentas() {
    guardarUsuariosEnArchivo();
    guardarCancionesEnArchivo();
    guardarAlbumesEnArchivo();
    guardarPlaylistsEnArchivo();

    qDeleteAll(usuarios);
    qDeleteAll(canciones);
    qDeleteAll(albumes);
    qDeleteAll(playlists);
}

// -------------------------------
// Métodos privados de inicialización
// -------------------------------

void Cuentas::inicializarArchivos() {
    const QStringList archivos = {
        ARCHIVO_USUARIOS,
        ARCHIVO_ARTISTAS,
        ARCHIVO_ALBUMES,
        ARCHIVO_PLAYLISTS,
        ARCHIVO_CANCIONES,
        ARCHIVO_INDICES
    };

    for (const QString& nombreArchivo : archivos) {
        QFile archivo(nombreArchivo);
        if (!archivo.exists() && archivo.open(QIODevice::WriteOnly)) {
            archivo.close();
            qDebug() << "Archivo creado:" << nombreArchivo;
        }
    }
}

// -------------------------------
// Métodos privados de carga/guardado
// -------------------------------

// Métodos optimizados de carga/guardado
void Cuentas::cargarUsuariosDesdeArchivo() {
    // 1. Cargar último ID usado
    QFile idFile("ultimoId.dat");
    if (idFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&idFile);
        in >> ultimoIdUsuario;
    }

    // 2. Cargar índices
    QFile indicesFile(ARCHIVO_INDICES);
    if (indicesFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&indicesFile);
        in >> indices;
    }

    // 3. Cargar usuarios con manejo de errores
    cargarArchivoUsuarios(ARCHIVO_USUARIOS, false);
    cargarArchivoUsuarios(ARCHIVO_ARTISTAS, true);
}

void Cuentas::cargarArchivoUsuarios(const QString& archivo, bool esArtista) {
    QFile file(archivo);
    if (!file.open(QIODevice::ReadOnly)) return;

    QDataStream in(&file);
    QMap<QString, IndiceUsuario> tempIndices;

    while (!in.atEnd()) {
        qint64 posicion = in.device()->pos();

        Usuario* usuario = esArtista ? static_cast<Usuario*>(new Administrador("","","","","",""))
                                     : static_cast<Usuario*>(new Estandar("","","",""));
        in >> *usuario;

        // Validación de datos críticos
        if (usuario->getId() <= 0 || usuario->getNombreUsuario().isEmpty()) {
            delete usuario;
            continue;
        }

        usuarios[usuario->getId()] = usuario;
        usernameAId[usuario->getNombreUsuario()] = usuario->getId();
        ultimoIdUsuario = qMax(ultimoIdUsuario, usuario->getId());

        // Actualizar índices
        indices[usuario->getNombreUsuario()] = {posicion, esArtista};
    }
}

void Cuentas::guardarUsuariosEnArchivo() {
    // 1. Guardar último ID usado
    QFile idFile("ultimoId.dat");
    if (idFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&idFile);
        out << ultimoIdUsuario;
    }

    // 2. Guardar usuarios en archivos separados
    guardarArchivoUsuarios(ARCHIVO_USUARIOS, false);
    guardarArchivoUsuarios(ARCHIVO_ARTISTAS, true);

    // 3. Guardar índices
    QFile indicesFile(ARCHIVO_INDICES);
    if (indicesFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&indicesFile);
        out << indices;
    }
}

void Cuentas::guardarArchivoUsuarios(const QString& archivo, bool esArtista) {
    QFile file(archivo);
    if (!file.open(QIODevice::WriteOnly)) return;

    QDataStream out(&file);
    QMap<QString, IndiceUsuario> tempIndices;

    for (Usuario* usuario : usuarios) {
        if ((esArtista && usuario->getTipo() == "ADMIN") ||
            (!esArtista && usuario->getTipo() != "ADMIN")) {

            qint64 posicion = out.device()->pos();
            out << *usuario;
            tempIndices[usuario->getNombreUsuario()] = {posicion, esArtista};
        }
    }

    // Fusionar con índices existentes
    for (auto it = tempIndices.begin(); it != tempIndices.end(); ++it) {
        indices[it.key()] = it.value();
    }
}

void Cuentas::cargarCancionesDesdeArchivo() {
    QFile file(ARCHIVO_CANCIONES);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);

        while (!in.atEnd()) {
            Cancion* cancion = new Cancion();
            in >> *cancion;
            canciones[cancion->getId()] = cancion;
            ultimoIdCancion = qMax(ultimoIdCancion, cancion->getId());

            // Actualizar índices
            cancionesPorTitulo[cancion->getTitulo()].append(cancion->getId());
            cancionesPorArtista[cancion->getArtista()].append(cancion->getId());
        }
    }
}

void Cuentas::guardarCancionesEnArchivo() {
    QFile file(ARCHIVO_CANCIONES);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        for (Cancion* cancion : canciones) {
            out << *cancion;
        }
    }
}

void Cuentas::cargarAlbumesDesdeArchivo() {
    QFile file(ARCHIVO_ALBUMES);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);

        while (!in.atEnd()) {
            Album* album = new Album();
            in >> *album;
            albumes[album->getId()] = album;
            ultimoIdLista = qMax(ultimoIdLista, album->getId());

            // Actualizar índices
            albumesPorTitulo[album->getNombre()].append(album->getId());
            albumesPorUsuario[album->getIdArtista()].append(album->getId());
        }
    }
}

void Cuentas::guardarAlbumesEnArchivo() {
    QFile file(ARCHIVO_ALBUMES);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        for (Album* album : albumes) {
            out << *album;
        }
    }
}

void Cuentas::cargarPlaylistsDesdeArchivo() {
    QFile file(ARCHIVO_PLAYLISTS);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);

        while (!in.atEnd()) {
            Playlist* pl = new Playlist();
            in >> *pl;
            playlists[pl->getId()] = pl;
            ultimoIdLista = qMax(ultimoIdLista, pl->getId());

            // Actualizar índices
            playlistsPorTitulo[pl->getNombre()].append(pl->getId());
            playlistsPorUsuario[pl->getIdUsuario()].append(pl->getId());
        }
    }
}

void Cuentas::guardarPlaylistsEnArchivo() {
    QFile file(ARCHIVO_PLAYLISTS);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        for (Playlist* pl : playlists) {
            out << *pl;
        }
    }
}



// -------------------------------
// Métodos privados de gestión de archivos de usuario
// -------------------------------

void Cuentas::crearArchivosUsuarioEstandar(int userId) {
    QString userDir = QString("%1%2/").arg(CARPETA_USUARIOS).arg(userId);
    QDir dir;
    dir.mkpath(userDir);

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

    QStringList archivos = {
        "biblioteca_" + QString::number(userId) + ".dat",
        "albumes_" + QString::number(userId) + ".dat",
        "favoritos_" + QString::number(userId) + ".dat"

    };

    for (const QString& archivo : archivos) {
        QFile file(userDir + archivo);
        if (file.open(QIODevice::WriteOnly)) {
            file.close();
        }
    }
}

// -------------------------------
// Métodos públicos de autenticación
// -------------------------------

Usuario* Cuentas::autenticar(const QString& nombreUsuario, const QString& contrasena) {
    // 1. Verificar existencia de usuario
    if (!usernameAId.contains(nombreUsuario)) {
        qDebug() << "Usuario no encontrado:" << nombreUsuario;
        return nullptr;
    }

    // 2. Obtener usuario
    int userId = usernameAId[nombreUsuario];
    Usuario* usuario = usuarios.value(userId, nullptr);

    if (!usuario) {
        qWarning() << "Usuario encontrado en usernameAId pero no en usuarios. ID:" << userId;
        return nullptr;
    }

    // 3. Verificar credenciales
    if (!usuario->verificarContrasena(contrasena)) {
        qDebug() << "Contraseña incorrecta para usuario:" << nombreUsuario;
        return nullptr;
    }

    if (!usuario->estaActivo()) {
        qDebug() << "Cuenta inactiva:" << nombreUsuario;
        return nullptr;
    }

    // 4. Establecer sesión
    idUsuarioActual = userId;
    qDebug() << "Autenticación exitosa. Usuario:" << nombreUsuario << "ID:" << userId;

    return usuario;
}

// Métodos optimizados de gestión de sesión
Usuario* Cuentas::getIdUsuarioActual() const {
    if (idUsuarioActual == -1) {
        qDebug() << "No hay usuario autenticado";
        return nullptr;
    }

    Usuario* usuario = usuarios.value(idUsuarioActual, nullptr);
    if (!usuario) {
        qWarning() << "Usuario actual ID" << idUsuarioActual << "no encontrado en el mapa";
    }

    return usuario;
}

bool Cuentas::setIdUsuarioActual(int nuevoId) {
    if (!usuarios.contains(nuevoId)) {
        qDebug() << "Intento de establecer ID de usuario no existente:" << nuevoId;
        return false;
    }

    idUsuarioActual = nuevoId;
    qDebug() << "Usuario actual establecido a ID:" << nuevoId;
    return true;
}

void Cuentas::cerrarSesion() {
    qDebug() << "Cerrando sesión del usuario ID:" << idUsuarioActual;
    idUsuarioActual = -1;
}

void Cuentas::verificarAutenticacion(const QString& username, const QString& password) {
    if (!usernameAId.contains(username)) {
        qDebug() << "Usuario no existe";
        return;
    }

    Usuario* u = usuarios[usernameAId[username]];
    qDebug() << "--- VERIFICACIÓN DE AUTENTICACIÓN ---";
    qDebug() << "ID Usuario:" << u->getId();
    qDebug() << "Contraseña almacenada (hash):" << u->getContrasena();

    QString hashCalculado = QCryptographicHash::hash(
                                (password + QString::number(u->getId())).toUtf8(),
                                QCryptographicHash::Sha256).toHex();

    qDebug() << "Hash calculado para comparación:" << hashCalculado;
    qDebug() << "Resultado verificación:" << (u->getContrasena() == hashCalculado);
}

// -------------------------------
// Métodos públicos de gestión de cuentas
// -------------------------------

bool Cuentas::crearUsuarioNormal(const QString& nombreUsuario, const QString& contrasena,
                                 const QString& nombreReal, const QString& email,
                                 const QString& rutaImagen) {
    // Validaciones iniciales
    if (nombreUsuario.isEmpty() || contrasena.isEmpty()) return false;
    if (existeUsuario(nombreUsuario)) return false;

    // Creación segura del usuario
    try {
        int nuevoId = ++ultimoIdUsuario;
        Estandar* nuevoUsuario = new Estandar(nombreUsuario, contrasena, nombreReal, email);
        nuevoUsuario->setId(nuevoId);
        nuevoUsuario->setRutaImagen(rutaImagen);
        nuevoUsuario->setEstado(true); // Activo por defecto

        // Verificación de debug
        qDebug() << "Hash final:" << nuevoUsuario->getContrasena();
        qDebug() << "Verificación:" << nuevoUsuario->verificarContrasena(contrasena);

        // Registro seguro
        usuarios[nuevoId] = nuevoUsuario;
        usernameAId[nombreUsuario] = nuevoId;

        // Creación de archivos y guardado
        crearArchivosUsuarioEstandar(nuevoId);
        guardarUsuariosEnArchivo();

        qDebug() << "Usuario normal creado exitosamente. ID:" << nuevoId;
        return true;
    } catch (...) {
        qCritical() << "Error al crear usuario normal";
        return false;
    }
}

bool Cuentas::crearArtista(const QString& nombreArtistico, const QString& contrasena,
                           const QString& pais, const QString& genero, const QString& desc,
                           const QString& nombreReal, const QString& rutaImagen) {
    // Validaciones iniciales
    if (nombreArtistico.isEmpty() || contrasena.isEmpty()) return false;
    if (existeUsuario(nombreArtistico)) return false;

    // Creación segura del artista
    try {
        int nuevoId = ++ultimoIdUsuario;
        Administrador* nuevoArtista = new Administrador(nombreArtistico, contrasena,
                                                        nombreReal, pais, genero, desc);
        nuevoArtista->setId(nuevoId);
        nuevoArtista->setRutaImagen(rutaImagen);
        nuevoArtista->setEstado(true); // Activo por defecto

        // Verificación de debug
        qDebug() << "Hash final:" << nuevoArtista->getContrasena();
        qDebug() << "Verificación:" << nuevoArtista->verificarContrasena(contrasena);

        // Registro seguro
        usuarios[nuevoId] = nuevoArtista;
        usernameAId[nombreArtistico] = nuevoId;

        // Creación de archivos y guardado
        crearArchivosAdministrador(nuevoId);
        guardarUsuariosEnArchivo();

        qDebug() << "Artista creado exitosamente. ID:" << nuevoId;
        return true;
    } catch (...) {
        qCritical() << "Error al crear artista";
        return false;
    }
}

bool Cuentas::desactivarCuenta(const QString& nombreUsuario) {
    Usuario* usuario = buscarUsuarioPorUsername(nombreUsuario);
    if (usuario && usuario->estaActivo()) {
        usuario->setEstado(false);
        guardarUsuariosEnArchivo();
        return true;
    }
    return false;
}

bool Cuentas::editarUsuario(const QString& nombreUsuarioActual, const QString& nuevoNombreUsuario,
                            const QString& nuevaContrasena, const QString& nuevoNombreReal,
                            const QString& nuevoEmail, const QString& nuevaRutaImagen,
                            const QString& nuevoPais, const QString& nuevoGenero,
                            const QString& nuevaDescripcion) {
    Usuario* usuario = buscarUsuarioPorUsername(nombreUsuarioActual);
    if (!usuario) return false;

    // Verificar si el nuevo nombre de usuario ya existe
    if (nombreUsuarioActual != nuevoNombreUsuario && existeUsuario(nuevoNombreUsuario)) {
        return false;
    }

    // Actualizar datos básicos
    if (!nuevoNombreUsuario.isEmpty()) {
        usernameAId.remove(usuario->getNombreUsuario());
        usuario->setNombreUsuario(nuevoNombreUsuario);
        usernameAId[nuevoNombreUsuario] = usuario->getId();
    }

    if (!nuevaContrasena.isEmpty()) {
        usuario->setContrasena(nuevaContrasena);
    }

    // Actualizar datos específicos según el tipo de usuario
    if (usuario->getTipo() == "ADMIN") {
        Administrador* artista = dynamic_cast<Administrador*>(usuario);
        if (artista) {
            if (!nuevoPais.isEmpty()) artista->setPais(nuevoPais);
            if (!nuevoGenero.isEmpty()) artista->setGenero(nuevoGenero);
            if (!nuevaDescripcion.isEmpty()) artista->setDescripcion(nuevaDescripcion);
        }
    } else {
        Estandar* user = dynamic_cast<Estandar*>(usuario);
        if (user) {
            if (!nuevoEmail.isEmpty()) user->setEmail(nuevoEmail);
        }
    }

    if (!nuevoNombreReal.isEmpty()) usuario->setNombreReal(nuevoNombreReal);
    if (!nuevaRutaImagen.isEmpty()) usuario->setRutaImagen(nuevaRutaImagen);

    guardarUsuariosEnArchivo();
    return true;
}

// -------------------------------
// Métodos públicos de gestión de álbumes
// -------------------------------
// -------------------------------
// Métodos para gestión de álbumes
// -------------------------------

bool Cuentas::eliminarAlbum(int albumId) {
    if (!albumes.contains(albumId)) return false;

    // Eliminar de los índices
    Album* album = albumes[albumId];
    albumesPorTitulo[album->getNombre()].removeAll(albumId);
    albumesPorUsuario[album->getIdArtista()].removeAll(albumId);
    albumesPorTipo[album->getTipoString()].removeAll(albumId);

    // Eliminar el álbum
    delete albumes.take(albumId);

    // Reescribir el archivo completo
    guardarAlbumesEnArchivo();

    return true;
}

bool Cuentas::actualizarAlbum(int albumId, const QString& nuevoNombre,
                              const QString& nuevaPortada, const QString& nuevoTipo) {
    if (!albumes.contains(albumId)) return false;

    Album* album = albumes[albumId];

    // Actualizar índices si cambió el nombre
    if (!nuevoNombre.isEmpty() && nuevoNombre != album->getNombre()) {
        albumesPorTitulo[album->getNombre()].removeAll(albumId);
        album->setNombre(nuevoNombre);
        albumesPorTitulo[nuevoNombre].append(albumId);
    }

    // Actualizar portada si se proporciona
    if (!nuevaPortada.isEmpty()) {
        album->setPortada(nuevaPortada);
    }

    // Actualizar tipo si se proporciona
    if (!nuevoTipo.isEmpty()) {
        albumesPorTipo[album->getTipoString()].removeAll(albumId);
        album->setTipoFromString(nuevoTipo);
        albumesPorTipo[nuevoTipo].append(albumId);
    }

    guardarAlbumesEnArchivo();
    return true;
}
bool Cuentas::crearAlbum(int userId, const QString& nombre, const QString& portada) {
    if (!usuarios.contains(userId)) return false;

    int nuevoId = ++ultimoIdLista;
    Album* nuevoAlbum = new Album(nuevoId, nombre, userId, portada);
    albumes[nuevoId] = nuevoAlbum;
    albumesPorTitulo[nombre].append(nuevoId);
    albumesPorUsuario[userId].append(nuevoId);
    //albumesPorTipo[nuevoAlbum->getTipoString()].append(nuevoId);

    // Guardar en archivo
    QFile file(ARCHIVO_ALBUMES);
    if (file.open(QIODevice::Append)) {
        QDataStream out(&file);
        out << *nuevoAlbum;
        return true;
    }

    return false;
}

bool Cuentas::agregarCancionAlbum(int albumId, const Cancion& cancion) {
    if (!albumes.contains(albumId)) {
        qDebug() << "Álbum no encontrado:" << albumId;
        return false;
    }

    int cancionId = cancion.getId();

    // Si la canción no existe, crearla
    if (!canciones.contains(cancionId)) {
        Cancion* nuevaCancion = new Cancion(cancion);
        nuevaCancion->setId(++ultimoIdCancion);
        canciones[nuevaCancion->getId()] = nuevaCancion;
        cancionId = nuevaCancion->getId();

        // Actualizar índices
        cancionesPorTitulo[nuevaCancion->getTitulo()].append(cancionId);
        cancionesPorArtista[nuevaCancion->getArtista()].append(cancionId);
        cancionesPorGenero[nuevaCancion->getGenero()].append(cancionId);

        qDebug() << "Nueva canción creada con ID:" << cancionId;
    }

    albumes[albumId]->agregarCancion(cancionId);
    albumesPorTipo[albumes[albumId]->getTipoString()].append(albumId);
    guardarAlbumesEnArchivo();
    guardarCancionesEnArchivo();
    return true;
}

// -------------------------------
// Métodos públicos de gestión de playlists
// -------------------------------

bool Cuentas::crearPlaylist(int userId, const QString& nombre) {
    if (!usuarios.contains(userId)) return false;

    int nuevoId = ++ultimoIdLista;
    Playlist* nuevaPlaylist = new Playlist(nuevoId, nombre, userId);
    playlists[nuevoId] = nuevaPlaylist;
    playlistsPorUsuario[userId].append(nuevoId);
    playlistsPorTitulo[nombre].append(nuevoId);

    // Guardar en archivo
    QFile file(ARCHIVO_PLAYLISTS);
    if (file.open(QIODevice::Append)) {
        QDataStream out(&file);
        out << *nuevaPlaylist;
        return true;
    }

    return false;
}

bool Cuentas::agregarCancionPlaylist(int playlistId, int cancionId) {
    if (!playlists.contains(playlistId) || !canciones.contains(cancionId)) return false;

    playlists[playlistId]->agregarCancion(cancionId);
    guardarPlaylistsEnArchivo();
    return true;
}

// -------------------------------
// Métodos públicos de gestión de canciones
// -------------------------------

bool Cuentas::eliminarCancion(int cancionId) {
    if (!canciones.contains(cancionId)) return false;

    // Eliminar de todos los álbumes y playlists
    for (Album* album : albumes) {
        album->eliminarCancion(cancionId);
    }
    for (Playlist* playlist : playlists) {
        playlist->eliminarCancion(cancionId);
    }

    // Eliminar de los índices
    Cancion* cancion = canciones[cancionId];
    cancionesPorTitulo[cancion->getTitulo()].removeAll(cancionId);
    cancionesPorArtista[cancion->getArtista()].removeAll(cancionId);
    cancionesPorGenero[cancion->getGenero()].removeAll(cancionId);

    delete canciones.take(cancionId);
    guardarCancionesEnArchivo();
    guardarAlbumesEnArchivo();
    guardarPlaylistsEnArchivo();

    return true;
}

bool Cuentas::actualizarCancion(int id, const QString &nuevoTitulo,
                                const QString &nuevoGenero,
                                const QString &nuevaRutaAudio) {
    if (!canciones.contains(id)) return false;

    Cancion* cancion = canciones[id];
    cancion->setTitulo(nuevoTitulo);
    cancion->setGenero(nuevoGenero);
    cancion->setRutaAudio(nuevaRutaAudio);

    guardarCancionesEnArchivo(); // sigue siendo privado pero se llama desde dentro de la clase
    return true;
}




// -------------------------------
// Métodos públicos de búsqueda
// -------------------------------

Usuario* Cuentas::buscarUsuarioPorId(int id)const {
    return usuarios.value(id, nullptr);
}

Usuario* Cuentas::buscarUsuarioPorUsername(const QString& username) {
    return usernameAId.contains(username) ? usuarios[usernameAId[username]] : nullptr;
}

bool Cuentas::existeUsuario(const QString& nombreUsuario) const {
    return usernameAId.contains(nombreUsuario);
}

Cancion* Cuentas::buscarCancionPorId(int id) {
    return canciones.value(id, nullptr);
}

QList<Cancion*> Cuentas::buscarCancionesPorTitulo(const QString& titulo) {
    QList<Cancion*> resultado;
    for (int id : cancionesPorTitulo.value(titulo)) {
        resultado.append(canciones.value(id));
    }
    return resultado;
}

QList<Cancion*> Cuentas::buscarCancionesPorArtista(const QString& artista) {
    QList<Cancion*> resultado;
    for (int id : cancionesPorArtista.value(artista)) {
        resultado.append(canciones.value(id));
    }
    return resultado;
}

QList<Cancion*> Cuentas::buscarCancionesPorGenero(const QString& genero){
    QList<Cancion*> resultado;
    for (int id : cancionesPorGenero.value(genero)) {
        resultado.append(canciones.value(id));
    }
    return resultado;
}

QList<Album*> Cuentas::buscarAlbumesPorNombre(const QString& nombre) {
    QList<Album*> resultado;
    for (int id : albumesPorTitulo.value(nombre)) {
        resultado.append(albumes.value(id));
    }
    return resultado;
}

QList<Album*> Cuentas::buscarAlbumesPorArtista(const int& artistaID) {
    QList<Album*> resultado;
    for (int id : albumesPorUsuario.value(artistaID)) {
        resultado.append(albumes.value(id));
    }
    return resultado;
}

QList<Album*> Cuentas::buscarAlbumesPorTipo(const QString& tipo) {
    QList<Album*> resultado;
    for (int id : albumesPorTipo.value(tipo)) {
        resultado.append(albumes.value(id));
    }
    return resultado;
}

QList<Playlist*> Cuentas::buscarPlaylistsPorNombre(const QString& nombre) {
    QList<Playlist*> resultado;
    for (int id : playlistsPorTitulo.value(nombre)) {
        resultado.append(playlists.value(id));
    }
    return resultado;
}
// -------------------------------
// Métodos para archivos de usuario
// -------------------------------

// Métodos para biblioteca (descargas)
bool Cuentas::agregarCancionABiblioteca(int userId, int cancionId) {
    if (!usuarios.contains(userId) || !canciones.contains(cancionId)) return false;

    QString archivo = QString("%1%2/biblioteca_%2.dat").arg(CARPETA_USUARIOS).arg(userId);
    return agregarIdAArchivo(archivo, cancionId);
}

bool Cuentas::eliminarCancionDeBiblioteca(int userId, int cancionId) {
    QString archivo = QString("%1%2/biblioteca_%2.dat").arg(CARPETA_USUARIOS).arg(userId);
    return eliminarIdDeArchivo(archivo, cancionId);
}

QList<int> Cuentas::obtenerBibliotecaUsuario(int userId) {
    QString archivo = QString("%1%2/biblioteca_%2.dat").arg(CARPETA_USUARIOS).arg(userId);
    return leerIdsDeArchivo(archivo);
}

// Métodos para playlists personales
bool Cuentas::agregarPlaylistAUsuario(int userId, int playlistId) {
    if (!usuarios.contains(userId) || !playlists.contains(playlistId)) return false;

    QString archivo = QString("%1%2/listas_%2.dat").arg(CARPETA_USUARIOS).arg(userId);
    return agregarIdAArchivo(archivo, playlistId);
}

bool Cuentas::eliminarPlaylistDeUsuario(int userId, int playlistId) {
    QString archivo = QString("%1%2/listas_%2.dat").arg(CARPETA_USUARIOS).arg(userId);
    return eliminarIdDeArchivo(archivo, playlistId);
}

QList<int> Cuentas::obtenerPlaylistsUsuario(int userId) {
    QString archivo = QString("%1%2/listas_%2.dat").arg(CARPETA_USUARIOS).arg(userId);
    return leerIdsDeArchivo(archivo);
}

// Métodos para favoritos
bool Cuentas::agregarCancionAFavoritos(int userId, int cancionId) {
    if (!usuarios.contains(userId) || !canciones.contains(cancionId)) return false;

    QString archivo = QString("%1%2/favoritos_%2.dat").arg(CARPETA_USUARIOS).arg(userId);
    return agregarIdAArchivo(archivo, cancionId);
}

bool Cuentas::eliminarCancionDeFavoritos(int userId, int cancionId) {
    QString archivo = QString("%1%2/favoritos_%2.dat").arg(CARPETA_USUARIOS).arg(userId);
    return eliminarIdDeArchivo(archivo, cancionId);
}

QList<int> Cuentas::obtenerFavoritosUsuario(int userId) {
    QString archivo = QString("%1%2/favoritos_%2.dat").arg(CARPETA_USUARIOS).arg(userId);
    return leerIdsDeArchivo(archivo);
}

// Métodos auxiliares para manejo de archivos de IDs
bool Cuentas::agregarIdAArchivo(const QString& archivo, int id) {
    QFile file(archivo);
    if (!file.open(QIODevice::Append)) return false;

    QDataStream out(&file);
    out << id;
    return true;
}

bool Cuentas::eliminarIdDeArchivo(const QString& archivo, int id) {
    QList<int> ids = leerIdsDeArchivo(archivo);
    ids.removeAll(id);

    QFile file(archivo);
    if (!file.open(QIODevice::WriteOnly)) return false;

    QDataStream out(&file);
    for (int currentId : ids) {
        out << currentId;
    }
    return true;
}

QList<int> Cuentas::leerIdsDeArchivo(const QString& archivo) {
    QList<int> ids;
    QFile file(archivo);

    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        while (!in.atEnd()) {
            int id;
            in >> id;
            ids.append(id);
        }
    }

    return ids;
}

// -------------------------------
// Métodos para estadísticas y reproducciones
// -------------------------------

// Métodos para registrar reproducciones
bool Cuentas::registrarReproduccion(int userId, int cancionId) {
    if (!usuarios.contains(userId) || !canciones.contains(cancionId)) return false;

    QString archivo = QString("%1%2/reproducciones_%2.dat").arg(CARPETA_USUARIOS).arg(userId);
    QFile file(archivo);
    if (!file.open(QIODevice::Append)) return false;

    QDataStream out(&file);
    out << cancionId << QDateTime::currentDateTime();
    return true;
}

// Métodos para registrar calificaciones
bool Cuentas::registrarCalificacion(int userId, int cancionId, int calificacion) {
    if (!usuarios.contains(userId) || !canciones.contains(cancionId) || calificacion < 1 || calificacion > 5) {
        return false;
    }

    QString archivo = QString("%1%2/calificaciones_%2.dat").arg(CARPETA_USUARIOS).arg(userId);
    QFile file(archivo);
    if (!file.open(QIODevice::Append)) return false;

    QDataStream out(&file);
    out << cancionId << calificacion << QDateTime::currentDateTime();
    return true;
}

// Métodos para obtener estadísticas personales
int Cuentas::totalCancionesEscuchadas(int userId) {
    QString archivo = QString("%1%2/reproducciones_%2.dat").arg(CARPETA_USUARIOS).arg(userId);
    QSet<int> cancionesUnicas;

    QFile file(archivo);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        while (!in.atEnd()) {
            int cancionId;
            QDateTime fecha;
            in >> cancionId >> fecha;
            cancionesUnicas.insert(cancionId);
        }
    }

    return cancionesUnicas.size();
}

QList<QPair<int, int>> Cuentas::cancionesMasEscuchadas(int userId, int limite) {
    QMap<int, int> contador;
    QString archivo = QString("%1%2/reproducciones_%2.dat").arg(CARPETA_USUARIOS).arg(userId);

    QFile file(archivo);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        while (!in.atEnd()) {
            int cancionId;
            QDateTime fecha;
            in >> cancionId >> fecha;
            contador[cancionId]++;
        }
    }

    // Convertir a lista y ordenar
    QList<QPair<int, int>> resultado;
    for (auto it = contador.begin(); it != contador.end(); ++it) {
        resultado.append(qMakePair(it.key(), it.value()));
    }

    // Ordenar descendente por cantidad de reproducciones
    std::sort(resultado.begin(), resultado.end(),
              [](const QPair<int, int>& a, const QPair<int, int>& b) {
                  return a.second > b.second;
              });

    return resultado.mid(0, limite);
}

double Cuentas::promedioCalificacionesUsuario(int userId) {
    QString archivo = QString("%1%2/calificaciones_%2.dat").arg(CARPETA_USUARIOS).arg(userId);
    double suma = 0;
    int cantidad = 0;

    QFile file(archivo);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        while (!in.atEnd()) {
            int cancionId, calificacion;
            QDateTime fecha;
            in >> cancionId >> calificacion >> fecha;
            suma += calificacion;
            cantidad++;
        }
    }

    return cantidad > 0 ? suma / cantidad : 0.0;
}

// Métodos para estadísticas globales (solo administradores)
QList<QPair<int, int>> Cuentas::cancionesMasEscuchadasGlobal(int limite) {
    QMap<int, int> contador;

    // Recorrer todos los usuarios
    for (Usuario* usuario : usuarios) {
        QString archivo = QString("%1%2/reproducciones_%2.dat").arg(CARPETA_USUARIOS).arg(usuario->getId());
        QFile file(archivo);
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);
            while (!in.atEnd()) {
                int cancionId;
                QDateTime fecha;
                in >> cancionId >> fecha;
                contador[cancionId]++;
            }
        }
    }

    // Convertir a lista y ordenar
    QList<QPair<int, int>> resultado;
    for (auto it = contador.begin(); it != contador.end(); ++it) {
        resultado.append(qMakePair(it.key(), it.value()));
    }

    // Ordenar descendente por cantidad de reproducciones
    std::sort(resultado.begin(), resultado.end(),
              [](const QPair<int, int>& a, const QPair<int, int>& b) {
                  return a.second > b.second;
              });

    return resultado.mid(0, limite);
}

QList<QPair<int, double>> Cuentas::cancionesMejorCalificadasGlobal(int limite) {
    QMap<int, QPair<double, int>> acumulador; // cancionId -> (suma, cantidad)

    // Recorrer todos los usuarios
    for (Usuario* usuario : usuarios) {
        QString archivo = QString("%1%2/calificaciones_%2.dat").arg(CARPETA_USUARIOS).arg(usuario->getId());
        QFile file(archivo);
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);
            while (!in.atEnd()) {
                int cancionId, calificacion;
                QDateTime fecha;
                in >> cancionId >> calificacion >> fecha;

                acumulador[cancionId].first += calificacion;
                acumulador[cancionId].second++;
            }
        }
    }

    // Convertir a lista de promedios y ordenar
    QList<QPair<int, double>> resultado;
    for (auto it = acumulador.begin(); it != acumulador.end(); ++it) {
        double promedio = it.value().second > 0 ? it.value().first / it.value().second : 0.0;
        resultado.append(qMakePair(it.key(), promedio));
    }

    // Ordenar descendente por calificación promedio
    std::sort(resultado.begin(), resultado.end(),
              [](const QPair<int, double>& a, const QPair<int, double>& b) {
                  return a.second > b.second;
              });

    return resultado.mid(0, limite);
}

QList<QPair<int, int>> Cuentas::usuariosMasActivos(int limite) {
    QMap<int, int> contador; // userId -> cantidad de reproducciones

    // Recorrer todos los usuarios
    for (Usuario* usuario : usuarios) {
        QString archivo = QString("%1%2/reproducciones_%2.dat").arg(CARPETA_USUARIOS).arg(usuario->getId());
        QFile file(archivo);
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);
            while (!in.atEnd()) {
                int cancionId;
                QDateTime fecha;
                in >> cancionId >> fecha;
                contador[usuario->getId()]++;
            }
        }
    }

    // Convertir a lista y ordenar
    QList<QPair<int, int>> resultado;
    for (auto it = contador.begin(); it != contador.end(); ++it) {
        resultado.append(qMakePair(it.key(), it.value()));
    }

    // Ordenar descendente por cantidad de reproducciones
    std::sort(resultado.begin(), resultado.end(),
              [](const QPair<int, int>& a, const QPair<int, int>& b) {
                  return a.second > b.second;
              });

    return resultado.mid(0, limite);
}

QMap<QString, double> Cuentas::promedioCalificacionesPorGenero() {
    QMap<QString, QPair<double, int>> acumulador; // género -> (suma, cantidad)

    // Recorrer todas las calificaciones de todos los usuarios
    for (Usuario* usuario : usuarios) {
        QString archivo = QString("%1%2/calificaciones_%2.dat").arg(CARPETA_USUARIOS).arg(usuario->getId());
        QFile file(archivo);
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);
            while (!in.atEnd()) {
                int cancionId, calificacion;
                QDateTime fecha;
                in >> cancionId >> calificacion >> fecha;

                if (canciones.contains(cancionId)) {
                    QString genero = canciones[cancionId]->getGenero();
                    acumulador[genero].first += calificacion;
                    acumulador[genero].second++;
                }
            }
        }
    }

    // Convertir a promedios
    QMap<QString, double> resultado;
    for (auto it = acumulador.begin(); it != acumulador.end(); ++it) {
        resultado[it.key()] = it.value().second > 0 ? it.value().first / it.value().second : 0.0;
    }

    return resultado;
}

// -------------------------------
// Métodos para obtener listas de canciones
// -------------------------------

QList<Cancion*> Cuentas::obtenerCancionesBiblioteca(int userId) {
    QList<int> ids = obtenerBibliotecaUsuario(userId);
    QList<Cancion*> resultado;

    for (int id : ids) {
        if (canciones.contains(id)) {
            resultado.append(canciones[id]);
        }
    }

    return resultado;
}

QList<Cancion*> Cuentas::obtenerCancionesFavoritas(int userId) {
    QList<int> ids = obtenerFavoritosUsuario(userId);
    QList<Cancion*> resultado;

    for (int id : ids) {
        if (canciones.contains(id)) {
            resultado.append(canciones[id]);
        }
    }

    return resultado;
}

QList<Cancion*> Cuentas::obtenerCancionesPlaylistsUsuario(int userId) {
    QList<int> playlistIds = obtenerPlaylistsUsuario(userId);
    QList<Cancion*> resultado;

    for (int playlistId : playlistIds) {
        if (playlists.contains(playlistId)) {
            for (int cancionId : playlists[playlistId]->getCanciones()) {
                if (canciones.contains(cancionId)) {
                    resultado.append(canciones[cancionId]);
                }
            }
        }
    }

    // Eliminar duplicados
    QSet<Cancion*> unicos(resultado.begin(), resultado.end());
    return QList<Cancion*>(unicos.begin(), unicos.end());
}

// -------------------------------
// Operadores de serialización
// -------------------------------



QDataStream& operator<<(QDataStream& out, const IndiceUsuario& indice) {
    out << indice.posicion << static_cast<qint8>(indice.esArtista);
    return out;
}

QDataStream& operator>>(QDataStream& in, IndiceUsuario& indice) {
    qint8 esArtistaTmp;
    in >> indice.posicion >> esArtistaTmp;
    indice.esArtista = static_cast<bool>(esArtistaTmp);
    return in;
}
