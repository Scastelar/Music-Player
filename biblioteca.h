#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <QDateTime>

struct EntradaBiblioteca {
    int idCancion;
    QDateTime fechaAgregado;
};


class Biblioteca
{
private:
    int userId;
    QList<EntradaBiblioteca> canciones;

public:
    Biblioteca(int userId);
    void agregarCancion(int idCancion);
    bool eliminarCancion(int idCancion);
    QList<int> obtenerCanciones() const;
    void guardarEnArchivo();
    void cargarDesdeArchivo();
};

#endif // BIBLIOTECA_H
