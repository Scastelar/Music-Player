#ifndef HAlbumWidget_H
#define HAlbumWidget_H

#include <QWidget>
#include <QLabel>
#include "album.h"

class HAlbumWidget : public QWidget
{
    Q_OBJECT
public:
    explicit HAlbumWidget(Album& album, QWidget *parent = nullptr);


private:
    Album& m_album;
    QLabel *coverLabel;
    QLabel *titleLabel;
    QLabel *artistLabel;
};

#endif // HAlbumWidget_H
