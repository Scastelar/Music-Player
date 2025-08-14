#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QString>
#include <QList>
#include <QDateTime>
#include <QDataStream>

class Playlist {
public:
    // Constructores
    Playlist();
    Playlist(int id, const QString& nombre, int idUsuario);

    // Getters
    int getId() const;
    QString getNombre() const;
    int getIdUsuario() const;
    QDateTime getFechaCreacion() const;
    QList<int> getCanciones() const;
    int getCantidadCanciones() const;

    // Setters
    void setId(int id);
    void setNombre(const QString& nombre);
    void setIdUsuario(int id);

    // Gestión de canciones
    void agregarCancion(int idCancion);
    void eliminarCancion(int idCancion);
    bool contieneCancion(int idCancion) const;
    void moverCancion(int posActual, int nuevaPos);
    void limpiarPlaylist();

    // Serialización
    void escribirEnStream(QDataStream& stream) const;
    void leerDesdeStream(QDataStream& stream);

    friend QDataStream& operator<<(QDataStream& out, const Playlist& playlist);
    friend QDataStream& operator>>(QDataStream& in, Playlist& playlist);

private:
    int id;
    QString nombre;
    int idUsuario;
    QList<int> canciones;
    QDateTime fechaCreacion;

};

#endif // PLAYLIST_H
