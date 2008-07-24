#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#define FOR(I, A, B) for(int I = int(A); I < int(B); ++I)
#define REP(I, B) FOR(I, 0, B)
#define SZ(V) (int((V).size()))

const int grp_min[10] = {  0,  1,  3,  6, 10, 15, 21, 28, 36, 45 };
const int grp_max[10] = {  0,  9, 17, 24, 30, 35, 39, 42, 44, 45 };

struct Cell
{
    inline Cell() : var(-1), hgrp(-1), vgrp(-1) { };
    int var, hgrp, vgrp;
};

struct Grid
{
    int rows, cols;
    std::vector<Cell> cells;

    inline Cell *operator[] (int r) { return &cells[cols*r]; }
    inline const Cell *operator[] (int r) const { return &cells[cols*r]; }
};

struct State
{
    int vars, grps;

    std::vector<int> hgrp;               // grp[x] is the horizontal group of x
    std::vector<int> vgrp;               // vgrp[x] is the vertical group of x
    std::vector<int> cand;               // bits in cand[x] are candidates for variable x
    std::vector<int> sum;                // sum[g] is sum for group g
    std::vector<std::vector<int> > mem;  // group[g][m] is member m of group g
};

static const int all_cands = (2<<9) - 2;

inline bool fixed(int c)
{
    return (c&(c-1)) == 0;
}

inline const int value(int n)
{
    int v = 0;
    while(n >>= 1)
        ++v;
    return v;
}

inline const int count(int n)
{
    int c = 0;
    while(n) {
        n &= (n - 1);
        ++c;
    }
    return c;
}

bool read_puzzle(std::istream &is, Grid &grid, State &state)
{
    // Read dimensions and initialize grid
    if(!(is >> grid.rows >> grid.cols))
        return false;
    grid.cells = std::vector<Cell>(grid.rows * grid.cols);

    // Initialize state
    state.vars = 0;
    state.grps = 0;
    state.hgrp.clear();
    state.vgrp.clear();
    state.cand.clear();
    state.sum.clear();
    state.mem.clear();

    // Read input
    std::vector<int> vgrp(grid.cols);
    int hgrp;
    REP(r, grid.rows) REP(c, grid.cols) {
        std::string str;
        if(!(is >> str))
            return false;

        if(str == "?")
        {
            // New variable
            int var = state.vars++;
            state.hgrp.push_back(hgrp);
            state.vgrp.push_back(vgrp[c]);
            state.cand.push_back(all_cands);

            if(hgrp >= 0)
                state.mem[hgrp].push_back(var);

            if(vgrp[c] >= 0)
                state.mem[vgrp[c]].push_back(var);

            grid.cells[r*grid.cols + c].var = var;
        }
        else
        {
            std::string::size_type pos = str.find('\\');
            if(pos == std::string::npos)
                return false;

            if(pos != 0) {
                int sum;
                std::istringstream iss(std::string(str.begin(), str.begin()+pos));
                if(!(iss >> sum))
                    return false;

                // New vertical group
                vgrp[c] = state.grps++;
                state.sum.push_back(sum);
                state.mem.push_back(std::vector<int>());

                grid.cells[r*grid.cols + c].vgrp = vgrp[c];
            }
            else
                vgrp[c] = -1;

            if(pos != str.size() - 1) {
                int sum;
                std::istringstream iss(std::string(str.begin()+pos+1, str.end()));
                if(!(iss >> sum))
                    return false;

                // New horizontal group
                hgrp = state.grps++;
                state.sum.push_back(sum);
                state.mem.push_back(std::vector<int>());

                grid.cells[r*grid.cols + c].hgrp = hgrp;
            }
            else
                hgrp = -1;

        }
    }

    return true;
}

