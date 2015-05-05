#include <stdlib.h>
#include <vector>
#include "cgamestate.h"
#include "cpuzzlesolver.h"

CPuzzleSolver::CPuzzleSolver(size_t new_game_size, const CGameState &new_target):
    target(new_target), game_size(new_game_size) {}

bool CPuzzleSolver::solve_puzzle(CGameState start, std::vector<int> &answer)
{

}

