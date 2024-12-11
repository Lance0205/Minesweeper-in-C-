#include <vector>
class Cell {
public:
    bool hasMine;
    int adjacentMines;
    bool isRevealed;
    bool isMarked;

    Cell() : hasMine(false), adjacentMines(0), isRevealed(false), isMarked(false) {}
    
};