#ifndef SIMULATION_GAMEOFLIFE_H
#define SIMULATION_GAMEOFLIFE_H

#include "../../Include/Visualization/Grid/Grid.h"

/* will use this to set the quad color state
*/
typedef enum{
    DEAD, 
    ALIVE
}cellState;

/* derive this from the grid class
*/
class GameClass: public GridClass{
    private:
        /* current and next state (dead/alive) of cells
        */
        int *cellCurr, *cellNext;
        /* age of cell, starting from ageInitial + ageIncrement 
         * and maxing out at 1.0
        */
        float *cellAge;
        float ageIncrement, ageInitial;

        int getIdx(int i, int j);
        void swap(int **pOne, int **pTwo);
        int getNeighborStateCnt(int i, int j, cellState state);
        int getAliveNeighborCnt(int i, int j);
        int getDeadNeighborCnt(int i, int j);
        colorVal computeCellColorFromAge(float age);
        void setCellColorFromAge(int i, int j);
        float getRandomAmount(float start, float end);

        bool isCellAlive(int i, int j);
        void setCellAlive(int i, int j, int *arr);
        void setCellDead(int i, int j, int *arr);
        void setRandomCellsAlive(int *arr);

        /* override pure virtual functions
        */
        void getInitialCellStates(void);
        void simulationStep(void);

    public:
        GameClass(int _N, int _scale, bool noStroke);
        ~GameClass(void);
};
#endif /* SIMULATION_GAMEOFLIFE_H
*/