bool read_grid(std::istream &is, Grid &grid, State &state)
{
    // Read dimensions and initialize grid
    if(!(is >> grid.rows >> grid.cols))
        return false;
    grid.cells = std::vector<Cell>(grid.rows * grid.cols);

    // Initialize state
    state.vars = 0;
    state.grps = 0;
    state.sum.clear();
    state.mem.clear();

    // Read data
    REP(r, grid.rows) {
        std::string line;
        if(!(is >> line) || SZ(line) != grid.cols)
            return false;
        REP(c, grid.cols) {
            if(line[c] == '.')
            {
                int var = state.vars++;
                grid.cells[r*grid.cols + c].var = var;
            }
            else
            if(line[c] != '#')
                return false;
        }
    }
    state.hgrp = std::vector<int>(state.vars, -1);
    state.vgrp = std::vector<int>(state.vars, -1);
    state.cand = std::vector<int>(state.vars, all_cands);

    // Create groups
    REP(r, grid.rows) REP(c, grid.cols) if(grid[r][c].var == -1) {
        if( c < grid.cols - 2 &&
            grid[r][c + 1].var >= 0 && grid[r][c + 2].var >= 0 )
        {
            // New horizontal group
            int g = state.grps++;
            grid[r][c].hgrp = g;
            state.mem.push_back(std::vector<int>());
            FOR(d, c + 1, grid.cols) {
                if(grid[r][d].var < 0) break;
                state.mem[g].push_back(grid[r][d].var);
                state.hgrp[grid[r][d].var] = g;
            }
        }

        if( r < grid.rows - 2 &&
            grid[r + 1][c].var >= 0 && grid[r + 2][c].var >= 0 )
        {
            // New vertical group
            int g = state.grps++;
            grid[r][c].vgrp = g;
            state.sum.push_back(0);
            state.mem.push_back(std::vector<int>());
            FOR(s, r + 1, grid.rows) {
                if(grid[s][c].var < 0) break;
                state.mem[g].push_back(grid[s][c].var);
                state.vgrp[grid[s][c].var] = g;
            }
        }

    }
    state.sum = std::vector<int>(state.grps, 0);

    return true;
}

bool write_puzzle(std::ostream &os, const Grid &grid, const State &state, bool write_candidates = false)
{
    os << grid.rows << ' ' << grid.cols;
    REP(n, grid.rows*grid.cols) {
        os << (n%grid.cols ? ' ' : '\n');

        int var = grid.cells[n].var;
        if(var == -1)
        {
            if(grid.cells[n].vgrp != -1)
                os << state.sum[grid.cells[n].vgrp];
            os << '\\';
            if(grid.cells[n].hgrp != -1)
                os << state.sum[grid.cells[n].hgrp];
        }
        else
        {
            if(fixed(state.cand[var]))
            {
                os << value(state.cand[var]);
            }
            else
            if(write_candidates)
            {
                os << '(';
                bool first = true;
                for(int x = 1; x <= 9; ++x)
                {
                    if(state.cand[var] & (1<<x))
                    {
                        if(first)
                            first = false;
                        else
                            os << ',';
                        os << x;
                    }
                }
                os << ')';
            }
            else
                os << '?';
        }
    }
    os << '\n';

    return os;
}

bool test(const State &s, int g, int m, int n, int used, int sum)
{
    if(sum < 0)
        return false;

    if(n == SZ(s.mem[g]))
        return sum == 0;

    if(m == n)
        return test(s, g, m, n + 1, used, sum);

    int var = s.mem[g][n];
    for(int x = 1; x <= 9; ++x)
        if( (used & (1 << x)) == 0 &&
            (s.cand[var] & (1 << x)) )
        {
            if(test(s, g, m, n + 1, used | (1 << x), sum - x))
                return true;
        }

    return false;
}

// Tests wether it's possible to assign digit x to member m of group g
inline bool test(const State &s, int g, int m, int x)
{
    return test(s, g, m, 0, 1 << x, s.sum[g] - x);
}

