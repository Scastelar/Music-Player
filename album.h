#ifndef ALBUM_H
#define ALBUM_H

#include "ListaReproduccion.h"

class Album : public ListaReproduccion {
public:
    enum class Tipo {
        ALBUM,
        EP,
        SINGLE
    };

private:
    int idArtista;
    Tipo tipo;

public:
    Album(int id = 0, const QString& nombre = "", int artistaId = 0,
          const QString& portada = "")
        : ListaReproduccion(id, nombre, portada), idArtista(artistaId), tipo(Tipo::ALBUM) {}

    int getIdArtista() const { return idArtista; }
    Tipo getTipo() const {
        return tipo;
    }
    QString getTipoString() const override {
        if (canciones.size()>=7){
            return "ALBUM";
        } else if (canciones.size()<7 && canciones.size()>1){
            return "EP";
        }else {
            return "SINGLE";
        }
    }

    void setIdArtista(int id) { idArtista = id; }
    void setTipo(Tipo t) { tipo = t; }

    friend QDataStream& operator<<(QDataStream& out, const Album& album);
    friend QDataStream& operator>>(QDataStream& in, Album& album);
};

#endif // ALBUM_H
