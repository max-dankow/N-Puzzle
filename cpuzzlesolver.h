#ifndef CPUZZLESOLVER_H
#define CPUZZLESOLVER_H

#include <stdlib.h>
#include <vector>
#include <set>
#include <map>
#include <limits>
#include "cgamestate.h"

const bool SHOW_TIME = true;
const size_t LIMIT = 13;

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

enum Algorithm
{
    A_STAR = 0,
    IDA_STAR = 1
};

class CPuzzleSolver
{
public:
    CPuzzleSolver(size_t new_game_size, const CGameState &new_target);
    bool solve_puzzle(const CGameState &start, std::vector<Directions> &answer, Algorithm algo, size_t back_limit);
    bool check_solution(const std::vector<Directions> &answer, const CGameState &start) const;
private:
    const long FOUND = -1;
    CGameState target;
    size_t game_size;

    Directions recognize_direction(CGameState start, CGameState finish) const;
    std::vector<Directions> restore_way(const std::map<CGameState, StateInfo> &closed, const CGameState &finish) const;
    void invert_way(std::vector<Directions> &way);
    bool a_star(const CGameState &start, std::vector<Directions> &answer, size_t back_limit);
    std::map<CGameState, StateInfo> pre_calc(const CGameState &start, std::vector<Directions> &answer, size_t limit);
    bool ida_star(const CGameState &start, std::vector<Directions> &answer);
    long ida_star_search(const CGameState &current, const CGameState &parent, long distance, long bound, std::vector<Directions> &answer) const;
};

#endif // CPUZZLESOLVER_H
