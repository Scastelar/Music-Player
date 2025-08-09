#ifndef CANCION_H
#define CANCION_H

#include <QString>
#include <QDateTime>
#include <QCryptographicHash>

class Cancion {
private:
    int id;
    QString titulo;
    QString artista;
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
    Cancion(const QString& titulo, const QString& genero, const QString& categoria,
            const QString& rutaPortada, const QString& rutaAudio, const QString& artista);

    // Getters
    int getId() const { return id; }
    QString getTitulo() const { return titulo; }
    QString getArtista() const { return artista; }
    QString getGenero() const { return genero; }
    QString getCategoria() const { return categoria; }
    QString getDuracion() const { return duracion; }
    QString getRutaPortada() const { return rutaPortada; }
    bool getEstado() const { return estado; }
    QString getRutaAudio() const { return rutaAudio; }
    QDateTime getFechaRegistro() const { return fechaRegistro; }

    // Setters
    void setTitulo(const QString& newTitulo) { titulo = newTitulo; }
    void setGenero(const QString& newGenero) { genero = newGenero; }
    void setCategoria(const QString& newCategoria) { categoria = newCategoria; }
    void setDuracion(const QString& newDuracion) { duracion = newDuracion; }
    void setRutaPortada(const QString& newRuta) { rutaPortada = newRuta; }
    void setEstado(bool newEstado) { estado = newEstado; }
    void setRutaAudio(const QString& newRuta) { rutaAudio = newRuta; }

    // Métodos para serialización
    void guardarEnArchivo(const QString& nombreArchivo = "canciones.dat") const;
    static QList<Cancion> cargarDesdeArchivo(const QString& nombreArchivo = "canciones.dat");

    // Métodos para operaciones con el archivo
    static bool existeCancion(int id, const QString& nombreArchivo = "canciones.dat");
    static bool eliminarCancion(int id, const QString& nombreArchivo = "canciones.dat");

    // Sobrecarga de operadores para serialización
    friend QDataStream& operator<<(QDataStream& out, const Cancion& cancion);
    friend QDataStream& operator>>(QDataStream& in, Cancion& cancion);
};

#endif // CANCION_H
