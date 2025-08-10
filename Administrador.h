#ifndef ADMINISTRADOR_H
#define ADMINISTRADOR_H

#include "usuario.h"


// Estructura para álbums
struct Album {
    int idAlbum;
    QString nombre;
    QDateTime fechaCreacion;
    bool activo;
    QString tipoAlbum;  // "single", "EP" o "album"
    QList<int> canciones;
};

class Administrador : public Usuario {
private:
    QString pais;
    QString genero;
    QString descripcion;
     int ultimoIdAlbum;

    void cargarUltimoIdAlbum();
    void guardarUltimoIdAlbum();
public:
    Administrador(const QString& username, const QString& password, const QString& pais, const QString& genero, const QString& desc);

    //MEtodos propios
    QString getPais() const { return pais; }
    QString getGenero() const { return genero; }
    QString getDescripcion() const { return descripcion; }

    // Métodos para listas de reproducción
    int generarIdAlbumUnico();
    void crearNuevoAlbum(const QString& nombreAlbum);
    void agregarCancionAlbum(int idLista, int cancionId);

    void agregarCancionCatalogo(const QString& titulo, const QString& genero,
                                const QString& categoria, const QString& rutaPortada,
                                const QString& rutaAudio);
    QList<Cancion> obtenerCancionesCatalogo() const;
    bool eliminarCancionCatalogo(int idCancion);


    //Serializacion
    void escribirEnStream(QDataStream& stream) const override;
    void leerDesdeStream(QDataStream& stream) override;
};

#endif // ADMINISTRADOR_H
