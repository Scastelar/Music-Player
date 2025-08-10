#ifndef ESTANDAR_H
#define ESTANDAR_H

#include "usuario.h"
#include <QStringList>

// Estructura para favoritos
struct Favorito {
    int idCancion;
    QDateTime fechaMarcado;
};

// Estructura para entradas de biblioteca
struct EntradaBiblioteca {
    int idCancion;
    QDateTime fechaAgregado;
};

// Estructura para listas de reproducción
struct ListaReproduccion {
    int idLista;
    QString nombre;
    QDateTime fechaCreacion;
    bool activa;
    QList<int> canciones;
};

class Estandar : public Usuario {
private:
    QString email;
    int ultimoIdPlaylist;

    void cargarUltimoIdPlaylist();
    void guardarUltimoIdPlaylist();

public:
    Estandar(const QString& username, const QString& password,
             const QString& nombreReal, const QString& email);


    // Métodos para biblioteca
    void agregarCancionABiblioteca(int cancionId);

    // Métodos para listas de reproducción
    int generarIdPlaylistUnico();
    void crearNuevaListaReproduccion(const QString& nombreLista);
    void agregarCancionALista(int idLista, int cancionId);

    // Métodos para favoritos
    void agregarCancionFavorita(int cancionId);
    bool eliminarCancionFavorita(int cancionId);
    QList<Favorito> obtenerCancionesFavoritas() const;
    bool esCancionFavorita(int cancionId) const;

    // Getters
    QString getEmail() const { return email; }

    // Serialización
    void escribirEnStream(QDataStream& stream) const override;
    void leerDesdeStream(QDataStream& stream) override;
};

#endif // ESTANDAR_H