int reduce_candidates(State &s, int g)
{
    bool changed = false;
    REP(m, SZ(s.mem[g])) {
        int var = s.mem[g][m];

        if(fixed(s.cand[var]))
            continue;

        for(int x = 1; x <= 9; ++x)
            if( (s.cand[var] & (1 << x)) &&
                !test(s, g, m, x) )
            {
                changed = true;
                s.cand[var] ^= (1<<x);
            }

        if(s.cand[var] == 0)
            return -1;
    }
    return changed ? +1 : 0;
}

template<class T>
int solve(State state, T &callback)
{
    // Forward checking
    bool changed;
    do {
        changed = false;
        REP(g, state.grps) {
            int r = reduce_candidates(state, g);
            if(r < 0)
                return 0;
            if(r > 0)
                changed = true;
        }
    } while(changed);

    // Find best cell to backtrack on
    int min_count = 9 + 1, best_cell = -1;
    REP(n, SZ(state.cand)) {

        if(fixed(state.cand[n]))
            continue;

        int c = count(state.cand[n]);
        if(c < min_count)
        {
            min_count = c;
            best_cell = n;
            if(c == 2)
                break;
        }
    }

    if(best_cell == -1)
    {
        const State &arg = state;
        int s = callback(arg);
        return s;
    }

    // Backtrack
    int v = state.cand[best_cell];
    for(int x = 1; x <= 9; ++x)
        if(v & (1 << x))
        {
            state.cand[best_cell] = (1 << x);
            int s = solve(state, callback);
            if(s != 0)
                return s;
        }

    return 0;
}

struct SolutionCounter
{
    int count;

    int operator() (const State &new_state) {
        return (++count < 2) ? 0 : 1;
    }
};

int count_solutions(const State &state) {
    SolutionCounter counter = { 0 };
    solve(state, counter);
    return counter.count;
}

struct SolutionFinder
{
    int count;
    State solution, x;

    int operator() (const State &new_state) {
        if(++count == 1)
        {
            solution = new_state;
            return 0;
        }
        else
        {
            x = new_state;
            return 1;
        }
    }
};

int find_solution(State &state)
{
    SolutionFinder finder = { 0 };
    solve(state, finder);
    if(finder.count > 0)
        state = finder.solution;

    return finder.count;
};

bool construct(State &state, int grps, int g = 0)
{
    state.grps = g;
    switch(count_solutions(state))
    {
    case 0:
        return false;

    case 1:
        if(find_solution(state) != 1)
            std::cerr << "INTERNAL ERROR: find_solution inconsistent with count_solutions!" << std::endl;
        return true;
    }

    if(g < grps)
    {
        // Shuffle possible sums
        std::vector<int> sums;
        int size = state.mem[g].size();
        sums.reserve(grp_max[size] - grp_min[size] + 1);
        FOR(s, grp_min[size], grp_max[size] + 1) sums.push_back(s);
        std::random_shuffle(sums.begin(), sums.end());

        REP(n, SZ(sums)) {
            state.sum[g] = sums[n];
            if(construct(state, grps, g + 1))
                return true;
        }
    }

    return false;
}

void shuffle_groups(State &state, Grid &grid)
{
    std::vector<int> grp_map(state.grps);
    REP(g, state.grps) grp_map[g] = g;
    std::random_shuffle(grp_map.begin(), grp_map.end());

    REP(var, state.vars) if(state.hgrp[var] >= 0) state.hgrp[var] = grp_map[state.hgrp[var]];
    REP(var, state.vars) if(state.vgrp[var] >= 0) state.vgrp[var] = grp_map[state.vgrp[var]];
    std::vector<std::vector<int> > new_mem(state.grps);
    REP(g, state.grps) new_mem[grp_map[g]] = state.mem[g];
    state.mem = new_mem;
    REP(r, grid.rows) REP(c, grid.cols) if(grid[r][c].hgrp >= 0) grid[r][c].hgrp = grp_map[grid[r][c].hgrp];
    REP(r, grid.rows) REP(c, grid.cols) if(grid[r][c].vgrp >= 0) grid[r][c].hgrp = grp_map[grid[r][c].vgrp];
}

