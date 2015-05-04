#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <string.h>
#include <vector>
#include <set>
#include <map>
#include <random>
#include <chrono>

const size_t TEST_NUMBER = 100;
const size_t RANDOM_MOVES_NUMBER = 10;
const size_t MAX_DEEP_PRECALC = 13;
const size_t MAX_HEURISTIC_PART = 70;
typedef unsigned long long GameState;//тип в котром будем хранить закодированное состояние поля

struct SetNode
{
    GameState state;
    long long priority;
    long long distance;
    GameState parent;
};

struct Info
{
    long long priority;
    long long distance;
    GameState parent;
};

bool operator <(const SetNode &a, const SetNode &b)
{
    return (a.priority < b.priority || (a.priority == b.priority && a.state < b.state));
}

enum Directions
{
    UP = 0,
    LEFT = 1,
    RIGTH = 2,
    DOWN = 3
};

const std::string direction_names[4] = {"UP", "LEFT", "RIGHT", "DOWN"};
static std::default_random_engine generator;

class CPuzzleSolver
{
public:
    void print_field(GameState state);
    CPuzzleSolver(ssize_t new_size):size(new_size),
        element_number(new_size * new_size){}
    GameState encode_vector_to_state(const std::vector<int> &data);//закодировать поле
    bool solve_puzzle(GameState start, GameState goal, GameState trap_state, std::vector<int> &answer);
    GameState generate_random_solvable_state(GameState perfect);

private:
    ssize_t size;
    ssize_t element_number;
    GameState start, goal, trap_state;

    const struct Offset
    {
        int row[4];
        int col[4];
    } moves = {{-1, 0, 0, 1}, {0, -1, 1, 0}};

    GameState move_free_cell_to(GameState state, Directions direction);
    long long calculate_heuristic(GameState state, GameState goal);
    long long calculate_manhattan_distance(GameState state, GameState goal);
    long long calculate_linear_conflict(GameState state, GameState goal);
    long long calculate_wrong_cells(GameState state, GameState goal);
    int recognize_direction(GameState start, GameState finish);
    std::vector<int> restore_way(const std::map<GameState, Info> &closed, GameState goal);
    bool is_valid_coordinates(int row, int col);
    void get_aim_cells_coordinates(std::vector<std::pair<size_t, size_t> > &correct_position, GameState goal);
    bool is_solvable(GameState state);
    size_t decode_to_table(GameState state, std::vector<int> &table);
    void precalc(GameState goal, std::map<GameState, Info> &answer);
    void precalc_heuristic(GameState goal, GameState start, std::map<GameState, Info> &answer);
    void invert_way(std::vector<int> &way);
};

void print_answer(const std::vector<int> &answer)
{
    std::cout << answer.size() << '\n';

    for (int d : answer)
    {
        std::cout << direction_names[d][0];
    }
    std::cout << '\n';
}


bool CPuzzleSolver::is_valid_coordinates(int row, int col)
{
    return (row >= 0 && col >= 0 && row < size && col < size);
}

size_t CPuzzleSolver::decode_to_table(GameState state, std::vector<int> &table)
{
    table.clear();
    table.reserve(element_number);
    GameState local_state = state;
    ssize_t free_index = element_number;

    for (size_t i = 0; i < element_number; ++i)
    {
        size_t element = local_state % element_number;

        if (element == 0)
        {
            free_index = i;
        }

        table.push_back(element);
        local_state /= element_number;
    }
    assert(free_index < element_number);
    return free_index;
}

bool CPuzzleSolver::is_solvable(GameState state)
{
    return true;
    /*std::vector<int> table;
    ssize_t free_index = decode_to_table(state, table);
    int sum = free_index / size + 1;
    table[free_index] = element_number;

    for (size_t i = 0; i < element_number; ++i)
    {
        if (i == free_index)
        {
            continue;
        }
        for (size_t j = i + 1; j < element_number; ++j)
        {
            assert(table[i] != table[j]);
            if (table[j] < table[i])
            {
                ++sum;
            }
        }
    }

    return sum % 2 == 0;*/
}


