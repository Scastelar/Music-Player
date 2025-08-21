#ifndef CUENTAS_H
#define CUENTAS_H

#include "usuario.h"
#include "Estandar.h"
#include "Administrador.h"
#include "cancion.h"
#include "ListaReproduccion.h"
#include "album.h"
#include "playlist.h"

#include <QFile>
#include <QDir>
#include <QHash>
#include <QMap>
#include <QDataStream>
#include <limits>
#include <QDateTime>

struct IndiceUsuario {
    qint64 posicion;      // Posición en archivo
    bool esArtista;
};

// Serialización de índices
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

    void reconstruirIndices();

    // Datos principales
    QHash<int, Usuario*> usuarios;          // ID → Usuario*
    QHash<int, Cancion*> canciones;         // ID → Cancion*
    QHash<int, Album*> albumes;             // ID → Album*
    QHash<int, Playlist*> playlists;        // ID → Playlist*
    QHash<QString, IndiceUsuario> indices;  // índices para archivos de usuarios

    // Índices secundarios
    QHash<QString, int> usernameAId;       // Username → ID usuario

    // Canciones
    QHash<QString, QList<int>> cancionesPorTitulo;
    QHash<QString, QList<int>> cancionesPorArtista;
    QHash<QString, QList<int>> cancionesPorGenero;

    // Álbumes
    QHash<QString, QList<int>> albumesPorTitulo;
    QHash<QString, QList<int>> albumesPorTipo;
    QHash<int, QList<int>> albumesPorUsuario;

    // Playlists
    QHash<QString, QList<int>> playlistsPorTitulo;
    QHash<int, QList<int>> playlistsPorUsuario;

    // Variables de estado
    int idUsuarioActual;
    int ultimoIdUsuario;
    int ultimoIdCancion;
    int ultimoIdLista;

    // ---- Métodos privados ----
    void inicializarArchivos();
    void cargarUsuariosDesdeArchivo();
    void guardarUsuariosEnArchivo();

    void cargarArchivoUsuarios(const QString& archivo, bool esArtista);
    void guardarArchivoUsuarios(const QString& archivo, bool esArtista);

    void cargarCancionesDesdeArchivo();
    void guardarCancionesEnArchivo();

    // Métodos genéricos de listas
    template<typename T>
    void cargarListasDesdeArchivo(const QString& archivo, QHash<int, T*>& contenedor);

    template<typename T>
    void guardarListasEnArchivo(const QString& archivo, const QHash<int, T*>& contenedor);

    // Métodos auxiliares de archivos de usuario
    int generarNuevoId();
    QString obtenerRutaArchivoUsuario(int userId, const QString& tipo) const;
    bool agregarIdAArchivo(const QString& archivo, int id);
    bool eliminarIdDeArchivo(const QString& archivo, int id);
    QList<int> leerIdsDeArchivo(const QString& archivo);

    // Crear estructura de carpetas/archivos por usuario
    void crearArchivosUsuarioEstandar(int userId);
    void crearArchivosAdministrador(int userId);

public:
    // Constructor y destructor
    Cuentas();
    ~Cuentas();

    // --- Autenticación ---
    Usuario* autenticar(const QString& nombreUsuario, const QString& contrasena);
    void cerrarSesion();
    Usuario* getIdUsuarioActual() const;
    bool setIdUsuarioActual(int nuevoId);
    void verificarAutenticacion(const QString& username, const QString& password);

    // --- Gestión de cuentas ---
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

    // --- Gestión de listas ---
    bool crearAlbum(int userId, const QString& nombre, const QString& portada,
                    Album::Tipo tipo = Album::Tipo::ALBUM);
    bool crearPlaylist(int userId, const QString& nombre, const QString& portada = "");
    bool agregarCancionALista(ListaReproduccion* lista, int cancionId);
    bool eliminarAlbum(int albumId);
    bool actualizarAlbum(int albumId, const QString& nuevoNombre, QString nuevaPortada);

    // --- Gestión de canciones ---
    bool crearCancion(int userId, const QString& titulo, const QString& categoria,
                     const QString& username,const QString& genero, const QDateTime& fecha,
                     const QString& duracion,const QString& rutaAudio);
    bool eliminarCancion(int cancionId);
    bool actualizarCancion(int id, const QString& nuevoTitulo,
                           const QString& nuevoGenero,
                           const QString& nuevaRutaAudio);

    // --- Archivos de usuario ---
    bool agregarCancionABiblioteca(int userId, int cancionId);
    bool eliminarCancionDeBiblioteca(int userId, int cancionId);
    QList<int> obtenerBibliotecaUsuario(int userId);

    bool agregarPlaylistAUsuario(int userId, int playlistId);
    bool eliminarPlaylistDeUsuario(int userId, int playlistId);
    QList<int> obtenerPlaylistsUsuario(int userId);

    bool agregarCancionAFavoritos(int userId, int cancionId);
    bool eliminarCancionDeFavoritos(int userId, int cancionId);
    QList<int> obtenerFavoritosUsuario(int userId);

    // --- Obtener canciones de listas ---
    QList<Cancion*> obtenerCancionesBiblioteca(int userId);
    QList<Cancion*> obtenerCancionesFavoritas(int userId);
    QList<Cancion*> obtenerCancionesPlaylistsUsuario(int userId);

    // --- Estadísticas ---
    bool registrarReproduccion(int userId, int cancionId);
    bool registrarCalificacion(int userId, int cancionId, int calificacion);

    int totalCancionesEscuchadas(int userId);
    QList<QPair<int, int>> cancionesMasEscuchadas(int userId, int limite = 10);
    double promedioCalificacionesUsuario(int userId);

    QList<QPair<int, int>> cancionesMasEscuchadasGlobal(int limite = 10);
    QList<QPair<int, double>> cancionesMejorCalificadasGlobal(int limite = 10);
    QList<QPair<int, int>> usuariosMasActivos(int limite = 10);
    QMap<QString, double> promedioCalificacionesPorGenero();

    // --- Búsqueda ---
    Usuario* buscarUsuario(const QString& nombreUsuario) const;
    bool existeUsuario(const QString& nombreUsuario) const;
    Usuario* buscarUsuarioPorId(int id) const;
    Usuario* buscarUsuarioPorUsername(const QString& username);

    Cancion* buscarCancionPorId(int id);
    QList<Cancion*> buscarCancionesPorTitulo(const QString& titulo);
    QList<Cancion*> buscarCancionesPorArtista(const QString& artista);
    QList<Cancion*> buscarCancionesPorGenero(const QString& genero);

    Album* buscarAlbumPorId(int id) const;
    QList<Album*> buscarAlbumesPorNombre(const QString& nombre);
    QList<Album*> buscarAlbumesPorTipo(const QString& tipo);
    QList<Album*> buscarAlbumesPorArtista(const int& artista);

    QList<Playlist*> buscarPlaylistsPorNombre(const QString& nombre);

    int getUltimoIdLista(){ return ultimoIdLista; }
    int getUltimoIdCancion(){ return ultimoIdCancion; }
};

#endif // CUENTAS_H