void write_cplex(std::ostream &os, const State &state)
{
    os << "Maximize\n obj: X1@1\nSubject To\n";

    // One digit per empty cell
    REP(var, state.vars) {
        os << " c" << var << ": ";
        FOR(digit, 1, 10) {
            if(digit != 1)
                os << " + ";
            os << 'X' << digit << '@' << var;
        }
        os << " = 1\n";
    }

    // At most one of each digit per cluster
    REP(g, state.grps) FOR(digit, 1, 10) {
        // if(digit > state.sum[g])
        //     break;

        bool first = true;
        os << " d" << g << ',' << digit << ": ";
        REP(m, state.mem[g].size()) {
            if(first)
                first = false;
            else
                os << " + ";
            os << 'X' << digit << '@' << state.mem[g][m];
        }
        os << " <= 1\n";
    }

    // Sum of digits in cluster
    REP(g, state.grps) {
        bool first = true;
        os << " e" << g << ": ";
        REP(m, state.mem[g].size()) FOR(digit, 1, 10) {
            if(first)
                first = false;
            else
                os << " + ";
            os << digit << 'X' << digit << '@' << state.mem[g][m];
        }
        //os << " = " << state.sum[g] << '\n';
        os << " - S" << g << " = 0\n";
    }

    os << "Binary\n";
    REP(var, state.vars) FOR(digit, 1, 10)  os << " X" << digit << '@' << var;
    os << "\nEnd\n";
}

int main(int argc, char *argv[])
{
    // Initialize RNG
    std::srand(std::time(NULL));

    // Parse command line arguments
    int mode = -1;
    bool failed = false;
    FOR(a, 1, argc) {
        std::string arg = argv[a];
        if(mode == -1 && arg == "--solve" || arg == "-s")
            mode = 0;
        else
        if(mode == -1 && arg == "--generate" || arg == "-g")
            mode = 1;
        else
        if(mode == -1 && arg == "--cplex" || arg == "-c")
            mode = 2;
        else
            failed = true;
    }

    if(mode == -1 || failed)
    {
        std::cout <<
            "Usage:" << argv[0] << " [OPTION]\n"
            "Solves or generates Kakuro puzzles.\n\n"
            "  -s, --solve     solves the puzzles provided\n"
            "  -g, --generate  generates puzzles based on the grid provided\n"
            "  -c, --cplex     converts the puzzle to CPLEX format\n"
            << std::flush;
        return (argc == 1) ? 0 : 1;
    }

    Grid grid;
    State state;
    int no = 0;
    while( (mode != 1 && read_puzzle(std::cin, grid, state)) ||
           (mode == 1 && read_grid(std::cin, grid, state)) )
    {
        ++no;
        switch(mode)
        {
        case 0:
            {   // Solve puzzle
                int count = find_solution(state);
                if(count != 1)
                {
                    std::cerr << "Error: puzzle " << no << " has "
                            << (count == 0 ? "no solution" : "multiple solutions")
                            << "!" << std::endl;
                }
            } break;

        case 1:
            {   // Generate puzzle
                int grps = state.grps;
                //shuffle_groups(state, grid);
                if(!construct(state, grps))
                    std::cerr << "Error: there is no puzzle for grid " << no << "!" << std::endl;

                state.grps = grps;

                // Remove givens
                REP(v, state.vars) state.cand[v] = all_cands;
            } break;

        case 2:
            {
                write_cplex(std::cout, state);
            } continue;
        }

        write_puzzle(std::cout, grid, state);
    }

    if(!std::cin.eof())
    {
        std::cerr << "Failed to read puzzle " << (no + 1) << " !" << std::endl;
        return 1;
    }

    return 0;
}
