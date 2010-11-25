#include "checkersgame.h"
#include <time.h>

CheckersGame::CheckersGame()
{
	setGameType(RUSSIAN);
	srand(time(NULL));
	cleared = 0;
	created = 0;
	current = NULL;
	first = NULL;
	current = NULL;
//	maxlevel = 3;
}

CheckersGame::~CheckersGame() {
	endGame();
}

bool CheckersGame::setGameType(uint8 type) {
	if(type == RUSSIAN) {
		this->type = type;
		n = 8; k_start = 3;
		return true;
	}
	if(type == INTERNATIONAL) {
		this->type = type;
		n = 10; k_start = 4;
		return true;
	}
	return false;
}

void CheckersGame::setMaxLevel(int level) {
	if(level >=3 && level <=7)
		maxlevel = level;
	else
		maxlevel = 3;
}

void CheckersGame::startNewGame(uint8 color) {
	computercolor = color;
	if(color == WHITE)
		humancolor = BLACK;
	else
		humancolor = WHITE;
	first = new CheckersState(n);
	created++;

	for(int i=0; i<n; i++) {
		for(int j=0; j<k_start; j++) {
			if(i%2 == j%2)
				first->at(i,j) = WHITE;
		}
		for(int j=n-k_start; j<n; j++) {
			if(i%2 == j%2)
				first->at(i,j) = BLACK;
		}
	}
	if(current)
		delete current;
	current = new CheckersState(first);
	created++;
	click = 0;
	// установка направлений обхода
	ix[0] = -1; ix[1] = 1; ix[2] = -1; ix[3] = 1;
	jx[0] = 1; jx[1] = 1; jx[2] = -1; jx[3] = -1;
	gamerunning = true;
	if(computercolor == WHITE) {
		go();
	} else {
		emit stateChanged(current);
		pp(current,humancolor);
		//std::cout << "Created: " << created << " Deleted: " << cleared << "\n"; std::cout.flush();
		//evaluation(current);
	}
}

void CheckersGame::endGame() {
	cleared = 0;
	created = 0;
	gamerunning = false;
	if(current) {
		clearTree(current, true, true);
		current = NULL;
	}
//	if(first) {
//		delete first;
		first = NULL;
//	}
}

// порождающая процедура для заданного состояния и цвета фишек игрока
void CheckersGame::pp(CheckersState * state, uint8 color) {
	//QVector < std::vector <point> > vpp;
	std::vector <point> vp;
	capturefound = false;
	for(unsigned i=0; i<n; i++) {
		for(unsigned j=0; j<n; j++) {
			if(i%2 == j%2 && state->color(i,j) == color) {
				searchMove(state, i, j, vp);
				clearTree(state, false, true);  // очищаем дерево, оставляя листы и родителя
			}
		}
	}
	// если найден хоть один ход с боем, мы должны исключить простые ходы
	if(capturefound) {
		for(unsigned i=0; i<movesearch.size(); i++) {
			if( movesearch.at(i)->move().size() == 2 ) {
				delete movesearch.at(i);
				movesearch.erase( movesearch.begin()+i );
				i--;
			} else {
				break;
			}
		}
	}
	// в международных шашках мы обязаны выбрать лучший ход боя
	if(type == INTERNATIONAL) {
		int max_deleted = 0;
		for(unsigned i=0; i<movesearch.size(); i++) {
			if( movesearch.at(i)->deletedAtMove() > max_deleted )
				max_deleted = movesearch.at(i)->deletedAtMove();
		}
		for(unsigned i=0; i<movesearch.size(); i++) {
			if( movesearch.at(i)->deletedAtMove() < max_deleted ) {
				delete movesearch.at(i);
				movesearch.erase( movesearch.begin()+i );
				i--;
			}
		}
	}
	// делаем найденные листы детьми рассматриваемого состояния
	for(unsigned i=0; i<movesearch.size(); i++) {
		std::vector<point>::iterator it;
		// MARKDELETED >> DELETED
		for ( it=movesearch.at(i)->move().begin() ; it < movesearch.at(i)->move().end(); it++ ) {
			if( it->type == DELETED )
				movesearch.at(i)->at( it->x, it->y ) = 0;
		}
		movesearch.at(i)->setParent(state);
//		if(lastlevel)
//			movesearch.at(i)->score() = evaluation( movesearch.at(i) );
		state->childs().push_back( movesearch.at(i) );
	}
	movesearch.clear();
}

