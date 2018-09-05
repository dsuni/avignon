#ifndef TABLELABEL_H
#define TABLELABEL_H
#include <QLabel>
#include <QPoint>
#include <QMouseEvent>

class TableLabel : public QLabel { 
	Q_OBJECT 

private:
	int pos_x;
	int pos_y;
	
public:
	TableLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
	void setPosition(int x, int y);
	void getPosition(int *x, int *y);

protected:
	void mousePressEvent(QMouseEvent *event);

signals:
	void clicked(const QPoint&);
};
#endif
