#ifndef CHECKERSSTATE_H
#define CHECKERSSTATE_H

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <vector>

typedef unsigned char uint8;

enum checker {
	WHITE = 1,				// белая шашка
	BLACK = 2,				// черная шашка
	WHITEKING = 3,			// белая дамка
	BLACKKING = 4,			// черная дамка
	WHITECELL = 10,			// неходовая клетка, i%2 != j%2

	MOVEDFROM = 20,			// перешла с
	MOVEDTO = 21,			// перешла на
	MOVEDTHROUGH = 22,		// прошла через
	DELETED = 23,			// побита
	MARKDELETED = 24,		// отмечена как побитая
	TOKING = 25				// превращение в дамку
};

class point {
public:
	point() {
		x=0; y=0; type=0;
	}
	point(int x, int y, int type) {
		this->x = x;
		this->y = y;
		this->type = type;
	}
	uint8 x;
	uint8 y;
	uint8 type;
bool operator == (const point & p ) {
		if( x==p.x && y==p.y && type==p.type )
			return true;
		return false;
	}
};

class CheckersState
{
public:
    CheckersState();
	CheckersState(uint8 size);
	~CheckersState();
	CheckersState(const CheckersState &source);
	CheckersState(const CheckersState *source);

	CheckersState * genNextState(std::vector <point> & v);	// пораждает новое состояние из текущего.
															// принимает в качестве аргумента вектор ходов
	void setParent(CheckersState * parent);
	//void calcCounts();										// рассчитывает число фишек
	std::vector <uint8> & counts();
	CheckersState * parent();
	std::vector < CheckersState * > & childs();
	std::vector < point > & move();
	int & deletedAtMove();									// количество фишек противника, побитых в результате перехода в это состояние
	int & score();
	void print();
	uint8 size();
	uint8 & at(uint8 i, uint8 j);
	uint8 color(uint8 i, uint8 j);
	bool isWhite(uint8 i, uint8 j);
	bool isBlack(uint8 i, uint8 j);
	bool isKing(uint8 i, uint8 j);
	bool isNull(uint8 i, uint8 j);
private:
	CheckersState * p;
	std::vector < CheckersState * > xchilds;
	std::vector < point > xmove;								// описание одного хода от родителя к данному состоянию
	std::vector < uint8 > xcounts;								// количество фишек разных типов
	void allocate(uint8 size);
	uint8 ** data;
	uint8 n;
	uint8 tmp;
	int xscore;
	int deletedatmove;
};

#endif // CHECKERSSTATE_H