int CheckersGame::movesCount(CheckersState * state, int i, int j) {
	uint8 color = state->color(i,j);
	int moves = 0;
	if(!color)
		return 0;
	int sidx, eidx, pidx = 1; // номера сдвигов, возм. максимальное число шагов в длину для шашки
	if(color == WHITE) {
		sidx = 0; eidx = 1;
	}
	if(color == BLACK) {
		sidx = 2; eidx = 3;
	}
	if( state->isKing(i,j) ) {
		sidx = 0; eidx = 3; pidx = n; // дамка может ходить по всему игровому полю и в любую сторону
	}
	// проверка возможности перехода
	for(char k=sidx; k<=eidx; k++) {
		for(char pk=1; pk <= pidx; pk++) {
			char xi = i + pk*ix[(int)k];
			char xj = j + pk*jx[(int)k];
			if( !checkCoordinate(xi) || !checkCoordinate(xj) ) // усли вышли за пределы игрового
				break;
			if(state->isNull(xi,xj)) {
				moves ++;
			} else {
				break;
			}
		}
	}
	bool captureflag;
	for(int k=0; k<=3; k++) {
		captureflag = false;
		for(char pk=1; pk <= pidx+1; pk++) {
			char xi = i + pk*ix[k];
			char xj = j + pk*jx[k];
			if( !checkCoordinate(xi) || !checkCoordinate(xj) )
				break;
			if( !captureflag && state->isNull(xi,xj) )
				continue;
			if( state->color(xi,xj) == color || state->at(xi,xj) == MARKDELETED )
				break;
			if( !captureflag && state->color(xi, xj)!= color ) {
				captureflag = true;
				continue;
			}
			if(captureflag) {
				if( !state->isNull(xi,xj) ) {
					captureflag = false;
					break;
				}
				moves++;
			}
		}
	}
	return moves;
}