void CPuzzleSolver::print_field(GameState state)
{
    GameState local_state = state;

    for (size_t i = 0; i < size; ++i)
    {
        for (size_t i = 0; i < size; ++i)
        {
            std::cout << local_state % element_number << ' ';
            local_state /= element_number;
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

GameState CPuzzleSolver::encode_vector_to_state(const std::vector<int> &data)
{
    assert(size > 0);
    assert(data.size() == size * size);
    GameState state = 0;

    for (ssize_t i = data.size() - 1; i >= 0; --i)
    {
        state = state * element_number + data[i];
    }

    return state;
}

GameState CPuzzleSolver::move_free_cell_to(GameState state, Directions direction)
{
    std::vector<int> table;
    ssize_t free_index = decode_to_table(state, table);
    int new_row = free_index / size + moves.row[direction];
    int new_col = free_index % size + moves.col[direction];

    if (is_valid_coordinates(new_row, new_col))
    {
        assert((new_row * size + new_col < element_number) && (new_row * size + new_col >= 0));
        std::swap(table[free_index], table[new_row * size + new_col]);
    }

    return encode_vector_to_state(table);
}

void CPuzzleSolver::get_aim_cells_coordinates(std::vector<std::pair<size_t, size_t> > &correct_position, GameState goal)
{
    correct_position.resize(element_number);
    GameState local_goal = goal;
    for (size_t i = 0; i < size; ++i)
    {
        for (size_t j = 0; j < size; ++j)
        {
            size_t index = local_goal % element_number;
            assert(correct_position[index].first == 0 && correct_position[index].second == 0);
            correct_position[index].first = i;
            correct_position[index].second = j;
            local_goal /= element_number;
        }
    }
}

long long CPuzzleSolver::calculate_manhattan_distance(GameState state, GameState goal)
{
    std::vector<std::pair<size_t, size_t> > correct_position;
    get_aim_cells_coordinates(correct_position, goal);
    int sum = 0;
    for (size_t i = 0; i < size; ++i)
    {
        for (size_t j = 0; j < size; ++j)
        {
            int number = state % element_number;
            sum += abs(correct_position[number].first - i)
                   + abs(correct_position[number].second - j);
            state /= element_number;
        }
    }
    return sum;
}

long long CPuzzleSolver::calculate_linear_conflict(GameState state, GameState goal)
{
    std::vector<int> table;
    decode_to_table(state, table);
    std::vector<std::pair<size_t, size_t> > aim_cell_position;
    get_aim_cells_coordinates(aim_cell_position, goal);
    int sum = 0;
    for (size_t row = 0; row < size; ++row)
    {
        for (size_t col = 0; col < size; ++col)
        {
            if (table[row * size + col] == 0)
            {
                continue;
            }
            if (aim_cell_position[table[row * size + col]].first == row)
            {
                for (size_t cmp_col = col + 1; cmp_col < size; ++cmp_col)
                {
                    if ((table[row * size + cmp_col] != 0) &&
                        (aim_cell_position[table[row * size + cmp_col]].first == row) &&
                        (aim_cell_position[table[row * size + cmp_col]].second <
                         aim_cell_position[table[row * size + col]].second))
                    {
                        sum += 2;
                    }
                }
            }
            if (aim_cell_position[table[row * size + col]].second == col)
            {
                for (size_t cmp_row = row + 1; cmp_row < size; ++cmp_row)
                {
                    if ((table[cmp_row * size + col] != 0) &&
                        (aim_cell_position[table[cmp_row * size + col]].second == col) &&
                        (aim_cell_position[table[cmp_row * size + col]].first <
                         aim_cell_position[table[row * size + col]].first))
                    {
                        sum += 2;
                    }
                }
            }
        }
    }
    return sum;
}

long long CPuzzleSolver::calculate_wrong_cells(GameState state, GameState goal)
{
    std::vector<std::pair<size_t, size_t> > correct_position;
    get_aim_cells_coordinates(correct_position, goal);
    int sum = 0;
    for (size_t i = 0; i < size; ++i)
    {
        for (size_t j = 0; j < size; ++j)
        {
            int number = state % element_number;
            if (correct_position[number].first != i)
            {
                ++sum;
            }
            if (correct_position[number].second != j)
            {
                ++sum;
            }
            state /= element_number;
        }
    }
    return sum;
}

long long CPuzzleSolver::calculate_heuristic(GameState state, GameState goal)
{
    return calculate_manhattan_distance(state, goal) +
           calculate_linear_conflict(state, goal) +
           calculate_wrong_cells(state, goal);
}

int CPuzzleSolver::recognize_direction(GameState start, GameState finish)
{
    ssize_t free_index_start = element_number;
    ssize_t free_index_finish = element_number;

    for (size_t i = 0; i < element_number; ++i)
    {
        if (start % element_number == 0)
        {
            free_index_start = i;
        }
        start /= element_number;

        if (finish % element_number == 0)
        {
            free_index_finish = i;
        }
        finish /= element_number;
    }

    assert(free_index_start < element_number);
    assert(free_index_finish < element_number);

    for (int i = 0; i < 4; ++i)
    {
        if(free_index_start - free_index_finish == moves.row[i] * size + moves.col[i])
            return Directions(i);
    }
    assert(false);
}

std::vector<int> CPuzzleSolver::restore_way(const std::map<GameState, Info> &closed, GameState goal)
{
    auto state_it = closed.find(goal);
    std::vector<int> way;

    std::cout << "!!!!!!!!!!\n";
    while (state_it->second.parent != -1)
    {
        print_field(state_it->second.parent);
        way.push_back(recognize_direction(state_it->first, state_it->second.parent));
        state_it = closed.find(state_it->second.parent);
    }

    std::reverse(way.begin(), way.end());
    return way;
}

void CPuzzleSolver::invert_way(std::vector<int> &way)
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
        }
    }
}

