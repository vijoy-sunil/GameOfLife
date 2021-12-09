#include "../../Include/Simulation/GameOfLife.h"
#include "../../Include/Simulation/Constants.h"
#include "../../Include/Utils/Common.h"
#include <stdlib.h>
#include <random>
#include <chrono>
#include <thread>

GameClass::GameClass(int _N, int _scale, bool noStroke): GridClass(_N, _scale, noStroke){
    /* create the 2D grid, we always read from cellCurr and
     * write to cellNext. Use cellNext for simulation and
     * at the end swap them both
    */
    cellCurr = (int*)calloc(N * N, sizeof(int));
    cellNext = (int*)calloc(N * N, sizeof(int));
    /* create age grid, dead cells have age of 0.0 and live
     * cells (starting at cellIncrement) with every time step 
     * increments their age 
    */
    cellAge = (float*)calloc(N * N, sizeof(float));
    ageInitial = 0.0;
    ageIncrement = 0.1;
}

GameClass::~GameClass(void){
    free(cellCurr);
    free(cellNext);
    free(cellAge);
}

/* convert 2D index into 1D value
*/
int GameClass::getIdx(int i, int j){
    return i + (j * N);
}

/* swap two pointers
*/
void GameClass::swap(int **pOne, int **pTwo){
    int *temp = *pOne;
    *pOne = *pTwo;
    *pTwo = temp;
}

/* get count of dead/alive neighbors
*/
int GameClass::getNeighborStateCnt(int i, int j, cellState state){
    int cnt = 0;
    for(int r = -1; r <= 1; r++){
        for(int c = -1; c <= 1; c++){
            /* skip the root cell
            */
            if(r == 0 && c == 0)
                continue;
            if(cellCurr[getIdx(i + c, j + r)] == state)
                cnt++;
        }
    }
    return cnt;
}

/* get number of alive neighbors (8 neighbors) around cell
 * at pos (i, j)
*/
int GameClass::getAliveNeighborCnt(int i, int j){
    return getNeighborStateCnt(i, j, ALIVE);
}

/* get number of dead neighbors (8 neighbors) around cell
 * at pos (i, j)
*/
int GameClass::getDeadNeighborCnt(int i, int j){
    return getNeighborStateCnt(i, j, DEAD);
}

colorVal GameClass::computeCellColorFromAge(float age){
    colorVal cVal = blackVal;
    /* insert any color change pattern here
    */
    cVal.R = age/2.0;
    cVal.G = age/1.0;
    cVal.B = age/5.0;
    return cVal;
}

/* this is a wrapper to the genCellColor(), this allows us to
 * pass the cell age to set the color
 * NOTE: we don't change the cell value in this function, it
 * would be done in the top level function
*/
void GameClass::setCellColorFromAge(int i, int j){
    /* compute color based on age, alpha value is set to 1.0
    */
    colorVal cVal;
    float alpha = 1.0; 
    float age = cellAge[getIdx(i, j)];

    if(age == ageInitial)
        cVal = blackVal;
    else
        cVal = computeCellColorFromAge(age);
    genCellColor(i, j, cVal, alpha);
}

/* given a start and an end range, generate a random number.
*/
float GameClass::getRandomAmount(float start, float end){
    /* At first, the std::random_device object should be 
     * initialized. It produces non-deterministic random bits 
     * for random engine seeding, which is crucial to avoid 
     * producing the same number sequences. Here we use std::
     * default_random_engine to generate pseudo-random values, 
     * but you can declare specific algorithm engine. Next, we
     * initialize a uniform distribution and pass min/max values
     * as optional arguments.
    */
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<> distr(start, end);
    return distr(eng);
}

bool GameClass::isCellAlive(int i, int j){
    return cellCurr[getIdx(i,j)] == ALIVE;
}

void GameClass::setCellAlive(int i, int j, int *arr){
    arr[getIdx(i, j)] = ALIVE;
    /* set color and increment age
    */
    cellAge[getIdx(i, j)] += ageIncrement;
    setCellColorFromAge(i, j);
}

void GameClass::setCellDead(int i, int j, int *arr){
    arr[getIdx(i, j)] = DEAD;
    /* set color and reset age
    */
    cellAge[getIdx(i, j)] = ageInitial;
    setCellColorFromAge(i, j);
}

/* This is one option to start the game, where instead of
 * letting the user select the alive cells before starting 
 * the game, we randomly set cells alive creating weird
 * patterns
*/     
void GameClass::setRandomCellsAlive(int *arr){
    float bias = 0.65;
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            /* get random state 0: DEAD, 1: ALIVE
            */
            float randomNumber = getRandomAmount(0, 1);
            cellState state = randomNumber > bias ? ALIVE : DEAD;

            state == ALIVE ? setCellAlive(i, j, cellCurr) : setCellDead(i, j, cellCurr);
        }
    }
}

void GameClass::getInitialCellStates(void){
#if INIT_RANDOM_PATTERN == 1
    /* In the random init case, the user doesn't have to start
     * the game by pressing key inputs (process input function)
     * hence cellInitialized is set here
    */
    if(!cellInitialized){
        setRandomCellsAlive(cellCurr);
        cellInitialized = true;
    }
#endif

#if INIT_USER_SELECT_PATTERN == 1
    /* if we have not yet finished the initialization phase
     * we can go ahead and set cell state with mouse clicks
     * else ignore them
    */
    if(mouseClicked && !cellInitialized){
        mouseClicked = false;

        mouseAction(xPos, yPos);

        isCellAlive(cellX, cellY) ? setCellDead(cellX, cellY, cellCurr) : 
                                    setCellAlive(cellX, cellY, cellCurr);
    }
#endif
}

/* simulation step that will be used in the render loop.
 * NOTE: we read the cell states from cellCurr and write the
 * final state to cellNext.
 * Finally, swap cellCurr and cellNext
*/
void GameClass::simulationStep(void){
    /* Start simulation only after initialization of cells
     * are complete
    */
    if(cellInitialized){
        /* solw down render time loop
         * NOTE: user key inputs will be missed while the thread is
         * asleep. so keep the sleep time low.
        */
#if INIT_RANDOM_PATTERN == 1
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
#endif
#if INIT_USER_SELECT_PATTERN == 1
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
#endif
        /* ignore mouse clicks while the game/simulation is
         * running
        */
        mouseClicked = false;
        for(int i = 0; i < N; i++){
            for(int j = 0; j < N; j++){
                /* death condition: 
                 * (1) <= 1 neighbor  (solitude)
                 * (2) >= 4 neighbors (overpopulation)
                */
                int numNeighbors = getAliveNeighborCnt(i, j);
                if(numNeighbors <= 1 || numNeighbors >= 4)
                    setCellDead(i, j, cellNext);
                /* survival condition
                 * (1) 2 or 3 neighbors survives if it is already
                 * alive
                */
                else if((numNeighbors == 2 || numNeighbors == 3) && 
                        (isCellAlive(i, j)))
                    setCellAlive(i, j, cellNext);
                /* new cell birth condition
                 * (1) exactly 3 neighbors, a new cell is born
                */
                else if(numNeighbors == 3 && !isCellAlive(i, j))
                    setCellAlive(i, j, cellNext);
                /* if none of the conditions are met, then the cell
                 * is dead
                */
                else    
                    setCellDead(i, j, cellNext);
            }
        }
        /* swap cellNext and cellCurr, so that the next itereation
         * uses the updated values
        */
        swap(&cellNext, &cellCurr);
    }
}
