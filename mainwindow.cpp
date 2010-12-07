#include "mainwindow.h"
#include <QStackedLayout>
#include <QLabel>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
	setupUi(this);

	game = new CheckersGame;

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
	// settings
	spinBox->setMinimum(3);
	spinBox->setMaximum(7);
	comboBox->addItem(tr("Russian"));
	comboBox->addItem(tr("International"));
	comboBoxColor->addItem(tr("White"));
	comboBoxColor->addItem(tr("Black"));
	settingsPage->setVisible(false);
	connect(ok, SIGNAL(clicked()), SLOT(saveSettings()));
	connect(cancel, SIGNAL(clicked()), SLOT(hideSettings()));
}

MainWindow::~MainWindow()
{
	delete game;
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
	menuPage->setVisible(false);
	statusLabel->setText(greeting);
}

void MainWindow::endGame() {
	actionEndGame->setEnabled(false);
	game->endGame();
	picture->clear();
	actionStartNewGame->setEnabled(true);
	menuPage->setVisible(true);
	stackedWidget->setCurrentIndex(1);
}

void MainWindow::gameEnded(uint8 status) {
	if(status == myColor)
		statusLabel->setText("YOU WON! =)");
	else
		statusLabel->setText("YOU LOSE! =(");
	endGame();
}

void MainWindow::settings() {
	loadSettings();
	menuPage->setVisible(false);
	settingsPage->setVisible(true);
	stackedWidget->setCurrentIndex(2);
}

void MainWindow::hideSettings() {
	menuPage->setVisible(true);
	settingsPage->setVisible(false);
	stackedWidget->setCurrentIndex(1);
}

void MainWindow::saveSettings() {
	QSettings settings("Arceny","QCheckers");
	int color, type, depth;
	if( comboBox->currentIndex() == 0 )
		type = RUSSIAN;
	else
		type = INTERNATIONAL;
	if( comboBoxColor->currentIndex() == 0)
		color = WHITE;
	else
		color = BLACK;
	depth = spinBox->value();
	settings.setValue("color",color);
	settings.setValue("depth",depth);
	settings.setValue("type",type);
	hideSettings();
}

void MainWindow::loadSettings() {
	QSettings settings("Arceny","QCheckers");

	int color = settings.value("color",WHITE).toInt();
	if(color != WHITE && color !=BLACK)
		color = WHITE;
	int type = settings.value("type",RUSSIAN).toInt();
	if( type != RUSSIAN && type!= INTERNATIONAL )
		type = RUSSIAN;
	int depth = settings.value("depth",3).toInt();
	if( depth < 3 || depth > 7)
		depth = 3;

	if( type == INTERNATIONAL )
		comboBox->setCurrentIndex(1);
	else
		comboBox->setCurrentIndex(0);

	if( color == BLACK )
		comboBoxColor->setCurrentIndex(1);
	else
		comboBoxColor->setCurrentIndex(0);

	spinBox->setValue(depth);
}

void MainWindow::about() {
	QMessageBox::about(this, trUtf8("About QCheckers"),
			   trUtf8(
"<h3 align=center>About QCheckers</h3>"
"<P>Developed by"
"<P align=right>Arseniy Sluchevskiy"
"<P align=center>- Russia, Bryansk, 2009 -"));
}


