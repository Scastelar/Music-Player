#ifndef ALBUM_H
#define ALBUM_H

#include <QString>
#include <QList>
#include <QDateTime>
#include <QDataStream>



class Album {
public:
    enum class Tipo {
        ALBUM,    // Álbum completo
        EP,       // Extended Play (4-6 canciones)
        SINGLE    // Sencillo (1-3 canciones)
    };

    Album();
    Album(int id, const QString& nombre, int idArtista, const QString& rutaPortada);

    // Getters
    int getId() const { return id; }
    QString getNombre() const { return nombre; }
    int getIdArtista() const { return idArtista; }
    QDateTime getFechaCreacion() const { return fechaCreacion; }
    QList<int> getCanciones() const { return canciones; }
    Tipo getTipo() const { return tipo; }
    QString getTipoString() const;
    bool getEstado() const { return estado; }
    QString getPortada() const { return portada; }


    // Setters
    void setId(int id) { this->id = id; }
    void setNombre(const QString& nombre) { this->nombre = nombre; }
    void setIdArtista(int id) { this->idArtista = id; }
    void setTipo(Tipo tipo) { this->tipo = tipo; }
    void setEstado(bool newEstado) { estado = newEstado; }
    void setPortada(QString& ruta) { portada = ruta; }


    // Gestión de canciones
    void agregarCancion(int idCancion);
    void eliminarCancion(int idCancion);
    bool contieneCancion(int idCancion) const;

    // Validación de tipo según cantidad de canciones
    bool validarTipo() const;

    // Serialización
    void escribirEnStream(QDataStream& stream) const;
    void leerDesdeStream(QDataStream& stream);

    friend QDataStream& operator<<(QDataStream& out, const Album& album);
    friend QDataStream& operator>>(QDataStream& in, Album& album);

private:
    int id;
    QString nombre;
    int idArtista;
    QList<int> canciones;
    Tipo tipo;
    bool estado;
    QString portada;
    QDateTime fechaCreacion;

    Tipo determinarTipo(int numCanciones) const;
};

#endif // ALBUM_H
