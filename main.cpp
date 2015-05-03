#include <iostream>
#include <stdio.h>
#include <assert.h>
#include <algorithm>
#include <string.h>
#include <vector>
#include <set>
#include <random>
#include <map>

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

bool operator ==(const SetNode &a, const SetNode &b)
{
    return a.state == b.state;
}

enum Directions
{
    UP = 0,
    LEFT = 1,
    RIGTH = 2,
    DOWN = 3
};

const std::string direction_names[4] = {"UP", "LEFT", "DOWN", "RIGHT"};
static std::default_random_engine generator;

class CPuzzleSolver
{
public:
    void print_field(GameState state);
    CPuzzleSolver(ssize_t new_size):size(new_size),
        element_number(new_size * new_size){}
    GameState encode_vector_to_state(const std::vector<int> &data);//закодировать поле
    bool solve_puzzle(GameState start, GameState goal, GameState trap_state, std::vector<int> &answer);
    GameState generate_random_solvable_state(GameState perfect, size_t element_number);

private:
    ssize_t size;
    ssize_t element_number;
    GameState start, goal, trap_state;

    const struct Offset
    {
        int x[4];
        int y[4];
    } moves = {{0, -1, 0, 1}, {-1, 0, 1, 0}};

    GameState move_free_cell_to(GameState state, Directions direction);
    long long calculate_heuristic(GameState state, GameState goal);
    long long calculate_manhattan_distance(GameState state, GameState goal);
    long long calculate_linear_conflict(GameState state, GameState goal);
    long long calculate_wrong_cells(GameState state, GameState goal);
    int recognize_direction(GameState start, GameState finish);
    std::vector<int> restore_way(const std::map<GameState, Info> &closed, GameState goal);
    bool is_valid_coordinates(int x, int y);
    std::vector<std::pair<size_t, size_t> > get_aim_cells_coordinates(void);
    bool is_solvable(GameState state);
    size_t decode_to_table(GameState state, std::vector<int> &table);
};

bool CPuzzleSolver::is_valid_coordinates(int x, int y)
{
    return (x >= 0 && y >= 0 && x < size && y < size);
}

size_t CPuzzleSolver::decode_to_table(GameState state, std::vector<int> &table)
{
    table.clear();
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
    std::vector<int> table;
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

    return sum % 2 == 0;
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

    int new_x = free_index % size + moves.x[direction];
    int new_y = free_index / size + moves.y[direction];

    if (is_valid_coordinates(new_x, new_y))
    {
        std::swap(table[free_index], table[free_index + moves.x[direction] + moves.y[direction] * size]);
    }
    return encode_vector_to_state(table);
}

std::vector<std::pair<size_t, size_t> > CPuzzleSolver::get_aim_cells_coordinates(void)
{
    std::vector<std::pair<size_t, size_t> > correct_position(element_number);
    GameState local_goal = goal;
    for (size_t i = 0; i < size; ++i)
    {
        for (size_t j = 0; j < size; ++j)
        {
            correct_position[local_goal % element_number].first = i;
            correct_position[local_goal % element_number].second = j;
            local_goal /= element_number;
        }
    }
    return correct_position;
}