// поиск возможного хода для заданной фишки
int CheckersGame::searchMove(CheckersState *state, int i, int j, std::vector <point> & vp) {
	std::vector < std::vector <point> > vpp; // массив для простых ходов
	std::vector <point> tmp_vp;
	point cp;
	int normmoves = 0, delmoves = 0;

	uint8 color = state->color(i,j);
	bool isking = state->isKing(i,j);
	if(!color)
		return false;

	int sidx, eidx, pidx = 1; // номера сдвигов, возм. максимальное число шагов в длину для шашки
	if(color == WHITE) {
		sidx = 0; eidx = 1;
	}
	if(color == BLACK) {
		sidx = 2; eidx = 3;
	}
	if( state->isKing(i,j) ) {
		sidx = 0; eidx = 3; pidx = n; // дамка может ходить по всему игровому полю и в любую сторону
	}
	// проверка возможности перехода
	for(char k=sidx; k<=eidx && !capturefound; k++) {
		for(char pk=1; pk <= pidx; pk++) {
			char xi = i + pk*ix[(int)k];
			char xj = j + pk*jx[(int)k];
			if( !checkCoordinate(xi) || !checkCoordinate(xj) ) // усли вышли за пределы игрового
				break;
			if(state->isNull(xi,xj)) {
				// создаём новое состояние
				tmp_vp.clear();
				tmp_vp.push_back(point(i,j,MOVEDFROM));
				if ( (color==BLACK && xj==0 && !isking) || (color==WHITE && xj==n-1 && !isking)  ) {
					tmp_vp.push_back(point(xi,xj,TOKING));
				} else {
					tmp_vp.push_back(point(xi,xj,MOVEDTO));
				}
				vpp.push_back(tmp_vp);
				normmoves ++;
//				std::cout << "Can move to " << (int)xi << " " << (int)xj << " \n"; std::cout.flush();
			} else {
				break;
			}
		}
	}
	bool captureflag;
	// проверка возможности боя
	for(int k=0; k<=3; k++) {
		captureflag = false;
		for(char pk=1; pk <= pidx+1; pk++) {
			char xi = i + pk*ix[k];
			char xj = j + pk*jx[k];
			if( !checkCoordinate(xi) || !checkCoordinate(xj) )	// проверка правильности координат
				break;
			if( !captureflag && state->isNull(xi,xj) )				// если не найдена сбитая фишка и пустая клетка
				continue;
			// встретился на пути такой же цвет - не можем обойти и перепрыгнуть :(
			if( state->color(xi,xj) == color || state->at(xi,xj) == MARKDELETED )
				break;
			// если не найдена сбитая фишка и в текущей позиции другая по цвету фишка
			if( !captureflag && state->color(xi, xj)!= color ) {
				captureflag = true;
				cp.x = xi; cp.y = xj; cp.type = MARKDELETED;
				continue;
			}
			// если найдена фишка, которую предположительно можно побить
			if(captureflag) {
					// если непустая клетка то побить не можем
				if( !state->isNull(xi,xj) ) {
					captureflag = false;
					break;
				}
				// построение нового состояния, запуск поиска возможного следующего боя из этого состояния
//				std::cout << "Can capture at " << (int)cp.x << " " << (int)cp.y << " and move to " << (int)xi << " " << (int)xj << "\n"; std::cout.flush();
				capturefound = true;
				delmoves++;

				// подготовка порождающего вектора
				tmp_vp.clear();
				tmp_vp.push_back(point(i,j,MOVEDFROM));
				tmp_vp.push_back(cp);
				if ( (color==BLACK && xj==0 && !isking) || (color==WHITE && xj==n-1 && !isking)  ) // проверка на дамочность
					tmp_vp.push_back(point(xi,xj,TOKING));
				else
					tmp_vp.push_back(point(xi,xj,MOVEDTO));

				CheckersState *tmpstate = state->genNextState(tmp_vp);		// порождение нового состояния
				created++;

				// подготовка вектора истории
				std::vector <point> history_vector = vp;
				if( vp.size() ) {
					(history_vector.end()-1)->type = MOVEDTHROUGH;
					(history_vector.end()-2)->type = DELETED;
					tmpstate->deletedAtMove() = state->deletedAtMove()+1;
				} else {
					history_vector.push_back( tmp_vp.at(0) );
					tmpstate->deletedAtMove() = 1;
				}
				cp.type = DELETED;
				history_vector.push_back( cp );
				history_vector.push_back( tmp_vp.at(2) );
				//
				tmpstate->move() = history_vector;
				tmpstate->setParent(state);
				state->childs().push_back(tmpstate);
				if(! searchMove(tmpstate, xi, xj, history_vector ))
					movesearch.push_back(tmpstate);
			}
		}
	}
	// +- добавить проверку на превращение в дамку!
	// + учесть правило турецкого удара!!!

	// имеются состояния, из которых бьются фишки, убираем простые переходы как не соответствующие правилам
	if(capturefound) {
		normmoves = 0;
	} else {
		for(unsigned i=0; i<vpp.size(); i++) {
			CheckersState * tmpstate = state->genNextState(vpp.at(i));
			created++;
			tmpstate->setParent(state);
			state->childs().push_back(tmpstate);
			movesearch.push_back(tmpstate);
		}
	}
	return normmoves + delmoves;
}

// процедура шага компьютера из текущего состояния
void CheckersGame::go() {
//	maxlevel = 3;
	//CheckersState * root;
//	alpha = -9999; beta = 9999;
	goRecursive(current, 1, -9999, 9999);
//	std::cout << alpha << " " << beta << "\n"; std::cout.flush();
	/// выбор из потомков самого лучшего
	int xmax = -9999;
	int id = 0;
	for(unsigned i=0; i<current->childs().size(); i++) {
		if( current->childs().at(i)->score() > xmax ) {
			xmax = current->childs().at(i)->score();
			//id = i;
		}
	}
	std::vector <CheckersState *> tmp;
	for(unsigned i=0; i<current->childs().size(); i++) {
		if( current->childs().at(i)->score() == xmax ) {
			tmp.push_back( current->childs().at(i) );
		}
	}
	id = rand() % tmp.size();
	//std::cout << tmp.size() << "\n";
//	id = 0;
	//CheckersState tmp = current->childs().at(i);
	move( tmp.at(id)->move().front(), tmp.at(id)->move().back() );
	pp(current,humancolor);
}

