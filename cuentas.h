#ifndef CUENTAS_H
#define CUENTAS_H

#include "usuario.h"
#include "Estandar.h"
#include "Administrador.h"
#include "cancion.h"
#include "album.h"
#include "playlist.h"
#include <QFile>
#include <QDir>
#include <QHash>
#include <QDataStream>
#include <limits>
#include <QDateTime>

struct IndiceUsuario {
    qint64 posicion;      // Posición en el archivo (usuarios.dat o artistas.dat)
    bool esArtista;
};

// Métodos de serialización
QDataStream& operator<<(QDataStream& out, const IndiceUsuario& obj);
QDataStream& operator>>(QDataStream& in, IndiceUsuario& obj);

class Cuentas {
private:
    // Constantes de archivos
    const QString CARPETA_USUARIOS = "usuarios/";
    const QString ARCHIVO_USUARIOS = "usuarios.dat";
    const QString ARCHIVO_ARTISTAS = "artistas.dat";
    const QString ARCHIVO_ALBUMES = "albumes.dat";
    const QString ARCHIVO_PLAYLISTS = "playlists.dat";
    const QString ARCHIVO_CANCIONES = "canciones.dat";
    const QString ARCHIVO_INDICES = "indices.dat";

    // Hashes principales
    QHash<int, Usuario*> usuarios;          // ID → Usuario*
    QHash<int, Cancion*> canciones;         // ID → Cancion*
    QHash<int, Album*> albumes;
    QHash<int, Playlist*> playlists;
    QHash<QString, IndiceUsuario> indices;  // Índices para archivos

    // Índices secundarios
    QHash<QString, int> usernameAId;       // Username → ID usuario

    // CANCIONES
    QHash<QString, QList<int>> cancionesPorTitulo;  // Título → IDs de canciones
    QHash<QString, QList<int>> cancionesPorArtista; // Artista → IDs de canciones
    QHash<QString, QList<int>> cancionesPorGenero;  // Genero → IDs de canciones

    // ALBUMES
    QHash<QString, QList<int>> albumesPorTitulo;   // Título → IDs de álbumes
    QHash<QString, QList<int>> albumesPorTipo;    // Tipo → IDs de álbumes
    QHash<int, QList<int>> albumesPorUsuario;     // UserID → IDs de sus álbumes

    // PLAYLISTS
    QHash<QString, QList<int>> playlistsPorTitulo; // Título → IDs de playlists
    QHash<int, QList<int>> playlistsPorUsuario;    // UserID → IDs de sus playlists

    // Variables de estado
    int idUsuarioActual;
    int ultimoIdUsuario;
    int ultimoIdCancion;
    int ultimoIdLista; // Para álbumes/playlists

    // Métodos privados de gestión de archivos
    void inicializarArchivos();
    void cargarDatos();
    void guardarDatos();
    void cargarUsuariosDesdeArchivo();

    void cargarArchivoUsuarios(const QString& archivo, bool esArtista);
    void guardarArchivoUsuarios(const QString& archivo, bool esArtista);
    void guardarUsuarioEnArchivo(Usuario* usuario);
    void guardarUsuariosEnArchivo();

    void cargarCancionesDesdeArchivo();
    void guardarCancionesEnArchivo();

    void cargarAlbumesDesdeArchivo();
    void guardarAlbumesEnArchivo();

    void cargarPlaylistsDesdeArchivo();
    void guardarPlaylistsEnArchivo();

    // Métodos privados auxiliares
    int generarIdUnico(bool esCancion = false, bool esLista = false);
    int generarNuevoId();
    QString obtenerRutaArchivoUsuario(int userId, const QString& tipo) const;
    bool agregarIdAArchivo(const QString& archivo, int id);
    bool eliminarIdDeArchivo(const QString& archivo, int id);
    QList<int> leerIdsDeArchivo(const QString& archivo);

    // Métodos privados de creación de archivos de usuario
    void crearArchivosUsuarioEstandar(int userId);
    void crearArchivosAdministrador(int userId);

public:
    // Constructor y destructor
    Cuentas();
    ~Cuentas();

    // Métodos de autenticación
    Usuario* autenticar(const QString& nombreUsuario, const QString& contrasena);
    void cerrarSesion();
    Usuario* getIdUsuarioActual() const;
    bool setIdUsuarioActual(int nuevoId);
    void verificarAutenticacion(const QString& username, const QString& password);

