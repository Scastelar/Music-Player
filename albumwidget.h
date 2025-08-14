#ifndef ALBUMWIDGET_H
#define ALBUMWIDGET_H

#include <QWidget>
#include <QLabel>

class Album;
class Cuentas;

class AlbumWidget : public QWidget {
    Q_OBJECT
public:
    explicit AlbumWidget(const Album& album, Cuentas* cuentas, QWidget* parent = nullptr);

signals:
    void clicked(const Album& album);

protected:
    void mousePressEvent(QMouseEvent* event) override;

private:
    void setupUI();
    const Album& m_album;
    Cuentas* m_cuentas;
    QLabel* coverLabel;
};

#endif // ALBUMWIDGET_H