// рекурсивное построение дерева поиска
int CheckersGame::goRecursive(CheckersState *state, int level, int alpha, int beta) {
	bool max = true;
	if(computercolor == WHITE)
		max = false;
	uint8 color = humancolor;
	if(level%2) {
		color = computercolor;
		max = !max;
	}
	if(level == maxlevel || checkTerminatePosition(state)) {
		state->score() = evaluation(state);
		return state->score();
	}
	pp(state, color);
	unsigned i;
	for(i=0; i< state->childs().size(); i++) {
		alpha = std::max( alpha, - goRecursive( state->childs().at(i), level+1 , -beta, -alpha ) );
		if ( beta < alpha ) {
//			std::cout << "Removing tree :-)\n"; std::cout.flush();
			break;
		}
	}
	int xmax=-9999, xmin=9999;
	for(unsigned j=0; j<state->childs().size(); j++) {
		if(max) {
			if( state->childs().at(j)->score() > xmax )
				xmax = state->childs().at(j)->score();
		} else {
			if( state->childs().at(j)->score() < xmin )
				xmin = state->childs().at(j)->score();
		}
	}
	if(max)
		state->score() = xmax;
	else
		state->score() = xmin;
	return alpha;
}

bool CheckersGame::checkCoordinate(char x) {
	if(x>=0 && x<n)
		return true;
	return false;
}

/*
	  0            1                 2              3            4                5               6                 7
белых_шашек | белых_дамок | былых перемещений | белых боёв | чёрных шашек | чёрных дамок | чёрных перемещений | чёрных боёв
*/
void CheckersGame::calcCounts(CheckersState * state) {
	std::vector <CheckersState *> tmp;
//	 если уже что-то посчитано, то пораждающая процедура была выполнена ранее для
//	 одного из цветов
//	int kkk = -1;
//	if(state->childs().size()) {
//		tmp = state->childs();
//		kkk = tmp.at(0)->color(tmp.at(0)->move().at(0).x , tmp.at(0)->move().at(0).y);
//	}
	state->counts().clear();
	state->counts().resize(8,0);
//	uint8 x[2] = {WHITE, BLACK};
//	for(unsigned k=0; k<2; k++) {
//		if( x[k] != kkk ) {
//			pp(state, x[k]);
//			state->counts()[2+k*4] = state->childs().size();
//			for(unsigned i=0; i < state->childs().size(); i++)
//				state->counts()[3+k*4] += state->childs().at(i)->deletedAtMove();
//			clearTree(state,true,true);
//		} else {
//			state->counts()[2+k*4] = tmp.size();
//			for(unsigned i=0; i < tmp.size(); i++)
//				state->counts()[3+k*4] += tmp.at(i)->deletedAtMove();
//		}
//	}
	int movescount;
	for(unsigned i=0; i<n; i++) {
		for(unsigned j=0; j<n; j++) {
			if(i%2!=j%2)
				continue;
			movescount = movesCount(state, i, j);
			if(state->at(i,j)==WHITE) {
				state->counts()[0]++;
				state->counts()[2] += movescount;
			}
			if(state->at(i,j)==WHITEKING) {
				state->counts()[1]++;
				state->counts()[2] += movescount;
			}
			if(state->at(i,j)==BLACK) {
				state->counts()[4]++;
				state->counts()[6] += movescount;
			}
			if(state->at(i,j)==BLACKKING) {
				state->counts()[5]++;
				state->counts()[6] += movescount;
			}
		}
	}
	if(tmp.size()) {
		state->childs() = tmp;
	}
}

//  оценочная функция
int CheckersGame::evaluation(CheckersState * state) {
	if(!state->counts().size())
		calcCounts(state);
	int evaluation = 0;
	evaluation += 4 * ( state->counts()[0] - state->counts()[4] );
	evaluation += 6 * ( state->counts()[1] - state->counts()[5] );
	evaluation += ( state->counts()[2] - state->counts()[6] );
//	evaluation += 2 * ( state->counts()[3] - state->counts()[7] );
//	for(unsigned i=0; i< state->counts().size(); i++)
//		std::cout << (int)state->counts().at(i) << " ";
//	std::cout << "\n"; std::cout.flush();
	return evaluation;
}

bool CheckersGame::checkTerminatePosition(CheckersState * state) {
	if(!state->counts().size())
		calcCounts(state);
	// если все фишки побиты
	if ( !(state->counts()[0]+state->counts()[1]) ||
		 !(state->counts()[4]+state->counts()[5]) )
		return true;
	// если все фишки блокированы
	if( !state->counts()[2] || !state->counts()[6] )
		return true;
	return false;
}

uint8 CheckersGame::whoWin(CheckersState *state) {
	if ( !(state->counts()[0]+state->counts()[1]) || !state->counts()[2] )
		return BLACK;	// победил чёрный
	if ( !(state->counts()[4]+state->counts()[5]) || !state->counts()[6] )
		return WHITE;	// победил белый
	return -1;
}

