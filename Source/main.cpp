#include "../Include/Visualization/Grid/Grid.h"
#include "../Include/Simulation/GameOfLife.h"
#include "../Include/Simulation/Constants.h"

int main(void){
    GridClass Grid(N, scale, true);

    Grid.runRender();
    return 0;
}