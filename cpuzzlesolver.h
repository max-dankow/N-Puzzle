#ifndef CPUZZLESOLVER_H
#define CPUZZLESOLVER_H


class CPuzzleSolver
{
public:
    CPuzzleSolver(size_t new_game_size, const CGameState &new_target);
    bool solve_puzzle(CGameState start, std::vector<int> &answer);
private:
    CGameState target;
    size_t game_size;
};

#endif // CPUZZLESOLVER_H