long long CPuzzleSolver::calculate_manhattan_distance(GameState state, GameState goal)
{
    std::vector<std::pair<size_t, size_t> > correct_position = get_aim_cells_coordinates();
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
    std::vector<std::pair<size_t, size_t> > aim_cell_position = get_aim_cells_coordinates();
    int sum = 0;
    for (size_t row = 0; row < size; ++row)
    {
        for (size_t col = 0; col < size; ++col)
        {
            if (table[row * size + col] != 0)
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
    std::vector<std::pair<size_t, size_t> > correct_position = get_aim_cells_coordinates();
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
    return calculate_manhattan_distance(start, goal) +
           calculate_linear_conflict(start, goal) +
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
        if(free_index_start - free_index_finish == moves.x[i] + moves.y[i] * size)
            return Directions(i);
    }
    assert(false);
}

std::vector<int> CPuzzleSolver::restore_way(const std::map<GameState, Info> &closed, GameState goal)
{
    auto state_it = closed.find(goal);
    std::vector<int> way;

    while (state_it->second.parent != -1)
    {
        way.push_back(recognize_direction(state_it->first, state_it->second.parent));
        state_it = closed.find(state_it->second.parent);
    }

    std::reverse(way.begin(), way.end());
    return way;
}

bool CPuzzleSolver::solve_puzzle(GameState start, GameState goal, GameState trap_state, std::vector<int> &answer)
{
    assert(size >= 3 && size <= 4);
    /*
    if (size == 4 && !is_solvable(start))
    {
        answer.clear();
        return false;
    }
    */
    this->goal = goal;
    this->start = start;
    this->trap_state = trap_state;
    std::set<SetNode> open;
    std::set<GameState> open_set_to_search;
    std::map<GameState, Info> closed;

    SetNode start_info;
    start_info.state = start;
    start_info.distance = 0;
    start_info.parent = -1;
    start_info.priority = calculate_heuristic(start, goal);
    open.insert(start_info);
    open_set_to_search.insert(start);

    while (open.size() != 0)
    {
        SetNode current = (*open.begin());
        //std::cout << current.priority << '\n';
        open.erase(current);
        open_set_to_search.erase(current.state);

        Info to_close;
        to_close.distance = current.distance;
        to_close.parent = current.parent;
        to_close.priority = current.priority;
        closed.insert(std::make_pair(current.state, to_close));

        if (current.state == goal)
        {
            answer = restore_way(closed, goal);
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

            if (closed.find(new_state) != closed.end())
                continue;

            if (new_state != current.state)
            {
                long long new_way_cost = current.distance + 1;
                /*
                SetNode to_find;
                to_find.state = new_state;
                */
                //std::cout << "try to find state:\n";
                //print_field(new_state);
                auto find_state_info = open_set_to_search.find(new_state);//= std::find(open.begin(), open.end(), to_find);
              //если полученное состояние уже в рассмотрении (в очереди open)
                if (find_state_info != open_set_to_search.end())
                {
                    /*
                    std::cout << "I got back:\n";
                    print_field(find_state_info->state);
                    std::cout << "Last cost was:" << find_state_info->distance << '\n';
                    std::cout << "New cost is:" << current.distance + 1 << '\n';
                    */
                  //если возможно, то улучшаем длинну пути
                    if (find_state_info->distance > new_way_cost)
                    {
                        std::cout << "I CAN IMPROVE! " << find_state_info->distance - new_way_cost << '\n';
                        open_set_to_search.erase(find_state_info->state);
                        open.erase(find_state_info);// !!
                        SetNode update_node;
                        update_node.state = new_state;
                        update_node.parent = current.state;
                        update_node.distance = current.distance + 1;
                        update_node.priority = update_node.distance + calculate_heuristic(new_state, goal);
                        open.insert(update_node);
                        open_set_to_search.insert(new_state);// !!
                    }
                }
                else
                {
                  //получили совершенно новое состояние
                    SetNode new_node;
                    new_node.state = new_state;
                    new_node.parent = current.state;
                    new_node.distance = current.distance + 1;
                    new_node.priority = new_node.distance + calculate_heuristic(new_state, goal);
                    open.insert(new_node);
                    open_set_to_search.insert(new_state);
                }
            }
        }

    }
  //опустошаем список рассмотренных
  //заполняем информацию о стартовой вершине и помещаем в очередь
  //пока очередь рассматриваемых вершин не пуст
      //извлекаем элемент из очереди с наименьшим приоритетом
      //если достигли победного состояния, то восстанавливаем путь
      //для всех соседних вершин:
          //если эту вершину мы уже рассмотрели, то пропускаем
          //если совершенно новая вершина, то добавляем в очередь, заполняем поля
          //если уже в рассмотрении, и можем улучшить, то обновляем вершину
}

/*
 * опен -извлечение минимума, поиск значения..
 * клосд - поиск значения
 * что если дублировать опен? две стуктуры - одни данные, поддерживать равенство....
 *
 ***/

void print_answer(const std::vector<int> &answer)
{
    std::cout << answer.size() << '\n';

    for (int d : answer)
    {
        std::cout << direction_names[d][0];
    }
    std::cout << '\n';
}

void read_input(std::vector<int> &start_table, std::vector<int> &goal_table, ssize_t &size)
{
    //std::cin >> size;
    size = 4;
    start_table.clear();
    goal_table.clear();

    for (int i = 0; i < size; ++i)
    {
        for (int j = 0; j < size; ++j)
        {
            int input_number;
            //std::cin >> input_number;
            //start_table.push_back(input_number);
            goal_table.push_back((i * size + j + 1) % (size * size));
        }
    }
}

GameState CPuzzleSolver::generate_random_solvable_state(GameState perfect, size_t element_number)
{
    /*std::vector<int> table(element_number);
    for (size_t i = 0; i < element_number; ++i)
    {
        table[i] = (i + 1) % element_number;
    }*/
    /*
    std::uniform_int_distribution<int> index(0, element_number - 1);
    for (size_t i = 0; i < 20; ++i)
    {
        std::swap(table[index(generator)], table[index(generator)]);
    }
    */
    GameState local_state = perfect;
    std::uniform_int_distribution<int> rand_direction(0, 3);
    std::uniform_int_distribution<int> moves(1, 3);
    for (size_t i = 0; i < 50; ++i)
    {
        size_t count = moves(generator);
        Directions direction = Directions(rand_direction(generator));
        for (size_t k = 0; k < count; ++k)
        {
            local_state = move_free_cell_to(local_state, direction);
        }
        //print_field(local_state);
        //std::swap(table[index(generator)], table[index(generator)]);
    }
    return local_state;
}

int main()
{
    //freopen("input.txt", "r", stdin);
    std::vector<int> start_table, goal_table;
    ssize_t size;
    read_input(start_table, goal_table, size);

    for (int i = 0; i < 10; ++i)
    {
        CPuzzleSolver solver(size);
        //State start = solver.encode_vector_to_state(start_table);
        GameState goal = solver.encode_vector_to_state(goal_table);
        GameState start = solver.generate_random_solvable_state(goal, size * size);
        std::swap(goal_table[goal_table.size() - 2], goal_table[goal_table.size() - 3]);
        GameState trap = solver.encode_vector_to_state(goal_table);
        solver.print_field(start);
        //solver.print_field(goal);
        //solver.print_field(trap);
        std::vector<int> answer;
        if (solver.solve_puzzle(start, goal, trap, answer))
        {
            print_answer(answer);
        }
        else
        {
            std::cout << "No solution.\n";
        }
    }
    return 0;
}
