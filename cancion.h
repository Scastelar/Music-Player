#ifndef CANCION_H
#define CANCION_H

#include <QString>
#include <QDateTime>
#include <QDataStream>
#include <QFile>

class Cancion {
private:
    int id;
    QString titulo;
    QString artista;
    QString album;
    QString genero;
    QString categoria;
    QString duracion;
    QString rutaPortada;
    bool estado;
    QString rutaAudio;
    QDateTime fechaRegistro;

public:
    // Constructor vacío para serialización
    Cancion();

    // Constructor para creación de nuevas canciones    
    Cancion(int id, const QString& titulo, const QString& artista,
            const QString& album, const QString& genero, const QString& categoria,
            const QString& rutaArchivo = "", const QString& rutaImagen = "");


    // Getters
    int getId() const { return id; }
    QString getTitulo() const { return titulo; }
    QString getArtista() const { return artista; }
    QString getAlbum() const { return album; }
    QString getGenero() const { return genero; }
    QString getCategoria() const { return categoria; }
    QString getDuracion() const { return duracion; }
    QString getRutaPortada() const { return rutaPortada; }
    bool getEstado() const { return estado; }
    QString getRutaAudio() const { return rutaAudio; }
    QDateTime getFechaRegistro() const { return fechaRegistro; }

    // Setters
    void setId(const int& newID){ id = newID; }
    void setTitulo(const QString& newTitulo) { titulo = newTitulo; }
    void setArtista(const QString& newArtista) { artista = newArtista;}
    void setGenero(const QString& newGenero) { genero = newGenero; }
    void setAlbum(const QString& album){ this->album = album; }
    void setCategoria(const QString& newCategoria) { categoria = newCategoria; }
    void setDuracion(const QString& newDuracion) { duracion = newDuracion; }
    void setRutaPortada(const QString& newRuta) { rutaPortada = newRuta; }
    void setEstado(bool newEstado) { estado = newEstado; }
    void setRutaAudio(const QString& newRuta) { rutaAudio = newRuta; }
    void setfechaRegistro(const QDateTime& newFecha) { fechaRegistro = newFecha; }


    void guardarEnArchivo(const QString& nombreArchivo = "canciones.dat") const;
    static QList<Cancion> cargarDesdeArchivo(const QString& nombreArchivo = "canciones.dat");

    // Métodos de serialización
    void escribirEnStream(QDataStream& stream) const;
    void leerDesdeStream(QDataStream& stream);

    // Sobrecarga de operadores para serialización
    friend QDataStream& operator<<(QDataStream& out, const Cancion& cancion);
    friend QDataStream& operator>>(QDataStream& in, Cancion& cancion);
};

#endif // CANCION_H
