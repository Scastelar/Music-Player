# Performance Improvements for Music-Player

This document identifies slow code patterns and suggests optimizations for the Music-Player Qt application.

## Table of Contents
1. [Critical Performance Issues](#critical-performance-issues)
2. [Medium Priority Optimizations](#medium-priority-optimizations)
3. [Low Priority Improvements](#low-priority-improvements)
4. [Implementation Examples](#implementation-examples)

---

## Critical Performance Issues

### 1. Global Statistics Methods - O(n) File I/O per User

**Location:** `cuentas.cpp` lines 923-1055

**Problem:** Methods like `cancionesMasEscuchadasGlobal()`, `cancionesMejorCalificadasGlobal()`, `usuariosMasActivos()`, and `promedioCalificacionesPorGenero()` iterate over ALL users and open a file for each one. This becomes extremely slow as the user count grows.

**Current Code (Example):**
```cpp
QList<QPair<int, int>> Cuentas::cancionesMasEscuchadasGlobal(int limite) {
    QMap<int, int> contador;
    // Iterates over ALL users - O(n) file opens!
    for (Usuario* usuario : usuarios) {
        QString archivo = QString("%1%2/reproducciones_%2.dat").arg(CARPETA_USUARIOS).arg(usuario->getId());
        QFile file(archivo);
        if (file.open(QIODevice::ReadOnly)) {
            // ... reads entire file
        }
    }
    // ... sorting
}
```

**Suggested Fix:** 
- Maintain an in-memory cache for aggregated statistics
- Use a background thread to periodically update statistics
- Consider a single aggregated statistics file instead of per-user files

### 2. Blocking Metadata Extraction with QEventLoop

**Location:** `artistawindow.cpp` lines 824-865

**Problem:** The `extractMetadata()` function uses a blocking `QEventLoop::exec()` call for each file being processed. When adding multiple songs, this blocks the UI thread repeatedly.

**Current Code:**
```cpp
ArtistaWindow::CancionMetadata ArtistaWindow::extractMetadata(const QString &filePath) {
    QMediaPlayer player;
    // ...
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    
    QObject::connect(&player, &QMediaPlayer::metaDataChanged, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    
    timer.start(2000); // 2 second timeout PER FILE!
    loop.exec(); // BLOCKS UI!
    // ...
}
```

**Suggested Fix:**
- Use `QtConcurrent::run()` to process metadata in a background thread
- Implement async batch processing with progress callbacks
- Consider using a dedicated metadata library like TagLib instead of QMediaPlayer

### 3. Synchronous File Saves on Every Operation

**Location:** `cuentas.cpp` - multiple methods

**Problem:** Many methods call `guardarCancionesEnArchivo()` or `guardarUsuariosEnArchivo()` after every single operation, causing unnecessary disk I/O.

**Affected Methods:**
- `crearCancion()` (line 617)
- `eliminarCancion()` (line 625)
- `actualizarCancion()` (line 637)
- `crearUsuarioNormal()` (line 416)
- `crearArtista()` (line 452)
- `desactivarCuenta()` (line 466)
- `editarUsuario()` (line 514)

**Suggested Fix:**
- Implement a dirty flag mechanism
- Use deferred/batched saving with a timer
- Save only on application close or explicit save commands

---

## Medium Priority Optimizations

### 4. Inefficient Search Operations with String Concatenation

**Location:** `artistawindow.cpp` lines 671-729

**Problem:** The search function performs multiple searches and concatenates results, potentially creating duplicates.

**Current Code:**
```cpp
case 1:
    canciones = manejo->buscarCancionesPorArtista(texto);
    canciones += manejo->buscarCancionesPorArtista(textoMin);  // May duplicate results!
    canciones += manejo->buscarCancionesPorTitulo(texto);
    canciones += manejo->buscarCancionesPorTitulo(textoMin);   // May duplicate results!
```

**Suggested Fix:**
- Use case-insensitive search internally
- Return results as QSet to avoid duplicates
- Combine searches into a single method with multiple criteria

### 5. Widget Creation in Loops Without Object Pooling

**Location:** Multiple files

**Problem:** SongWidget and AlbumWidget are created/destroyed frequently when filtering or refreshing lists, causing memory churn and potential UI stuttering.

**Affected Areas:**
- `artistawindow.cpp::loadSongs()` (lines 278-328)
- `artistawindow.cpp::cargarAlbumesUsuario()` (lines 331-376)
- `listareproducciondetailwindow.cpp::loadSongs()` (lines 192-293)

**Suggested Fix:**
- Implement widget recycling/pooling
- Use QListView with a model instead of creating widgets directly
- Consider virtual scrolling for large lists

### 6. Repeated Style String Construction

**Location:** `mainwindow.cpp` lines 27-61, `artistawindow.cpp` lines 988-1033

**Problem:** Large stylesheet strings are constructed with `QString::replace()` at runtime.

**Current Code:**
```cpp
QString style = R"(
    QWidget {
        background-color: rgb(40,40,40);
        color: white;
        font-family: 'FONT';
    }
    // ... many lines
)";
style.replace("FONT", Montserrat);
this->setStyleSheet(style);
```

**Suggested Fix:**
- Load stylesheets from `.qss` resource files
- Cache the processed stylesheet as a static QString
- Use template strings or QString::arg() for better performance

### 7. Missing const References in Parameters

**Location:** Multiple files

**Problem:** Several methods pass QString and QList by value instead of const reference, causing unnecessary copies.

**Examples:**
- `cuentas.h` line 189: `buscarAlbumesPorArtista(const int& artista)` - `int` doesn't need reference
- Various methods pass QString by value

**Suggested Fix:**
- Pass large objects (QString, QList, etc.) by `const&`
- Pass small primitives (int, bool) by value

---

## Low Priority Improvements

### 8. QPixmap Scaling on Every Widget Creation

**Location:** `songwidget.cpp` lines 29-36, `albumwidget.cpp` lines 30-37

**Problem:** Images are loaded and scaled for every widget, even if the same image was already processed.

**Suggested Fix:**
- Implement an image cache using QCache
- Store pre-scaled thumbnails
- Use lazy loading for images outside the viewport

### 9. Font Loading on Every Window Creation

**Location:** `mainwindow.cpp` lines 20-24, `artistawindow.cpp` lines 34-41

**Problem:** Fonts are loaded from resources on each window construction.

**Suggested Fix:**
- Load fonts once in main() before creating any windows
- Store font family names in a global/singleton configuration

### 10. Linear Search in playSong for Current Index

**Location:** `artistawindow.cpp` lines 118-124

**Problem:** Linear search O(n) to find the current song index.

**Current Code:**
```cpp
for (int i = 0; i < m_cancionesAlbumActual.size(); ++i) {
    if (m_cancionesAlbumActual.at(i)->getId() == cancion.getId()) {
        m_indiceAlbumActual = i;
        break;
    }
}
```

**Suggested Fix:**
- Maintain a QHash<int, int> mapping song ID to index
- Or pass the index directly when calling playSong()

### 11. Unnecessary Container Copies

**Location:** `listareproduccion.h` line 26

**Problem:** `getCanciones()` returns a copy of the QList.

**Current Code:**
```cpp
QList<int> getCanciones() const { return canciones; }
```

**Suggested Fix:**
```cpp
const QList<int>& getCanciones() const { return canciones; }
```

### 12. Repeated Dynamic Casts

**Location:** `cuentas.cpp` lines 497-509

**Problem:** Dynamic casting is used but could be avoided with virtual methods.

**Suggested Fix:**
- Add virtual setters in the base Usuario class
- Or cache the dynamic_cast result

---

## Implementation Examples

### Example 1: Deferred Save Mechanism

```cpp
// In cuentas.h
class Cuentas {
private:
    bool m_cancionesDirty = false;
    bool m_usuariosDirty = false;
    QTimer* m_saveTimer;
    
    void scheduleSave();
    
public:
    void saveNowIfNeeded();
};

// In cuentas.cpp
void Cuentas::scheduleSave() {
    if (!m_saveTimer->isActive()) {
        m_saveTimer->start(5000); // Save after 5 seconds of inactivity
    }
}

void Cuentas::saveNowIfNeeded() {
    if (m_cancionesDirty) {
        guardarCancionesEnArchivo();
        m_cancionesDirty = false;
    }
    if (m_usuariosDirty) {
        guardarUsuariosEnArchivo();
        m_usuariosDirty = false;
    }
}
```

### Example 2: Image Cache

```cpp
// In a new imagecache.h
class ImageCache {
private:
    static QCache<QString, QPixmap> s_cache;
    
public:
    static QPixmap getScaledPixmap(const QString& path, const QSize& size) {
        QString key = QString("%1_%2x%3").arg(path).arg(size.width()).arg(size.height());
        
        if (QPixmap* cached = s_cache.object(key)) {
            return *cached;
        }
        
        QPixmap original(path);
        if (original.isNull()) {
            original = QPixmap(":/images/default_cover.png");
        }
        
        QPixmap* scaled = new QPixmap(original.scaled(size, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
        s_cache.insert(key, scaled);
        return *scaled;
    }
};
```

### Example 3: Async Metadata Extraction

```cpp
// Using QtConcurrent
#include <QtConcurrent>

void ArtistaWindow::extractMetadataAsync(const QStringList& filePaths, 
                                          std::function<void(QList<CancionMetadata>)> callback) {
    QtConcurrent::run([=]() {
        QList<CancionMetadata> results;
        for (const QString& path : filePaths) {
            results.append(extractMetadataSync(path));
        }
        QMetaObject::invokeMethod(this, [=]() {
            callback(results);
        }, Qt::QueuedConnection);
    });
}
```

---

## Priority Summary

| Issue | Impact | Effort | Priority |
|-------|--------|--------|----------|
| Global Statistics O(n) I/O | High | High | Critical |
| Blocking Metadata Extraction | High | Medium | Critical |
| Synchronous Saves | Medium | Low | Critical |
| Duplicate Search Results | Medium | Low | Medium |
| Widget Creation Churn | Medium | Medium | Medium |
| Repeated Style Construction | Low | Low | Medium |
| Missing const References | Low | Low | Low |
| Pixmap Scaling | Low | Medium | Low |
| Font Loading | Very Low | Low | Low |
| Linear Search for Index | Very Low | Low | Low |

---

## Recommended First Steps

1. **Implement deferred saving** - Easy win, significant I/O reduction
2. **Add case-insensitive search** - Quick fix for duplicate search results
3. **Return collections by const reference** - Simple code change, prevents copies
4. **Background metadata processing** - Improves UX significantly when adding songs
