/*
Author: Lance Cheong Youne
Description: Header file containing the methods and pointers required for the mainwindow.cpp file
Date: February 6, 2024
*/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QPushButton>
#include <QtGui>
#include <QtCore> 
#include <vector>
#include "cell.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
    bool firstClick = true;  // Boolean variable to check if it is the first cell being clicked
    bool eventFilter(QObject *watched, QEvent *event) override; // Event filter to check for mouse button presses

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // Pointers to the gridlayout and button
    QGridLayout *grid;
    QPushButton *button;
    // 2D vector to hold the cells
    std::vector<std::vector<Cell>> board;
    // Number of rows and columns in the grid (game board size)
    const int rows = 16;
    const int cols = 30;

    // Initialize Board
    void initializeBoard();
    // Handle right clicks from user
    void handleRightClick(int, int);
    // Place mines on board
    void placeMines(int, int);
    // Calculates number of adjacent mines for each cell
    void calculateAdjacentMines(); 
    // Handles when cell is clicked
    void cellClicked(int row, int column); 
    // Reveals adjacent cells for cells with 0 adjacent mines
    void revealAdjacentCells(int row, int column); 
    // Updates button according to state
    void updateButton(int row, int column);
    // Checks if win condition is met 
    bool checkWinCondition(); 
    // Reveals all mines when game is lost
    void revealAllMines();
    // Resets the game state and variables
    void resetGame();
};

#endif // MAINWINDOW_H