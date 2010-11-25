#include "checkerspicture.h"

CheckersPicture::CheckersPicture(QWidget *parent) : QWidget(parent)
{
	QPalette pal = palette();
	pal.setColor(QPalette::Light,QColor(0x87,0xa1,0xc0, 0));
	setPalette(pal);
	setBackgroundRole(QPalette::Light);
	setAutoFillBackground(true);

	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	setAttribute(Qt::WA_StaticContents);

	zoom = 16;
	curstate = NULL;
	n = 8;
	setMinimumSize(zoom*(n+1),zoom*(n+1));

	// flags
//	mouseClickFlag = true;
//	isWhite = false;				// комп играет белым?
//	isWhite = true;
}

CheckersPicture::~CheckersPicture()
{

}

CheckersState * CheckersPicture::state() {
	return curstate;
}

void CheckersPicture::setComputerColor(uint8 color) {
	this->color = color;
}

void CheckersPicture::setState(CheckersState * state) {
	if(state) {
		curstate = state;
		n = state->size();
		v.clear();
		//mouseClickFlag = true;
		update();
	} else {
		clear();
	}
}

void CheckersPicture::setSize(int n) {
	this->n = n;
	update();
}

void CheckersPicture::deleteVector() {
	if(v.size()) {
		v.clear();
		update();
	}
}

void CheckersPicture::setVector(std::vector <point> & v) {
	this->v = v;
	update();
}

void CheckersPicture::clear() {
	curstate = NULL;
	v.clear();
	update();
}

void CheckersPicture::mousePressEvent(QMouseEvent *event)
{
	//qDebug() << side << " " << x << " " << event->pos().x() << " " << event->pos().y();
	//if (event->buttons() && Qt::LeftButton && mouseClickFlag) {
	if (event->buttons() & Qt::LeftButton) {
		//qreal i = (event->pos().x() - p.x() + side/(2*n+2)) * (n+1)/side - 1.0;
		//qreal j = (double)n - (event->pos().y() - p.y() + side/(2*n+2)) * (n+1)/side;
		qreal i = (event->pos().x() - p.x()) * (n)/side;
		qreal j = (double)n - (event->pos().y() - p.y()) * (n)/side - 1;
		//qDebug() << (int)i << " " << (int)j;
		if(color==BLACK)
			emit mouseClicked((int)i,(int)j);
		else
			emit mouseClicked(n -1 - (int)i, n - 1 - (int)j);
	}
}

void CheckersPicture::mouseMoveEvent(QMouseEvent *event)
{

}

void CheckersPicture::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
//#ifdef Q_WS_S60
//	QImage bg(":/bg_symbian.png");
//#else
//	QImage bg(":/bg_desktop.png");
//#endif
//	painter.drawImage(0, 0, bg);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setViewport(p.x(),p.y(),side,side);
	painter.setWindow(0, 0, zoom*(n), zoom*(n));

/*
	QColor border(0xce,0x5c,0x00);
	painter.fillRect(QRect(0,0,zoom*(n+1.0),zoom*0.5), border);
	painter.fillRect(QRect(0,zoom*(n+0.5),zoom*(n+1.0),zoom*0.5), border);
	painter.fillRect(QRect(0,0,zoom*(0.5),zoom*(n+1.0)), border);
	painter.fillRect(QRect(zoom*(n+0.5),0,zoom*0.5,zoom*(n+1.0)), border);
*/

	QColor dark(0xcc,0xcc,0xcc,200);

	for(int i=0; i<n; i++) {
		for(int j=0; j<n; j++) {
			QRect rect = pixelRect(i, j);
			if( !((i+j%2)%2) ) {
					painter.fillRect(rect, dark);
					//painter.drawEllipse(QPoint(zoom*(i+1),zoom*(10-j)),s,s);
			} else {
				painter.fillRect(rect, Qt::white);
			}
		}
	}

	int ix,jx;
//	QColor endColor(0x4c,0x4c,0xcc);
	QColor endColor(0x90,0x00,0x90,200);
	QColor startColor(0x33,0xff,0x00,200);
	QColor capturedColor(0xff,0x33,0x33,200);
	QColor normalColor(0x4c,0x4c,0xcc,200);
	QColor black(0x00, 0x00, 0x00, 200);
	QColor white(0xff, 0xff, 0xff, 200);

	if(v.size()) {
		int type;
		for(unsigned i=0; i< v.size(); i++) {
			color==WHITE ? jx = n-1-v.at(i).y : jx = v.at(i).y;
			color==WHITE ? ix = n-1-v.at(i).x : ix = v.at(i).x;
			QRect rect = pixelRect(ix, jx);
			type = v.at(i).type;
			if(type == MOVEDFROM)
				painter.fillRect(rect, startColor);
			else if(type == MOVEDTO || type == TOKING)
				painter.fillRect(rect, endColor);
			else if(type == MOVEDTHROUGH)
				painter.fillRect(rect, normalColor);
			else if(type == DELETED)
				painter.fillRect(rect, capturedColor);
		}
	}

	int s = zoom*0.4;
	int sd = zoom*0.2;
	if(curstate) {
		painter.setPen(QPen(black,zoom*0.1));
		painter.setBrush(QBrush(white));
		for(int i=0; i<n; i++) {
			for(int j=0; j<n; j++) {
				color==WHITE ? jx = j+1 : jx = n-j;
				color==WHITE? ix = n-i : ix = i+1;
				if(curstate->at(i,j)==WHITE)
					painter.drawEllipse(QPoint(zoom*(ix-0.5),zoom*(jx-0.5)),s,s);
				if(curstate->at(i,j)==WHITEKING) {
					painter.drawEllipse(QPoint(zoom*(ix-0.5),zoom*(jx-0.5)),s,s);
					painter.drawEllipse(QPoint(zoom*(ix-0.5),zoom*(jx-0.5)),sd,sd);
				}
			}
		}
		painter.setBrush(QBrush(black));
		for(int i=0; i<n; i++) {
			for(int j=0; j<n; j++) {
				color==WHITE ? jx = j+1 : jx = n-j;
				color==WHITE ? ix = n-i : ix = i+1;
				if(curstate->at(i,j)==BLACK)
					painter.drawEllipse(QPoint(zoom*(ix-0.5),zoom*(jx-0.5)),s,s);
				if(curstate->at(i,j)==BLACKKING) {
					painter.drawEllipse(QPoint(zoom*(ix-0.5),zoom*(jx-0.5)),s,s);
					painter.setPen(QPen(white,zoom*0.1));
					painter.drawEllipse(QPoint(zoom*(ix-0.5),zoom*(jx-0.5)),sd,sd);
					painter.setPen(QPen(black,zoom*0.1));
				}
			}
		}
	}
}

void CheckersPicture::resizeEvent (QResizeEvent * event) {
	if(event->oldSize()!=event->size()) {
		update();
		side = qMin(width(), height());
		p = QPoint((width() - side) / 2, (height() - side) / 2);
	} else {
		event->ignore();
	}
}

QRect CheckersPicture::pixelRect(int i, int j) const
{
	return QRect(zoom * (i-0.5) + zoom*0.5, zoom*(n-1.0) - zoom * j, zoom, zoom);
}