bool CPuzzleSolver::solve_puzzle(GameState start, GameState goal, GameState trap_state, std::vector<int> &answer)
{
    assert(size >= 3 && size <= 4);

    /*if (size == 4 && !is_solvable(start))
    {
        answer.clear();
        return false;

        std::cout << "TRAP may be?\n";
    }*/

    auto time_on_start = std::chrono::steady_clock::now();
    std::map<GameState, Info> back_wave;
    precalc(goal, back_wave);
    precalc_heuristic(goal, start, back_wave);

    auto time_on_end = std::chrono::steady_clock::now();
    auto work_time = std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(time_on_end - time_on_start).count());
    std::cout << "precalc: " << work_time.count() << "ms.\n";
    for (auto it = back_wave.begin(); it != back_wave.end(); ++it)
    {
        assert(it->first != 0);
        print_field(it->first);
    }

    this->goal = goal;
    this->start = start;
    this->trap_state = trap_state;
    std::set<SetNode> open;
    std::map<GameState, Info> closed;
    SetNode start_info;
    start_info.state = start;
    start_info.distance = 0;
    start_info.parent = -1;
    start_info.priority = calculate_heuristic(start, goal);
    open.insert(start_info);

    while (open.size() != 0)
    {
        SetNode current = (*open.begin());
        //print_field(current.state);
        //std::cout << current.priority << '\n';
        open.erase(current);

        if (closed.find(current.state) != closed.end())
        {
            //std::cout << "closed refind! skipping...\n";
            continue;
        }
        Info to_close;
        to_close.distance = current.distance;
        to_close.parent = current.parent;
        to_close.priority = current.priority;
        closed.insert(std::make_pair(current.state, to_close));

        /*if (current.state == goal)
        {
            answer = restore_way(closed, goal);
            return true;
        }*/
        if (back_wave.find(current.state) != back_wave.end())
        {
            //std::cout << "WAVES CONNECTED!!!!";
            answer = restore_way(closed, current.state);
            //print_field(current.state);
            print_answer(answer);
            std::vector<int> back_way = restore_way(back_wave, current.state);
            invert_way(back_way);
            std::reverse(back_way.begin(), back_way.end());
            print_answer(back_way);
            answer.insert(answer.end(), back_way.begin(), back_way.end());
            //getchar();
            return true;
        }
        if (current.state == trap_state)
        {
            answer.clear();
            return false;
        }

        for (int i = 0; i < 4; ++i)
        {
            GameState new_state = move_free_cell_to(current.state, Directions(i));
            //print_field(new_state);
            if (closed.find(new_state) != closed.end())
                continue;

            if (new_state != current.state)
            {
                SetNode new_info;
                new_info.state = new_state;
                new_info.parent = current.state;
                new_info.distance = current.distance + 1;
                new_info.priority = new_info.distance + calculate_heuristic(new_state, goal);
                open.insert(new_info);
            }
        }

    }
}

