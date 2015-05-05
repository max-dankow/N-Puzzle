#ifndef CPUZZLESOLVER_H
#define CPUZZLESOLVER_H

#include <stdlib.h>
#include <vector>
#include <set>
#include <map>
#include "cgamestate.h"

struct StateInfo
{
    StateInfo (long new_priority, long new_distance, std::map<CGameState, StateInfo> ::iterator new_parent):
        priority(new_priority), distance(new_distance), parent(new_parent){}
    long priority;
    long distance;
    std::map<CGameState, StateInfo>::iterator parent;
};

struct SetElement
{
    SetElement(const CGameState &new_state, long new_priority, long new_distance,
               std::map<CGameState, StateInfo> ::iterator new_parent):
        state(new_state), priority(new_priority), distance(new_distance), parent(new_parent){}

    bool operator<(const SetElement &other) const
    {
        return (this->priority < other.priority) ||
            (this->priority == other.priority && this->state < other.state);
    }

    CGameState state;
    long priority;
    long distance;
    std::map<CGameState, StateInfo>::iterator parent;
};

class CPuzzleSolver
{
public:
    CPuzzleSolver(size_t new_game_size, const CGameState &new_target);
    bool solve_puzzle(const CGameState &start, std::vector<Directions> &answer);
private:
    CGameState target;
    size_t game_size;

    Directions recognize_direction(CGameState start, CGameState finish) const;
    std::vector<Directions> restore_way(const std::map<CGameState, StateInfo> &closed, const CGameState &finish) const;
    bool a_star(const CGameState &start, std::vector<Directions> &answer);
};

#endif // CPUZZLESOLVER_H
