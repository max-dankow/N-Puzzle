#include <stdlib.h>
#include <iostream>
#include <vector>
#include <assert.h>
#include "cgamestate.h"

CGameState::CGameState(const std::vector<char> &new_field, size_t new_size):
    field(new_field), size(new_size)
{
    assert(new_field.size() == new_size * new_size);
}

ssize_t CGameState::get_index(ssize_t row, ssize_t column)
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

void CGameState::print_field(void)
{
    for (size_t row = 0; row < size; ++row)
    {
        for (size_t col = 0; col < size; ++col)
        {
            std::cout << (int)(field[get_index(row, col)]) << ' ';
        }
        std::cout << '\n';
    }
    std::cout << '\n';
}

