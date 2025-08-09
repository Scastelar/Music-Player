// songwidget.h
#ifndef SONGWIDGET_H
#define SONGWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include "cancion.h"

class SongWidget : public QWidget {
    Q_OBJECT
public:
    explicit SongWidget(const Cancion& cancion, QWidget *parent = nullptr);

signals:
    void songClicked(const Cancion& cancion);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    Cancion m_cancion;
    QLabel *coverLabel;
    QLabel *titleLabel;
    QLabel *artistLabel;
};

#endif // SONGWIDGET_H
