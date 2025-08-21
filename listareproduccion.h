#ifndef LISTA_REPRODUCCION_H
#define LISTA_REPRODUCCION_H

#include <QString>
#include <QList>
#include <QDateTime>
#include <QDataStream>

class ListaReproduccion {
protected:
    int id;
    QString nombre;
    QList<int> canciones;       // IDs de canciones
    QString portada;            // ruta de imagen
    QDateTime fechaCreacion;    // fecha de creación

public:
    ListaReproduccion(int id = 0, const QString& nombre = "", const QString& portada = "")
        : id(id), nombre(nombre), portada(portada), fechaCreacion(QDateTime::currentDateTime()) {}

    virtual ~ListaReproduccion() = default;

    // Getters
    int getId() const { return id; }
    QString getNombre() const { return nombre; }
    QList<int> getCanciones() const { return canciones; }
    QString getPortada() const { return portada; }
    QDateTime getFechaCreacion() const { return fechaCreacion; }

    // Setters
    void setNombre(const QString& nuevo) { nombre = nuevo; }
    void setPortada(const QString& p) { portada = p; }

    void agregarCancion(int cancionId) {
        if (!canciones.contains(cancionId)) canciones.append(cancionId);
    }
    void eliminarCancion(int cancionId) {
        canciones.removeAll(cancionId);
    }

    // Virtual puro para identificar tipo
    virtual QString getTipoString() const = 0;

    // Serialización genérica
    friend QDataStream& operator<<(QDataStream& out, const ListaReproduccion& lista);
    friend QDataStream& operator>>(QDataStream& in, ListaReproduccion& lista);
};

#endif
