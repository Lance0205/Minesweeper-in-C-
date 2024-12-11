/*
Author: Lance Cheong Youne
Description: Creates a grid layout of push buttons and saves the gamestate in a 2D vector
Date: February 6, 2024
*/
#include "mainwindow.h"
#include <QRandomGenerator>
#include <QMessageBox>
#include <QPushButton>

using namespace std;

// Constructor
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  // Create a central widget for the QMainWindow
  QWidget *centralWidget = new QWidget(this);  

  // Initialize the grid layout
  grid = new QGridLayout;

  // Initialize the game board
  initializeBoard();

  // Create buttons for each cell in each row and column
  for(int i = 0; i < rows; i++)
  {
    for(int j = 0; j < cols; j++){
      button = new QPushButton(this);
      button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      button->setFixedSize(30, 30); // Set button size to avoid any resizing issues
      button->installEventFilter(this); // Install event filter on the button

      connect(button, &QPushButton::clicked, this, [this, i, j]() {
        // Handle the button click for button at row i, column j
        cellClicked(i, j);
      });

      // Connect button signals to desired function
      grid->addWidget(button, i, j);
    }
  }

  // Apply the grid layout to the central widget
  centralWidget->setLayout(grid);
    
  // Set the central widget of the QMainWindow
  setCentralWidget(centralWidget);
}

// Method that sets up the board by resizing the vector to the number of specified elements
void MainWindow::initializeBoard() {
    board.resize(rows, std::vector<Cell>(cols));
}

// Method that triggers when the user clicks a button on the window
bool MainWindow::eventFilter(QObject *object, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::RightButton) {
            // Retrieve the button that was clicked
            QPushButton *button = qobject_cast<QPushButton *>(object);
            if (button) {
                // Find the button's position in the grid
                const int index = grid->indexOf(button);
                const int row = index / cols;
                const int col = index % cols;
                // Handle the right click at button's location
                handleRightClick(row, col);
                return true; 
            }
        }
    }
    // Pass the event on to the base class
    return QMainWindow::eventFilter(object, event);
}

// Method to handle right clicks as marking/unmarking
void MainWindow::handleRightClick(int row, int column) {
    // Toggle the marked state and update the button look
    Cell &cell = board[row][column];
    if (!cell.isRevealed) {
        cell.isMarked = !cell.isMarked;
        updateButton(row, column);
    }
}

// Method triggered when user clicks on a cell
void MainWindow::cellClicked(int row, int column) {
    // Retrieve the layout item at the specified row and column
    QLayoutItem* layoutItem = grid->itemAtPosition(row, column);

    // Get the widget associated with the layout item
    QWidget* widget = layoutItem->widget();

    // Cast the widget to a QPushButton pointer
    QPushButton* button = qobject_cast<QPushButton*>(widget);
    
    //  If it is first click, reveal then
    if (firstClick) {
        button->setStyleSheet("background-color: grey");
        button->setIconSize(QSize(26, 26));
        placeMines(row, column);  // Place mines after the first click, ignoring the first clicked cell
        firstClick = false;
        return;
    }

    Cell& cell = board[row][column];
    // If cell is revealed or is marked
    if (cell.isRevealed || cell.isMarked) return;
    cell.isRevealed = true;
    // If cell with mine is clicked
    if (cell.hasMine) {
        revealAllMines(); // explode all mines
        
        // Display a gameover window with 2 options: Reset the game or Close the game
        QMessageBox gameOverMsgBox;
        gameOverMsgBox.setWindowTitle("Uh Oh!");
        gameOverMsgBox.setText("You have hit a mine and exploded.");
        gameOverMsgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
        gameOverMsgBox.button(QMessageBox::Retry)->setText("Play Again!");
        gameOverMsgBox.button(QMessageBox::Cancel)->setText("Exit");
        int ret = gameOverMsgBox.exec();

        // Reset game or close game depending on user choice
        switch (ret) {
            case QMessageBox::Retry:
                resetGame();
                break;
            case QMessageBox::Cancel:
                close();
                break;
            default:
                break;
        }
        
    } else {
        // call the revealAdjacentCells when a cell with no adjacent mine is clicked
        if (cell.adjacentMines == 0) {
            revealAdjacentCells(row, column);
        }
        updateButton(row, column);

        // Display a win window with 2 options: Reset the game or Close the game
        if (checkWinCondition()) {
            QMessageBox gameOverMsgBox;
            gameOverMsgBox.setWindowTitle("Ding!");
            gameOverMsgBox.setText("Congratulations, you cleared all the mines!");
            gameOverMsgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Cancel);
            gameOverMsgBox.button(QMessageBox::Retry)->setText("Play Again!");
            gameOverMsgBox.button(QMessageBox::Cancel)->setText("Exit");
            int ret = gameOverMsgBox.exec();

            // Reset game or close game depending on user choice
            switch (ret) {
                case QMessageBox::Retry:
                    resetGame();
                    break;
                case QMessageBox::Cancel:
                    close();
                default:
                    break;
            }
        }
    }
}