void CPuzzleSolver::precalc(GameState goal, std::map<GameState, Info> &answer)
{
    assert(size >= 3 && size <= 4);

    /*if (size == 4 && !is_solvable(start))
    {
        answer.clear();
        return false;

        std::cout << "TRAP may be?\n";
    }*/

    std::set<SetNode> open;
    std::map<GameState, Info> closed;
    SetNode start_info;
    start_info.state = goal;
    start_info.distance = 0;
    start_info.parent = -1;
    start_info.priority = start_info.distance;
    open.insert(start_info);
    //size_t deep = 0;

    while (open.size() != 0)
    {
        SetNode current = (*open.begin());
        open.erase(current);
        if (current.distance > MAX_DEEP_PRECALC)
        {
            continue;
        }

        if (closed.find(current.state) != closed.end())
        {
            //std::cout << "closed refind! skipping...\n";
            continue;
        }
        Info to_close;
        to_close.distance = current.distance;
        to_close.parent = current.parent;
        to_close.priority = current.priority;
        closed.insert(std::make_pair(current.state, to_close));
        /*if (to_close.distance == MAX_DEEP_PRECALC || current.state == goal)
        {*/
        answer.insert(std::make_pair(current.state, to_close));
        //}

        for (int i = 0; i < 4; ++i)
        {
            GameState new_state = move_free_cell_to(current.state, Directions(i));
            if (closed.find(new_state) != closed.end())
                continue;

            if (new_state != current.state)
            {
                SetNode new_info;
                new_info.state = new_state;
                new_info.parent = current.state;
                new_info.distance = current.distance + 1;
                new_info.priority = new_info.distance;
                open.insert(new_info);
            }
        }
        //++deep;
    }
}

void CPuzzleSolver::precalc_heuristic(GameState goal, GameState start, std::map<GameState, Info> &answer)
{
    assert(size >= 3 && size <= 4);

    /*if (size == 4 && !is_solvable(start))
    {
        answer.clear();
        return false;

        std::cout << "TRAP may be?\n";
    }*/

    std::set<SetNode> open;
    std::map<GameState, Info> closed;
    SetNode start_info;
    start_info.state = goal;
    start_info.distance = 0;
    start_info.parent = -1;
    print_field(goal);
    print_field(start);
    start_info.priority = start_info.distance + calculate_heuristic(goal, start);
    open.insert(start_info);
    //size_t deep = 0;
    long long end_heuristic = calculate_heuristic(goal, start);
    //std::cout << end_heuristic << "!!!!!!!\n";

    //getchar();
    while (open.size() != 0)
    {
        SetNode current = (*open.begin());
        open.erase(current);
        //std::cout << current.priority - current.distance << "lol\n";
        if (current.priority - current.distance < (long long) (end_heuristic*(1) ||
            current.priority - current.distance > end_heuristic))
        {
            continue;
        }

        if (closed.find(current.state) != closed.end())
        {
            //std::cout << "closed refind! skipping...\n";
            continue;
        }
        Info to_close;
        to_close.distance = current.distance;
        to_close.parent = current.parent;
        to_close.priority = current.priority;
        closed.insert(std::make_pair(current.state, to_close));
        answer.insert(std::make_pair(current.state, to_close));

        for (int i = 0; i < 4; ++i)
        {
            GameState new_state = move_free_cell_to(current.state, Directions(i));
            if (closed.find(new_state) != closed.end())
                continue;

            if (new_state != current.state)
            {
                SetNode new_info;
                new_info.state = new_state;
                new_info.parent = current.state;
                new_info.distance = current.distance + 1;
                new_info.priority = new_info.distance + calculate_heuristic(current.state, start);
                open.insert(new_info);
            }
        }
        //++deep;
    }
}


