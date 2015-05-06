#include "cgamestate.h"

CGameState::CGameState(const std::vector<char> &new_field, size_t new_size):
    field(new_field), size(new_size), free_cell_index(get_free_cell_index())
{
    assert(new_field.size() == new_size * new_size);
}

bool CGameState::operator<(const CGameState &other) const
{
    return this->field < other.field;
}

bool CGameState::operator==(const CGameState &other) const
{
    return this->field == other.field;
}

CGameState& CGameState::operator=(const CGameState &source)
{
    if (&source != this)
    {
        this->field = source.field;
        this->free_cell_index = source.free_cell_index;
        this->size = source.size;
    }
    return *this;
}

ssize_t CGameState::get_index(ssize_t row, ssize_t column) const
{
    if (row >= 0 && column >= 0 && row < size && column < size)
    {
        return row * size + column;
    }
    else
    {
        return INVALID_COORDINATES;
    }
}

ssize_t CGameState::get_row(size_t index) const
{
    return index / size;
}

ssize_t CGameState::get_column(size_t index) const
{
    return index % size;
}

void CGameState::print_field(void) const
{
    for (size_t row = 0; row < size; ++row)
    {
        for (size_t col = 0; col < size; ++col)
        {
            std::cout << (int) (field[get_index(row, col)]) << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

size_t CGameState::get_free_cell_index(void) const
{
    auto search_result = std::find(field.begin(), field.end(), 0);
    assert(search_result != field.end());
    return search_result - field.begin();
}

bool CGameState::try_to_move_free_cell(CGameState &new_state, Directions direction) const
{
    ssize_t new_row = get_row(free_cell_index) + moves[direction].row_offset;
    ssize_t new_col = get_column(free_cell_index) + moves[direction].col_offset;
    ssize_t new_index = get_index(new_row, new_col);
    if (new_index == INVALID_COORDINATES)
    {
        return false;
    }
    else
    {
        new_state = *this;
        std::swap(new_state.field[free_cell_index], new_state.field[new_index]);
        new_state.free_cell_index = new_index;
        return true;
    }
}

CGameState CGameState::shuffle_field(size_t shuffles_number) const
{
    CGameState new_random_state = *this;
    std::uniform_int_distribution<int> rand_direction(0, 3);
    for (size_t i = 0; i < shuffles_number; ++i)
    {
        Directions direction = Directions(rand_direction(generator));
        new_random_state.try_to_move_free_cell(new_random_state, direction);
    }
    return new_random_state;
}

void CGameState::get_target_cells_coordinates(std::vector<std::pair<size_t, size_t> > &correct_position, CGameState target) const
{
    correct_position.resize(field.size());
    for (size_t row = 0; row < size; ++row)
    {
        for (size_t col = 0; col < size; ++col)
        {
            size_t index = get_index(row, col);
            int element = (int)(target.field[index]);
            correct_position[element].first = row;
            correct_position[element].second = col;
        }
    }
}

long CGameState::calculate_manhattan_distance(const CGameState &target) const
{
    std::vector<std::pair<size_t, size_t> > correct_position;
    get_target_cells_coordinates(correct_position, target);
    long sum = 0;
    for (size_t row = 0; row < size; ++row)
    {
        for (size_t col = 0; col < size; ++col)
        {
            int number = (int) (field[get_index(row, col)]);
            sum += abs(correct_position[number].first - row)
                + abs(correct_position[number].second - col);
        }
    }
    return sum;
}

long CGameState::calculate_linear_conflict(const CGameState &target) const
{
    std::vector<std::pair<size_t, size_t> > target_tiles_position;
    get_target_cells_coordinates(target_tiles_position, target);
    long sum = 0;
    for (size_t row = 0; row < size; ++row)
    {
        for (size_t col = 0; col < size; ++col)
        {
            size_t current_index = get_index(row, col);
            if (field[current_index] == 0)
            {
                continue;
            }
            if (target_tiles_position[field[current_index]].first == row)
            {
                for (size_t cmp_col = col + 1; cmp_col < size; ++cmp_col)
                {
                    size_t cmp_index = get_index(row, cmp_col);
                    if ((field[cmp_index] != 0) &&
                        (target_tiles_position[field[cmp_index]].first == row) &&
                        (target_tiles_position[field[cmp_index]].second <
                         target_tiles_position[field[current_index]].second))
                    {
                        sum += 2;
                    }
                }
            }
            if (target_tiles_position[field[current_index]].second == col)
            {
                for (size_t cmp_row = row + 1; cmp_row < size; ++cmp_row)
                {
                    size_t cmp_index = get_index(cmp_row, col);
                    if ((field[cmp_index] != 0) &&
                        (target_tiles_position[field[cmp_index]].second == col) &&
                        (target_tiles_position[field[cmp_index]].first <
                         target_tiles_position[field[current_index]].first))
                    {
                        sum += 2;
                    }
                }
            }
        }
    }
    return sum;
}

long CGameState::calculate_tiles_out_of_row_and_col(const CGameState &target) const
{
    std::vector<std::pair<size_t, size_t> > target_tiles_position;
    get_target_cells_coordinates(target_tiles_position, target);
    int sum = 0;
    for (size_t row = 0; row < size; ++row)
    {
        for (size_t col = 0; col < size; ++col)
        {
            ssize_t index = get_index(row, col);
            if (target_tiles_position[field[index]].first != row)
            {
                ++sum;
            }
            if (target_tiles_position[field[index]].second != col)
            {
                ++sum;
            }
        }
    }
    return sum;
}

long CGameState::calculate_heuristic(const CGameState &target) const
{
    long heuristic = 0;
    heuristic += calculate_manhattan_distance(target);
    heuristic += calculate_linear_conflict(target);
    heuristic += calculate_tiles_out_of_row_and_col(target);
    return heuristic;
}

