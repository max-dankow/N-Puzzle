#include <stdlib.h>
#include <vector>
#include "cgamestate.h"
#include "cpuzzlesolver.h"

std::vector<char> generate_target_table(size_t size)
{
    std::vector<char> target_field;
    size_t element_number = size * size;
    for (size_t i = 0; i < element_number; ++i)
    {
        target_field.push_back((i + 1) % (element_number));
    }
    return target_field;
}

int main()
{
    CGameState target(generate_target_table(4), 4);
    target.print_field();
    CPuzzleSolver solver(4, CGameState(generate_target_table(4), 4));
    return EXIT_SUCCESS;
}
