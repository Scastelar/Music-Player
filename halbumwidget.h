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

signals:
    void editAlbumRequested(int id);
    void deleteAlbumRequested(int id);
    void albumClicked(Album& album);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private slots:
    void onEditAlbum();
    void onDeleteAlbum();

private:
    Album& m_album;
    QLabel *coverLabel;
    QLabel *titleLabel;
    QLabel *artistLabel;
};

#endif // HAlbumWidget_H
