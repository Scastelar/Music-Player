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

    //Metodos propios
    QString getEmail() const { return email; }
    void setEmail(const QString& correo){ email = correo; }

    // Serialización
    friend QDataStream& operator<<(QDataStream& out, const Estandar& estandar);
    friend QDataStream& operator>>(QDataStream& in, Estandar& estandar);

    void escribirEnStream(QDataStream& stream) const override;
    void leerDesdeStream(QDataStream& stream) override;
};

#endif // ESTANDAR_H
