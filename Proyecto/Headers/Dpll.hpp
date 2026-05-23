#ifndef DPLL_HPP
#define DPLL_HPP

#include <vector>
#include <array>
#include <cmath>
#include <algorithm>
#include "Global.hpp" // Asumiendo que aqui estan Cell, Position, MAP_WIDTH, etc.

#define pb push_back
#define MAX_VARS 1000

using namespace std;

// Estructura basica para el solver SAT
struct SAT_Solver {
    int num_vars;
    vector<vector<int>> clauses;

    SAT_Solver(int n) : num_vars(n) {}

    void add_clause(vector<int> clause){
        clauses.pb(clause);
    }

    bool dpll(vector<vector<int>> cnf, vector<int> assignment, int depth){
        // Bounded DPLL: Si superamos la profundidad k=5, asumimos que no podemos probar que es una trampa.
        // Retornamos true (Pacman escapa) para abortar esta rama y no exceder el TLE.
        if(depth>5){
            return true;
        }

        // 1. Unit propagation
        bool changed;
        changed = true;
        while(changed){
            changed = false;
            for(int i=0;i<cnf.size();++i){
                if(cnf[i].size()==1){
                    int unit;
                    unit = cnf[i][0];
                    int var;
                    var = abs(unit);
                    bool val;
                    val = (unit>0);
                    
                    if(assignment[var]!=-1&&assignment[var]!=val){
                        return false;
                    }
                    assignment[var] = val;
                    
                    // Limpiar clausulas
                    vector<vector<int>> new_cnf;
                    for(int j=0;j<cnf.size();++j){
                        bool clause_satisfied;
                        clause_satisfied = false;
                        vector<int> reduced_clause;
                        for(int k=0;k<cnf[j].size();++k){
                            if(cnf[j][k]==unit){
                                clause_satisfied = true;
                            }
                            else if(cnf[j][k]!=-unit){
                                reduced_clause.pb(cnf[j][k]);
                            }
                        }
                        if(!clause_satisfied){
                            if(reduced_clause.empty()){
                                return false;
                            }
                            new_cnf.pb(reduced_clause);
                        }
                    }
                    cnf = new_cnf;
                    changed = true;
                    break;
                }
            }
        }

        if(cnf.empty()){
            return true;
        }

        // 3. Branching (Elegir variable no asignada)
        int pick;
        pick = -1;
        for(int i=1;i<=num_vars;++i){
            if(assignment[i]==-1){
                pick = i;
                break;
            }
        }
        if(pick==-1){
            return false;
        }

        // Intentar TRUE (Aumentamos profundidad)
        vector<vector<int>> cnf_true;
        cnf_true = cnf;
        cnf_true.pb({pick});
        if(dpll(cnf_true, assignment, depth+1)){
            return true;
        }

        // Intentar FALSE (Aumentamos profundidad)
        vector<vector<int>> cnf_false;
        cnf_false = cnf;
        cnf_false.pb({-pick});
        return dpll(cnf_false, assignment, depth+1);
    }

    bool solve(){
        vector<int> assignment(num_vars+1, -1);
        return dpll(clauses, assignment, 0); // Iniciamos con profundidad 0
    }
};

// Utilidad para mapear coordenadas 2D a IDs de variable 1D (DPLL usa variables 1..N)
inline int get_var_id(int x, int y){
    // Ajustando las coordenadas al grid de celdas (dividiendo entre CELL_SIZE si es necesario)
    int grid_x = x / CELL_SIZE;
    int grid_y = y / CELL_SIZE;
    return (grid_y * MAP_WIDTH + grid_x) + 1; 
}