GameState CPuzzleSolver::generate_random_solvable_state(GameState perfect)
{
    GameState local_state = perfect;
    std::uniform_int_distribution<int> rand_direction(0, 3);
    std::uniform_int_distribution<int> moves(1, 3);
    for (size_t i = 0; i < RANDOM_MOVES_NUMBER; ++i)
    {
        size_t count = moves(generator);
        Directions direction = Directions(rand_direction(generator));
        for (size_t k = 0; k < count; ++k)
        {
            local_state = move_free_cell_to(local_state, direction);
        }
    }
    return local_state;
}

void read_field(std::vector<int> &start_table, size_t size)
{
    start_table.clear();
    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            int input_number;
            std::cin >> input_number;
            start_table.push_back(input_number);
        }
    }
}

std::vector<int> generate_goal_table(size_t element_number)
{
    std::vector<int> goal_table;
    for (int i = 0; i < element_number; ++i)
    {
        goal_table.push_back((i + 1) % (element_number));
    }
    return goal_table;
}

void run_full_testing(size_t size)
{
    std::vector<int> goal_table;
    std::chrono::milliseconds average_time;
    average_time = std::chrono::milliseconds (0);
    std::chrono::milliseconds max_time;
    max_time = std::chrono::milliseconds (0);
    for (int i = 0; i < TEST_NUMBER; ++i)
    {
        CPuzzleSolver solver(size);
        goal_table = generate_goal_table(size * size);
        GameState goal = solver.encode_vector_to_state(goal_table);
        GameState start = solver.generate_random_solvable_state(goal);
        std::swap(goal_table[goal_table.size() - 2], goal_table[goal_table.size() - 3]);
        GameState trap = solver.encode_vector_to_state(goal_table);
        solver.print_field(start);

        std::vector<int> answer;
        auto time_on_start = std::chrono::steady_clock::now();
        if (solver.solve_puzzle(start, goal, trap, answer))
        {
            print_answer(answer);
        }
        else
        {
            std::cout << "No solution.\n";
        }
        auto time_on_end = std::chrono::steady_clock::now();
        auto work_time = std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(time_on_end - time_on_start).count());
        std::cout << work_time.count() << "ms.\n";
        if (work_time > max_time)
        {
            max_time = work_time;
        }
        average_time += work_time;
    }
    average_time /= TEST_NUMBER;
    std::cout << "Average time: " << average_time.count() << "ms.\n";
    std::cout << "Max time: " << max_time.count() << "ms.\n";
}

int main()
{
    run_full_testing(4);
    /*size_t size;
    std::cin >> size;
    CPuzzleSolver solver(size);
    std::vector<int> start_table, goal_table;
    read_field(start_table, size);
    goal_table = generate_goal_table(size * size);
    GameState goal = solver.encode_vector_to_state(goal_table);
    GameState start = solver.generate_random_solvable_state(goal);
    std::swap(goal_table[goal_table.size() - 2], goal_table[goal_table.size() - 3]);
    GameState trap = solver.encode_vector_to_state(goal_table);
    std::vector<int> answer;
    auto time_on_start = std::chrono::steady_clock::now();
    if (solver.solve_puzzle(start, goal, trap, answer))
    {
        print_answer(answer);
    }
    else
    {
        std::cout << "No solution.\n";
    }
    auto time_on_end = std::chrono::steady_clock::now();
    auto work_time = std::chrono::milliseconds(std::chrono::duration_cast<std::chrono::milliseconds>(time_on_end - time_on_start).count());
    std::cout << work_time.count() << "ms.\n";*/
    return 0;
}