    // Métodos de gestión de cuentas
    bool crearUsuarioNormal(const QString& nombreUsuario, const QString& contrasena,
                            const QString& nombreReal, const QString& email,
                            const QString& rutaImagen = "");

    bool crearArtista(const QString& nombreArtistico, const QString& contrasena,
                      const QString& pais, const QString& genero, const QString& descripcion,
                      const QString& nombreReal, const QString& rutaImagen = "");
    bool desactivarCuenta(const QString& nombreUsuario);

    bool editarUsuario(const QString& nombreUsuarioActual, const QString& nuevoNombreUsuario,
                       const QString& nuevaContrasena, const QString& nuevoNombreReal,
                       const QString& nuevoEmail, const QString& nuevaRutaImagen,
                       const QString& nuevoPais, const QString& nuevoGenero,
                       const QString& nuevaDescripcion);

    // Métodos de gestión de álbumes
    bool crearAlbum(int userId, const QString& nombre, const QString& portada);
    bool agregarCancionAlbum(int albumId, const Cancion& cancion);
    bool eliminarAlbum(int albumId);
    bool actualizarAlbum(int albumId, const QString& nuevoNombre,
                          QString& nuevaPortada);
    int getUltimoIdLista() { return ultimoIdLista; }

    // Métodos de gestión de playlists
    bool crearPlaylist(int userId, const QString& nombre);
    bool agregarCancionPlaylist(int playlistId, int cancionId);

    // Métodos de gestión de canciones
    bool eliminarCancion(int cancionId);
    bool actualizarCancion(int id, const QString& nuevoTitulo,
                           const QString& nuevoGenero,
                           const QString& nuevaRutaAudio);

    // Métodos para archivos de usuario (biblioteca, playlists, favoritos)
    bool agregarCancionABiblioteca(int userId, int cancionId);
    bool eliminarCancionDeBiblioteca(int userId, int cancionId);
    QList<int> obtenerBibliotecaUsuario(int userId);

    bool agregarPlaylistAUsuario(int userId, int playlistId);
    bool eliminarPlaylistDeUsuario(int userId, int playlistId);
    QList<int> obtenerPlaylistsUsuario(int userId);

    bool agregarCancionAFavoritos(int userId, int cancionId);
    bool eliminarCancionDeFavoritos(int userId, int cancionId);
    QList<int> obtenerFavoritosUsuario(int userId);

    // Métodos para obtener listas de canciones
    QList<Cancion*> obtenerCancionesBiblioteca(int userId);
    QList<Cancion*> obtenerCancionesFavoritas(int userId);
    QList<Cancion*> obtenerCancionesPlaylistsUsuario(int userId);

    // Métodos para estadísticas y reproducciones
    bool registrarReproduccion(int userId, int cancionId);
    bool registrarCalificacion(int userId, int cancionId, int calificacion);

    // Estadísticas personales
    int totalCancionesEscuchadas(int userId);
    QList<QPair<int, int>> cancionesMasEscuchadas(int userId, int limite = 10);
    double promedioCalificacionesUsuario(int userId);

    // Estadísticas globales (solo administradores)
    QList<QPair<int, int>> cancionesMasEscuchadasGlobal(int limite = 10);
    QList<QPair<int, double>> cancionesMejorCalificadasGlobal(int limite = 10);
    QList<QPair<int, int>> usuariosMasActivos(int limite = 10);
    QMap<QString, double> promedioCalificacionesPorGenero();

    // Métodos de búsqueda
    Usuario* buscarUsuario(const QString& nombreUsuario) const;
    bool existeUsuario(const QString& nombreUsuario) const;
    Usuario* buscarUsuarioPorId(int id) const;
    Usuario* buscarUsuarioPorUsername(const QString& username);
    Cancion* buscarCancionPorId(int id);
    QList<Cancion*> buscarCancionesPorTitulo(const QString& titulo);
    QList<Cancion*> buscarCancionesPorArtista(const QString& artista);
    QList<Cancion*> buscarCancionesPorGenero(const QString& genero);
    QList<Album*> buscarAlbumesPorNombre(const QString& nombre);
    QList<Album*> buscarAlbumesPorTipo(const QString& tipo);
    QList<Album*> buscarAlbumesPorArtista(const int& artista);
    QList<Playlist*> buscarPlaylistsPorNombre(const QString& nombre);
};

#endif // CUENTAS_H