// Distancia Manhattan
inline int m_dist(Position a, Position b){
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// Funcion principal que el fantasma naranja llamara
Position calculate_DPLL_move(
    const std::array<std::array<Cell, MAP_HEIGHT>, MAP_WIDTH>& map, 
    const Position& orange_pos, 
    const Position& pacman_pos,
    const vector<Position>& other_ghosts // Recibe a los demas fantasmas para modelar el entorno
){
    // Vectores de direcciones (N, S, E, W)
    int dx[] = {0, 0, CELL_SIZE, -CELL_SIZE};
    int dy[] = {-CELL_SIZE, CELL_SIZE, 0, 0};

    Position best_move = orange_pos;
    bool found_trap = false;

    // Evaluamos cada movimiento posible para el Naranja
    for(int dir=0;dir<4;++dir){
        Position next_orange = {short(orange_pos.x + dx[dir]), short(orange_pos.y + dy[dir])};
        
        // Verificar si es pared (Ajustar segun la estructura Cell de tu repo)
        int g_x = next_orange.x / CELL_SIZE;
        int g_y = next_orange.y / CELL_SIZE;
        if(g_x < 0 || g_x >= MAP_WIDTH || g_y < 0 || g_y >= MAP_HEIGHT) continue;
        if(map[g_x][g_y] == Cell::Wall) continue;

        // Construir la formula SAT asumiendo que el Naranja se mueve a 'next_orange'
        SAT_Solver solver(MAP_WIDTH * MAP_HEIGHT);

        // 1. Clausula Premisa: Pacman intenta escapar desde su posicion actual
        solver.add_clause({get_var_id(pacman_pos.x, pacman_pos.y)});

        // Rango de analisis (evita crear todo el mapa para que corra en < 1ms)
        int radio_busqueda = 8 * CELL_SIZE; 

        for(int x=0;x<MAP_WIDTH;++x){
            for(int y=0;y<MAP_HEIGHT;++y){
                Position curr = {short(x * CELL_SIZE), short(y * CELL_SIZE)};
                int u = get_var_id(curr.x, curr.y);

                if(m_dist(curr, pacman_pos) > radio_busqueda) continue;

                // Si es pared, negamos el escape por aqui
                if(map[x][y] == Cell::Wall){
                    solver.add_clause({-u});
                    continue;
                }

                // Si el Naranja esta aqui, muerte segura
                if(curr.x == next_orange.x && curr.y == next_orange.y){
                    solver.add_clause({-u});
                    continue;
                }

                // Si otro fantasma esta mas cerca de este nodo que pacman, esta bloqueado
                bool blocked_by_ghost = false;
                for(int i=0;i<other_ghosts.size();++i){
                    if(m_dist(curr, other_ghosts[i]) <= m_dist(curr, pacman_pos)){
                        blocked_by_ghost = true; break;
                    }
                }
                if(blocked_by_ghost){
                    solver.add_clause({-u});
                    continue;
                }

                // Si es espacio libre, Escape(u) -> Escape(v1) v Escape(v2) v Escape(v3) v Escape(v4)
                vector<int> transition_clause;
                transition_clause.pb(-u); // -E_u
                
                for(int i=0;i<4;++i){
                    int nx = x * CELL_SIZE + dx[i];
                    int ny = y * CELL_SIZE + dy[i];
                    if(nx >= 0 && nx < MAP_WIDTH * CELL_SIZE && ny >= 0 && ny < MAP_HEIGHT * CELL_SIZE){
                        transition_clause.pb(get_var_id(nx, ny));
                    }
                }
                solver.add_clause(transition_clause);
            }
        }

        // 2. Si es INSATISFACTIBLE (!solver.solve()), significa que este movimiento del Naranja encierra a pacman.
        if(!solver.solve()){
            best_move = next_orange;
            found_trap = true;
            break; 
        }
    }

    // Si no encontramos un movimiento que lo encierre 100%, 
    // hacemos fallback a acercarnos usando Manhattan para seguir metiendo presion
    if(!found_trap){
        int min_dist = 1e9;
        for(int dir=0;dir<4;++dir){
            Position next_pos = {short(orange_pos.x + dx[dir]), short(orange_pos.y + dy[dir])};
            int g_x = next_pos.x / CELL_SIZE;
            int g_y = next_pos.y / CELL_SIZE;
            
            if(g_x >= 0 && g_x < MAP_WIDTH && g_y >= 0 && g_y < MAP_HEIGHT){
                if(map[g_x][g_y] != Cell::Wall){
                    int dist = m_dist(next_pos, pacman_pos);
                    if(dist < min_dist){
                        min_dist = dist;
                        best_move = next_pos;
                    }
                }
            }
        }
    }

    return best_move;
}

#endif
