#include <chrono>
#include "cpuzzlesolver.h"

CPuzzleSolver::CPuzzleSolver(size_t new_game_size, const CGameState &new_target):
    target(new_target), game_size(new_game_size) {}

Directions CPuzzleSolver::recognize_direction(CGameState start, CGameState finish) const
{
    ssize_t start_row = start.get_row(start.free_cell_index);
    ssize_t start_col = start.get_column(start.free_cell_index);
    for (int direction = 0; direction < 4; ++direction)
    {
        ssize_t supposed_index = start.get_index(start_row + start.moves[direction].row_offset,
            start_col + start.moves[direction].col_offset);
        if (supposed_index == finish.free_cell_index)
        {
            return Directions(direction);
        }
    }
    assert(false);
}

void CPuzzleSolver::invert_way(std::vector<Directions> &way)
{
    for (auto it = way.begin(); it != way.end(); ++it)
    {
        switch (*it) {
        case LEFT:
            *it = RIGTH;
            break;
        case RIGTH:
            *it = LEFT;
            break;
        case UP:
            *it = DOWN;
            break;
        case DOWN:
            *it = UP;
            break;
        default:
            assert(false);
        }
    }
}

std::vector<Directions> CPuzzleSolver::restore_way(const std::map<CGameState, StateInfo> &closed, const CGameState &finish) const
{
    auto state_it = closed.find(finish);
    assert(state_it != closed.end());
    std::vector<Directions> way;

    while (state_it->second.parent != closed.end())
    {
        way.push_back(recognize_direction(state_it->second.parent->first, state_it->first));
        state_it = state_it->second.parent;
    }
    std::reverse(way.begin(), way.end());
    return way;
}

std::map<CGameState, StateInfo> CPuzzleSolver::pre_calc(const CGameState &start, size_t limit)
{
    std::set<SetElement> open;
    std::map<CGameState, StateInfo> closed;
    open.insert(SetElement(target, target.calculate_heuristic(start), 0, closed.end()));

    while (open.size() != 0)
    {
        SetElement current = (*open.begin());
        open.erase(current);
        StateInfo to_close = StateInfo(current.priority, current.distance, current.parent);
        std::pair<std::map<CGameState, StateInfo>::iterator, bool> insert_result;
        insert_result = closed.insert(std::make_pair(current.state, to_close));

        if (current.distance >= limit)
        {
            continue;
        }
        for (int direction = 0; direction < 4; ++direction)
        {
            CGameState new_state = current.state;
            if (new_state.try_to_move_free_cell(new_state, Directions(direction)))
            {
                if (closed.find(new_state) != closed.end())
                {
                    continue;
                }
                open.insert(SetElement(new_state, current.distance/* + new_state.calculate_heuristic(start)*/ + 1,
                    current.distance + 1, insert_result.first));
            }
        }
    }
    return closed;
}

bool CPuzzleSolver::a_star(const CGameState &start, std::vector<Directions> &answer, size_t back_limit)
{
    std::set<SetElement> open;
    std::map<CGameState, StateInfo> back_wave = pre_calc(start, back_limit);
    std::map<CGameState, StateInfo> closed;
    open.insert(SetElement(start, start.calculate_heuristic(target), 0, closed.end()));

    while (open.size() != 0)
    {
        SetElement current = (*open.begin());
        open.erase(current);
        StateInfo to_close = StateInfo(current.priority, current.distance, current.parent);
        std::pair<std::map<CGameState, StateInfo>::iterator, bool> insert_result;
        insert_result = closed.insert(std::make_pair(current.state, to_close));

        if (back_wave.find(current.state) != back_wave.end())
        {
            answer = restore_way(closed, current.state);
            std::vector<Directions> back_way = restore_way(back_wave, current.state);
            invert_way(back_way);
            std::reverse(back_way.begin(), back_way.end());
            answer.insert(answer.end(), back_way.begin(), back_way.end());
            return true;
        }
        for (int direction = 0; direction < 4; ++direction)
        {
            CGameState new_state = current.state;
            if (new_state.try_to_move_free_cell(new_state, Directions(direction)))
            {
                if (closed.find(new_state) != closed.end())
                {
                    continue;
                }
                open.insert(SetElement(new_state, current.distance + new_state.calculate_heuristic(target) + 1,
                    current.distance + 1, insert_result.first));
            }
        }
    }
    return false;
}

long CPuzzleSolver::ida_star_search(const CGameState &current, const CGameState &parent,
    long distance, long bound, std::vector<Directions> &answer) const
{
    long priority = distance + current.calculate_heuristic(target);
    if (priority > bound)
    {
        return priority;
    }
    if (current == target)
    {
        answer.clear();
        return FOUND;
    }
    long min = std::numeric_limits<long>::max();
    for (int direction = 0; direction < 4; ++direction)
    {
        CGameState new_state = current;
        if (new_state.try_to_move_free_cell(new_state, Directions(direction)) &&
            !(new_state == parent))
        {
            long search_result = ida_star_search(new_state, current, distance + 1, bound, answer);
            if (search_result == FOUND)
            {
                answer.push_back(Directions(direction));
                return FOUND;
            }
            min = std::min(search_result, min);
        }
    }
    return min;
}

bool CPuzzleSolver::ida_star(const CGameState &start, std::vector<Directions> &answer)
{
    long bound = start.calculate_heuristic(target);
    while (true)
    {
        long search_result = ida_star_search(start, start, 0, bound, answer);
        if (search_result == FOUND)
        {
            std::reverse(answer.begin(), answer.end());
            return true;
        }
        if (search_result == std::numeric_limits<long>::max())
        {
            return false;
        }
        bound = search_result;
    }
}

bool CPuzzleSolver::check_solution(const std::vector<Directions> &answer, const CGameState &start) const
{
    CGameState state = start;
    for (Directions direction:answer)
    {
        if (!state.try_to_move_free_cell(state, direction))
        {
            return false;
        }
    }
    return state == target;
}

bool CPuzzleSolver::solve_puzzle(const CGameState &start, std::vector<Directions> &answer, Algorithm algo, size_t back_limit)
{
    auto time_on_start = std::chrono::steady_clock::now();
    if (!start.is_solvable())
    {
        return false;
    }

    bool verdict = false;
    switch (algo)
    {
    case A_STAR:
        verdict = a_star(start, answer, back_limit);
        break;
    case IDA_STAR:
        verdict = ida_star(start, answer);
        break;
    }

    auto time_on_end = std::chrono::steady_clock::now();
    auto work_time = std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(time_on_end - time_on_start).count());
    if (SHOW_TIME)
    {
        std::cout << work_time.count() << " ms.\n";
    }
    return verdict;
}