// рекурсивная очистка дерева состояний
void CheckersGame::clearTree(CheckersState * state, bool clearlists, bool onlychilds) {
	//
//	if( !state )
//		return;
	if (onlychilds) {		// если очищаем только детей, не удаляя корневой узел
		for(unsigned i =0; i < state->childs().size(); i++) {
			clearTreeRecursive( state->childs().at(i), clearlists ); // удаляем всех детей
		}
		state->childs().clear();
	} else {
		clearTreeRecursive( state, clearlists ); // удаляем всё!
	}
}

void CheckersGame::clearTreeRecursive(CheckersState * state, bool clearlists) {
	// если у рассматриваемого узла нет потомков , значит он - лист
	if( state->childs().size() == 0 && clearlists ) {
		cleared ++;
		delete state;
	} else {
		for(unsigned i =0; i < state->childs().size(); i++) {
			clearTreeRecursive( state->childs().at(i), clearlists );
		}
		state->childs().clear();
		if(clearlists) {
			cleared ++;
			delete state;
		}
	}
}

void CheckersGame::printPointVector(std::vector <point> & v) {
	for(unsigned i=0; i<v.size(); i++) {
		//std::cout << (int)v.at(i).x << " " << (int)v.at(i).y << " " << (int)v.at(i).type << "\n";
	}
	//std::cout.flush();
}

void CheckersGame::setClicked(int i, int j) {
	if(i>=0 && i<n && j>=0 && j<n && i%2==j%2 && gamerunning) {
		//std::cout << "Clicked at " << (int)i << " " << (int)j << " = "<< (int)current->at(i,j) << std::endl;
		//click++;
		if(click == 0)
			firstClick(i, j);
		else
			secondClick(i, j);
	} else {
		emit vectorDeleted();
		click = 0;
	}
}

void CheckersGame::firstClick(int i, int j) {
	if( (humancolor == current->color(i,j))) {
		//std::cout << "Click 0" << "\n"; std::cout.flush();
		tmppoint.x = i; tmppoint.y = j; tmppoint.type = MOVEDFROM;
		tmpvector.clear();
		//click = 1;
		for(unsigned ii=0; ii< current->childs().size(); ii++ ) {
			if( current->childs().at(ii)->move().at(0) == tmppoint ) {
				for( unsigned jj=0; jj<current->childs().at(ii)->move().size(); jj++ ) {
					tmpvector.push_back( current->childs().at(ii)->move().at(jj) );
				}
			}
		}
		if(tmpvector.size()) {
			click = 1;
			emit vectorChanged(tmpvector);
			return;
		}
	}
	emit vectorDeleted();
}

void CheckersGame::secondClick(int i, int j) {
	//std::cout << "Click 1" << "\n"; std::cout.flush();
	bool move = false;
//	if ( (tmppoint.x == i && tmppoint.y == j) ) {
//		return;
//	}
	if( current->isNull(i,j) || (tmppoint.x == i && tmppoint.y == j))
		move = this->move( tmppoint , point(i,j,MOVEDTO) );
	if( !move ) {
		click =0;
		firstClick(i, j);
	} else {
		// ход успешен
		//pp(current, humancolor);
		if(gamerunning)
			go(); // выполним ход компом
		click = 0;
	}
}

bool CheckersGame::move(point p1, point p2) {
//	bool flag;
	for(unsigned i=0; i< current->childs().size(); i++ ) {
		if( current->childs().at(i)->move().front() == p1 &&
			current->childs().at(i)->move().back().x == p2.x &&
			current->childs().at(i)->move().back().y == p2.y )
		{
//			flag = true;
			// выполнить операцию по переходу в новое состояние
			//std::cout << "Move ok!!!\n"; std::cout.flush();
			CheckersState * tmpstate = current->childs().at(i);
			current->childs().erase(current->childs().begin()+i);
			clearTree(current,true);
			clearTree(tmpstate,true,true);
			current = tmpstate;
			tmp = new CheckersState(current); 	tmp->childs().clear();
			emit stateChanged(current);

//			std::cout << "Created: " << created << " Deleted: " << cleared << " FUCK:" << created-cleared <<"\n";	std::cout.flush();
			if(checkTerminatePosition(current)) {
				emit gameEnded( whoWin(current) );
				gamerunning = false;
			}
			return true;
		}
	}
	return false;
//	return flag;
}


