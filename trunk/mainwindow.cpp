#include "mainwindow.h"
#include <QStackedLayout>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	setupUi(this);

	game = new CheckersGame;
	dialog = new SettingsDialog;

	// Signal-Slots
	connect(actionOpen, SIGNAL(triggered()), this, SLOT(open()));
	connect(actionSave, SIGNAL(triggered()), this, SLOT(save()));
	connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));

	connect(actionStartNewGame, SIGNAL(triggered()), this, SLOT(startNewGame()));
	connect(actionEndGame, SIGNAL(triggered()), this, SLOT(endGame()));
	connect(actionSettings, SIGNAL(triggered()), this, SLOT(settings()));

	connect(actionAboutCheckers, SIGNAL(triggered()), this, SLOT(about()));
	connect(actionAboutQt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

	connect(picture, SIGNAL(mouseClicked(int, int)), game, SLOT(setClicked(int, int)));
	connect(game, SIGNAL(stateChanged(CheckersState *)), picture, SLOT(setState(CheckersState *)));
	connect(game, SIGNAL(vectorChanged(std::vector <point> &)), picture, SLOT(setVector(std::vector <point> &)));
	connect(game, SIGNAL(vectorDeleted()), picture, SLOT(deleteVector()));
//
	connect(game, SIGNAL(gameEnded(uint8)), this, SLOT(gameEnded(uint8)));
	QStackedLayout * sl = qobject_cast<QStackedLayout *>(stackedWidget->layout());
	if (sl)
		sl->setStackingMode(QStackedLayout::StackAll);

	tbNew->setDefaultAction(actionStartNewGame);
	tbSettings->setDefaultAction(actionSettings);
	tbExit->setDefaultAction(actionExit);

	QWidget * spacer = new QWidget(toolBar);
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
	toolBar->insertWidget(actionEndGame, spacer);

	setWindowTitle(tr("QCheckers"));
#ifndef Q_WS_S60
	resize(800,600);
#endif

	actionOpen->setEnabled(false);
	actionSave->setEnabled(false);
	actionEndGame->setEnabled(false);
	greeting = statusLabel->text();
}

MainWindow::~MainWindow()
{
	delete game;
	delete dialog;
}

void MainWindow::open() {
	// TODO: open saved game
}

void MainWindow::save() {
	// TODO: save current game
}

void MainWindow::startNewGame() {
	QSettings settings("Arceny","QCheckers");
	actionStartNewGame->setEnabled(false);
	int type = settings.value("type",RUSSIAN).toInt();
	int color = myColor = settings.value("color",WHITE).toInt();
	int level = settings.value("depth",3).toInt();
	if (color == WHITE)
		color = BLACK;
	else
		color = WHITE;
	game->setGameType(type);
	picture->setComputerColor(color);
	game->setMaxLevel(level);
	game->startNewGame(color);

	actionEndGame->setEnabled(true);
	stackedWidget->setCurrentIndex(0);
	page_2->setVisible(false);
	statusLabel->setText(greeting);
}

void MainWindow::endGame() {
	actionEndGame->setEnabled(false);
	game->endGame();
	picture->clear();
	actionStartNewGame->setEnabled(true);
	page_2->setVisible(true);
	stackedWidget->setCurrentIndex(1);
}

void MainWindow::gameEnded(uint8 status) {
	if(status == myColor)
		statusLabel->setText("YOU WIN! =)");
	else
		statusLabel->setText("YOU LOSE! =(");
	endGame();
}

void MainWindow::settings() {
#ifdef Q_WS_S60
	dialog->setWindowOpacity(0.7);
	dialog->showFullScreen();
#else
	dialog->adjustSize();
	dialog->show();
#endif
}

void MainWindow::about() {
	QMessageBox::about(this, trUtf8("About QCheckers"),
			   trUtf8(
"<h3 align=center>About QCheckers</h3>"
"<P>Developed by"
"<P align=right>Arseniy Sluchevskiy"
"<P align=center>- Russia, Bryansk, 2009 -"));
}


