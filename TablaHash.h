#ifndef TABLAHASH_H
#define TABLAHASH_H

#include <QList>
#include <QPair>

template<typename K, typename V>
class TablaHash {
private:
    QList<QList<QPair<K, V>>> tabla;
    int capacidad;

    int funcionHash(const K& clave) const {
        int hash = 0;
        for (QChar c : clave) {
            hash = (hash * 31 + c.unicode()) % capacidad;
        }
        return hash;
    }

public:
    // Constructor
    TablaHash(int capacidad = 100) : capacidad(capacidad) {
        tabla.resize(capacidad);
    }

    // Métodos básicos
    void insertar(const K& clave, const V& valor) {
        int indice = funcionHash(clave);
        tabla[indice].append(qMakePair(clave, valor));
    }

    V buscar(const K& clave) const {  // Devuelve V directamente (que es Usuario*)
        int indice = funcionHash(clave);
        for (const auto& par : tabla[indice]) {
            if (par.first == clave) {
                return par.second;  // par.second es Usuario*, que es exactamente V
            }
        }
        return nullptr;
    }

    bool contiene(const K& clave) const {
        int indice = funcionHash(clave);
        for (const auto& par : tabla[indice]) {
            if (par.first == clave) {
                return true;
            }
        }
        return false;
    }

    // Métodos para iteración
    QList<QPair<K, V>> todosLosElementos() const {
        QList<QPair<K, V>> resultado;
        for (const auto& cubeta : tabla) {
            for (const auto& par : cubeta) {
                resultado.append(par);
            }
        }
        return resultado;
    }

    // Método para limpieza
    void limpiar() {
        for (auto& cubeta : tabla) {
            cubeta.clear();
        }
    }
};

#endif // TABLAHASH_H
