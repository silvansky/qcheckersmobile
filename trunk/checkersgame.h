#ifndef CHECKERSGAME_H
#define CHECKERSGAME_H

#include <QObject>
#include <QVector>
#include <vector>
#include "checkersstate.h"

// типы правил игры
enum GameType{
	RUSSIAN = 1,
	INTERNATIONAL = 2,
};

class CheckersGame : public QObject
{
	Q_OBJECT
public:
	CheckersGame();						// конструктор
	~CheckersGame();
	bool setGameType(uint8 type);		// устанавливает соответствующие типу игры переменные
	void setMaxLevel(int level);
	void startNewGame(uint8 color);		// начинает новую игру, аргумент - цвет шашек компьютера
	void endGame();
public slots:
	void setClicked(int i, int j);
signals:
	void stateChanged(CheckersState *);	// изменилось состояние, нада перерисовывать
	void vectorChanged(std::vector <point> & v);
	void vectorDeleted();
	void gameEnded(uint8);
private:
	void firstClick(int i, int j);		// игрок кликнул мышкой первый раз
	void secondClick(int i, int j);		// игрок кликнул мышкой второй раз

	// функции интеллектуального алгоритма
	void go();										// осуществление шага компьютера из текущего состояния
	int goRecursive(CheckersState * state, int level, int alpha, int beta);	// рекурсивное построение дерева поиска
	void calcCounts(CheckersState * state);
	int evaluation(CheckersState * state);			// оценочная функция для произвольного состояния
	void pp(CheckersState * state, uint8 color);	// порождающая процедура
	bool checkTerminatePosition(CheckersState * state);	// проверка позиции на терминальность
	int movesCount(CheckersState * state, int i, int j);
	int searchMove(CheckersState * state, int i, int j, std::vector <point> & vp); // рекурсивно ищет возможные ходы для заданной фишки и состояния
	bool checkCoordinate(char x);					// проверяет координату на принадлежность игровому полю
	void clearTree(CheckersState * state, bool clearlists = true, bool onlychilds = false);
	void clearTreeRecursive(CheckersState * state, bool clearlists = false);
	bool move(point p1, point p2);						// функиця проверяет возможность перехода p1>>p2 и осуществляет его, если возможно
	uint8 whoWin(CheckersState * state);
	//

	// разные временные перемменные нужные для работы алгоритма
	std::vector <CheckersState *> movesearch;
	std::vector < point > tmpvector;	// временный вектор для хранения разных точек
	point tmppoint;
	void printPointVector(std::vector <point> & v);

	CheckersState * first;			// первое состояние игры
	CheckersState * current;		// текущее состояние игры
	CheckersState * tmp;
	uint8 n;						// размер доски
	uint8 k_start;						// число рядов шашек на старте
	uint8 type;						// тип игры (русские, интернациональные шашки)
	uint8 computercolor;			// цвет компьютера-игрока
	uint8 humancolor;				// цвет человека-игрока

	int level;						// уровень глубины построения дерева
	uint8 click;					// номер клика
	char ix[4];						// направления обхода
	char jx[4];						// используются на этапе поиска возм. ходов

	// различные флаги, управляющие ограничениями поиска и т.д.:
	bool capturefound;
	bool calccounts;
	bool gamerunning;				// игра запущена щасс!
	int maxlevel;				// максимальная глубина поиска в уровнях. корень - уровень 0

	// счётчики состояний (( где-то течёт память!
	int cleared;
	int created;
};

#endif // CHECKERSGAME_H
