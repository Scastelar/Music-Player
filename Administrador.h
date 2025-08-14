#ifndef ADMINISTRADOR_H
#define ADMINISTRADOR_H

#include "usuario.h"

class Administrador : public Usuario {
private:
    QString pais;
    QString genero;
    QString descripcion;

public:
    Administrador(const QString& username, const QString& password, const QString& nombre,const QString& pais, const QString& genero, const QString& desc);

    //MEtodos propios
    QString getPais() const { return pais; }
    QString getGenero() const { return genero; }
    QString getDescripcion() const { return descripcion; }

    void setPais(const QString& region){ pais = region; }
    void setGenero(const QString& gen){ genero = gen; }
    void setDescripcion(const QString& desc){ descripcion = desc; }

    //Serializacion
    friend QDataStream& operator<<(QDataStream& out, const Administrador& admin);
    friend QDataStream& operator>>(QDataStream& in,Administrador& admin);

    void escribirEnStream(QDataStream& stream) const override;
    void leerDesdeStream(QDataStream& stream) override;
};

#endif // ADMINISTRADOR_H
