#ifndef CGAMESTATE_H
#define CGAMESTATE_H

class CGameState
{
public:
    CGameState(const std::vector<char> &new_field, size_t new_size);
    void print_field(void);
private:
    const ssize_t INVALID_COORDINATES = -1;

    std::vector<char> field;
    size_t size;

    ssize_t get_index(ssize_t row, ssize_t column);
};


#endif // CGAMESTATE_H
