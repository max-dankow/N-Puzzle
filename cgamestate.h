#ifndef CGAMESTATE_H
#define CGAMESTATE_H

#include <stdlib.h>
#include <iostream>
#include <vector>
#include <assert.h>
#include <algorithm>
#include <random>

enum Directions
{
    UP = 0,
    LEFT = 1,
    RIGTH = 2,
    DOWN = 3
};

static std::default_random_engine generator;
const bool DOWN_LEFT_CONER_ZERO = true;

class CGameState
{
public:
    const struct
    {
        ssize_t row_offset, col_offset;
    } moves[4] = {{-1, 0}, {0, -1}, {0, 1}, {1, 0}};

    CGameState(const std::vector<char> &new_field, size_t new_size);
    inline bool operator<(const CGameState &other) const;
    inline bool operator==(const CGameState &other) const;
    CGameState& operator=(const CGameState &source);
    CGameState shuffle_field(size_t shuffles_number) const;
    long calculate_heuristic(const CGameState &target) const;
    bool try_to_move_free_cell(CGameState &new_state, Directions direction) const;
    bool is_solvable(void) const;
    void print_field(void) const;
    size_t size, free_cell_index;
    ssize_t get_index(ssize_t row, ssize_t column) const;
    ssize_t get_row(size_t index) const;
    ssize_t get_column(size_t index) const;
private:
    const ssize_t INVALID_COORDINATES = -1;
    std::vector<char> field;
    size_t get_free_cell_index(void) const;
    void get_target_cells_coordinates(std::vector<std::pair<size_t, size_t> > &correct_position, CGameState target) const;
    long calculate_manhattan_distance(const CGameState &target) const;
    long calculate_linear_conflict(const CGameState &target) const;
    long calculate_tiles_out_of_row_and_col(const CGameState &target) const;
};

inline bool CGameState::operator<(const CGameState &other) const
{
    return this->field < other.field;
}

inline bool CGameState::operator==(const CGameState &other) const
{
    if (this->free_cell_index != other.free_cell_index)
    {
        return 0;
    }
    else
    {
        return this->field == other.field;
    }
}

#endif // CGAMESTATE_H
