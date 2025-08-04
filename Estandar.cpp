#include "estandar.h"
#include <QDataStream>
#include <QFile>
#include <QDateTime>
#include <random>

Estandar::Estandar(const QString& username, const QString& password,
                   const QString& nombreReal, const QString& email) :
    Usuario(username, password),
    email(email),
    ultimoIdPlaylist(0) {  // Inicializar contador de playlists
    this->nombreReal = nombreReal;
    //this->rutaImagen = rutaImagen;
    cargarUltimoIdPlaylist();  // Cargar el último ID usado al crear el objeto
}

int Estandar::generarIdPlaylistUnico() {
    // Incrementar el contador y guardarlo
    ultimoIdPlaylist++;
    guardarUltimoIdPlaylist();

    // Generar un ID único combinando timestamp y contador
    int timestamp = QDateTime::currentDateTime().toSecsSinceEpoch() % 1000000;
    int uniqueId = (timestamp << 16) | (ultimoIdPlaylist & 0xFFFF);

    return uniqueId;
}

void Estandar::cargarUltimoIdPlaylist() {
    QString filename = QString("playlist_counter_%1.dat").arg(this->getId());
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);
        in >> ultimoIdPlaylist;
    }
}

void Estandar::guardarUltimoIdPlaylist() {
    QString filename = QString("playlist_counter_%1.dat").arg(this->getId());
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        QDataStream out(&file);
        out << ultimoIdPlaylist;
    }
}

void Estandar::agregarCancionABiblioteca(int cancionId) {
    QString filename = QString("biblioteca_%1.dat").arg(this->getId());
    QFile file(filename);
    if (file.open(QIODevice::Append)) {
        QDataStream out(&file);
        EntradaBiblioteca entrada;
        entrada.idCancion = cancionId;
        entrada.fechaAgregado = QDateTime::currentDateTime();
        out << entrada.idCancion << entrada.fechaAgregado;
        file.close();
    }
}

void Estandar::crearNuevaListaReproduccion(const QString& nombreLista) {
    QString filename = QString("listas_%1.dat").arg(this->getId());
    QFile file(filename);
    if (file.open(QIODevice::Append)) {
        QDataStream out(&file);
        ListaReproduccion lista;
        lista.idLista = generarIdPlaylistUnico();
        lista.nombre = nombreLista;
        lista.fechaCreacion = QDateTime::currentDateTime();
        lista.activa = true;
        lista.canciones = QList<int>(); // Lista vacía inicialmente

        out << lista.idLista << lista.nombre << lista.fechaCreacion
            << lista.activa << lista.canciones;
        file.close();
    }
}

void Estandar::agregarCancionALista(int idLista, int cancionId) {
    QString filename = QString("listas_%1.dat").arg(this->getId());
    QFile file(filename);
    QFile tempFile(filename + ".tmp");

    if (file.open(QIODevice::ReadOnly) && tempFile.open(QIODevice::WriteOnly)) {
        QDataStream in(&file);
        QDataStream out(&tempFile);
        bool encontrada = false;

        while (!in.atEnd()) {
            ListaReproduccion lista;
            in >> lista.idLista >> lista.nombre >> lista.fechaCreacion
                >> lista.activa >> lista.canciones;

            if (lista.idLista == idLista && lista.activa) {
                if (!lista.canciones.contains(cancionId)) {
                    lista.canciones.append(cancionId);
                    encontrada = true;
                }
            }

            out << lista.idLista << lista.nombre << lista.fechaCreacion
                << lista.activa << lista.canciones;
        }

        file.close();
        tempFile.close();

        if (encontrada) {
            QFile::remove(filename);
            QFile::rename(filename + ".tmp", filename);
        } else {
            QFile::remove(filename + ".tmp");
        }
    }
}

void Estandar::agregarCancionFavorita(int cancionId) {
    QString filename = QString("favoritos_%1.dat").arg(this->getId());
    QFile file(filename);

    // Verificar si ya existe como favorita primero
    if (esCancionFavorita(cancionId)) {
        return;
    }

    if (file.open(QIODevice::Append)) {
        QDataStream out(&file);
        Favorito favorito;
        favorito.idCancion = cancionId;
        favorito.fechaMarcado = QDateTime::currentDateTime();
        out << favorito.idCancion << favorito.fechaMarcado;
        file.close();
    }
}

bool Estandar::eliminarCancionFavorita(int cancionId) {
    QString filename = QString("favoritos_%1.dat").arg(this->getId());
    QFile file(filename);
    QFile tempFile(filename + ".tmp");
    bool eliminado = false;

    if (file.open(QIODevice::ReadOnly) && tempFile.open(QIODevice::WriteOnly)) {
        QDataStream in(&file);
        QDataStream out(&tempFile);

        while (!in.atEnd()) {
            Favorito favorito;
            in >> favorito.idCancion >> favorito.fechaMarcado;

            if (favorito.idCancion != cancionId) {
                out << favorito.idCancion << favorito.fechaMarcado;
            } else {
                eliminado = true;
            }
        }

        file.close();
        tempFile.close();

        if (eliminado) {
            QFile::remove(filename);
            QFile::rename(filename + ".tmp", filename);
            return true;
        } else {
            QFile::remove(filename + ".tmp");
            return false;
        }
    }
    return false;
}

QList<Favorito> Estandar::obtenerCancionesFavoritas() const {
    QList<Favorito> favoritos;
    QString filename = QString("favoritos_%1.dat").arg(this->getId());
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);

        while (!in.atEnd()) {
            Favorito favorito;
            in >> favorito.idCancion >> favorito.fechaMarcado;
            favoritos.append(favorito);
        }

        file.close();
    }

    return favoritos;
}

bool Estandar::esCancionFavorita(int cancionId) const {
    QString filename = QString("favoritos_%1.dat").arg(this->getId());
    QFile file(filename);

    if (file.open(QIODevice::ReadOnly)) {
        QDataStream in(&file);

        while (!in.atEnd()) {
            Favorito favorito;
            in >> favorito.idCancion >> favorito.fechaMarcado;
            if (favorito.idCancion == cancionId) {
                file.close();
                return true;
            }
        }

        file.close();
    }

    return false;
}

void Estandar::escribirEnStream(QDataStream& stream) const {
    Usuario::escribirEnStream(stream);
    stream << email << ultimoIdPlaylist;
}

void Estandar::leerDesdeStream(QDataStream& stream) {
    Usuario::leerDesdeStream(stream);
    stream >> email >> ultimoIdPlaylist;
}