// Method to place 99 mines randomly across the grid (using QRandomGenerator)
void MainWindow::placeMines(int firstRow,int firstCol) {
    //cout << "help";
    int mineCount = 0;
    while (mineCount < 99) {
        int row = QRandomGenerator::global()->bounded(rows);
        int col = QRandomGenerator::global()->bounded(cols);

        // Check if the randomly chosen cell is not the first clicked cell or its neighbors
        if (!board[row][col].hasMine && row != firstRow && col != firstCol ) {
            board[row][col].hasMine = true;
            mineCount++;
        }
    }
    calculateAdjacentMines();  // After mines are placed, calculate the adjacent mines
}

// Method to calculate the amount of adjacent mines to the specified cell
void MainWindow::calculateAdjacentMines() {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (!board[i][j].hasMine) {
                int count = 0;
                for (int k = -1; k <= 1; ++k) {
                    for (int l = -1; l <= 1; ++l) {
                        int m = i + k, n = j + l;
                        if (m >= 0 && m < rows && n >= 0 && n < cols && board[m][n].hasMine) { //within range and has a mine
                            count++;
                        }
                    }
                }
                board[i][j].adjacentMines = count;
            }
        }
    }
}

// Methods that causes the cascading effect when a cell with no adjacent mine is clicked
void MainWindow::revealAdjacentCells(int row, int column) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int newRow = row + i;
            int newCol = column + j;
            // Check bounds
            if (newRow >= 0 && newRow < rows && newCol >= 0 && newCol < cols) {
                Cell& adjacentCell = board[newRow][newCol];
                // Only reveal if not already revealed and not marked
                if (!adjacentCell.isRevealed && !adjacentCell.isMarked) {
                    adjacentCell.isRevealed = true;
                    updateButton(newRow, newCol);
                    if (adjacentCell.adjacentMines == 0) {
                        revealAdjacentCells(newRow, newCol);
                    }
                }
            }
        }
    }
}

// Method that changes the button's appearance depending on if it is revealed, marked or mine
void MainWindow::updateButton(int row, int column) {
    // Retrieve the layout item at the specified row and column
    QLayoutItem* layoutItem = grid->itemAtPosition(row, column);

    // Get the widget associated with the layout item
    QWidget* widget = layoutItem->widget();

    // Cast the widget to a QPushButton pointer
    QPushButton* button = qobject_cast<QPushButton*>(widget);
    
    Cell& cell = board[row][column];

    if (cell.isRevealed) {
        if (cell.hasMine) {
            // If the game is lost and this is the mine that was clicked, show the exploded mine icon
            button->setIcon(QIcon("./bomb_explode.png"));
            button->setIconSize(QSize(26, 26));
          
        } else if (cell.adjacentMines > 0) {
            button->setText(QString::number(cell.adjacentMines));

            // Use different colours for different number of adjacent mines
            if (cell.adjacentMines == 1){
                button->setStyleSheet("color: blue;");
            }
            else if (cell.adjacentMines == 2) {
                button->setStyleSheet("color: green");
            }
            else if (cell.adjacentMines == 3) {
                button->setStyleSheet("color: red");
            }
            else if (cell.adjacentMines == 4) {
                button->setStyleSheet("color: yellow");
            }
            else if (cell.adjacentMines == 5) {
                button->setStyleSheet("color: purple");
            }
            else if (cell.adjacentMines == 6) {
                button->setStyleSheet("color: orange");
            }
            else if (cell.adjacentMines == 7) {
                button->setStyleSheet("color: brown");
            }
            else if (cell.adjacentMines == 8) {
                button->setStyleSheet("color: black");
            }
            
        } else {
            // For cells with no adjacent mines, set a grey background
            button->setStyleSheet("background-color: grey"); 
        }
        button->setEnabled(true);
    } 
    else if (cell.isMarked) {
        // If the cell is marked as a mine by the user, show the flag icon
        button->setIcon(QIcon("./mine_flag.png"));
        button->setIconSize(QSize(26, 26));
    } 
    
    else {
        // Clear the icon if the cell is not marked
        button->setIcon(QIcon());
        button->setEnabled(true); // Ensure the button is enabled
    }
}

// Method that reterns true if win condition (all mines are cleared) has been reached or false otherwise
bool MainWindow::checkWinCondition() {
  // If mines are not placed yet, the player cannot win.
    if (firstClick) {
        return false;
    }
    for (const auto& row : board) {
        for (const auto& cell : row) {
            if (!cell.hasMine && !cell.isRevealed) {
                return false; // Found a non-mine cell that is not revealed
            }
        }
    }
    return true; // All non-mine cells are revealed
}

// Method that reveals all mines on the board
void MainWindow::revealAllMines() {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            Cell& cell = board[i][j];
            if (cell.hasMine) {
                cell.isRevealed = true;
                updateButton(i, j);
            }
        }
    }
}

// Method that takes care of garbage collection and resetting the game
void MainWindow::resetGame() {
    // Clear the board
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            board[i][j] = Cell(); // Reset each cell to the default state
            QPushButton* button = qobject_cast<QPushButton*>(grid->itemAtPosition(i, j)->widget());
            button->setEnabled(true);
            button->setIcon(QIcon());
            button->setText("");
            button->setStyleSheet("");
        }
    }

    // Reset game state variables
    firstClick = true;
}

MainWindow::~MainWindow() {

}