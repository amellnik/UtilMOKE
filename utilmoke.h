#ifndef UTILMOKE_H
#define UTILMOKE_H

#include <QWidget>

namespace Ui {
class UtilMOKE;
}

class UtilMOKE : public QWidget
{
    Q_OBJECT

public:
    explicit UtilMOKE(QWidget *parent = 0);
    ~UtilMOKE();

private:
    Ui::UtilMOKE *ui;
};

#endif // UTILMOKE_H
