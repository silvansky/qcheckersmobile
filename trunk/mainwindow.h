#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QMessageBox>

#include "ui_mainwindow.h"
//#include "state.h"
#include "checkersgame.h"
#include "checkersstate.h"

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
private slots:
	void open();
	void save();
	void startNewGame();
	void endGame();
	void settings();
	void hideSettings();
	void about();
	void gameEnded(uint8 status);
	void saveSettings();
	void loadSettings();
private:
	CheckersGame * game;
	int myColor;
	QString greeting;
};

#endif // MAINWINDOW_H
