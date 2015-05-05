#include <stdlib.h>
#include <vector>
#include "cgamestate.h"
#include "cpuzzlesolver.h"

const int GAME_SIZE = 3;
const size_t RANDOM_MOVES_NUMBER = 1000;
const bool DOWN_LEFT_CONER_ZERO = true;
const std::string direction_names[4] = {"UP", "LEFT", "RIGHT", "DOWN"};

std::vector<char> generate_target_table(size_t size)
{
    std::vector<char> target_field;
    size_t element_number = size * size;
    for (size_t i = 0; i < element_number; ++i)
    {
        if (DOWN_LEFT_CONER_ZERO)
        {
            target_field.push_back((i + 1) % (element_number));
        }
        else
        {
            target_field.push_back(i % (element_number));
        }
    }
    return target_field;
}

void print_answer(const std::vector<Directions> &answer)
{
    std::cout << answer.size() << '\n';

    for (int d : answer)
    {
        std::cout << direction_names[d][0];
    }
    std::cout << '\n';
}

int main()
{
    int size = GAME_SIZE;
    CGameState target(generate_target_table(size), size);
    CGameState test_state = target.shuffle_field(RANDOM_MOVES_NUMBER);
    std::vector<Directions> answer;
    CPuzzleSolver solver(size, CGameState(generate_target_table(size), size));
    solver.solve_puzzle(test_state, answer);
    target.print_field();
    test_state.print_field();
    print_answer(answer);
    return EXIT_SUCCESS;
}
