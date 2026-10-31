#include "PathFind.hpp"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

namespace {

    constexpr int CUSTO_RETO = 10;
    constexpr int CUSTO_DIAGONAL = 14;
    constexpr int INFINITO = std::numeric_limits<int>::max();

    int heuristica(int col, int row, int destinoCol, int destinoRow) {
        const int dx = std::abs(col - destinoCol);
        const int dy = std::abs(row - destinoRow);
        const int diagonais = std::min(dx, dy);
        const int retos = std::max(dx, dy) - diagonais;
        return diagonais * CUSTO_DIAGONAL + retos * CUSTO_RETO;
    }

}

PathFind::PathFind(const int* grid, int width, int height)
    : m_grid(grid), m_width(width), m_height(height) {
    assert(grid != nullptr && "A matriz nao pode ser nula");
    assert(width > 0 && height > 0 && "Dimensoes devem ser positivas");
}

std::vector<Vector2D> PathFind::findPath(const Vector2D& startWorld,
    const Vector2D& goalWorld) const {
    auto livre = [this](int col, int row) {
        return col >= 0 && col < m_width && row >= 0 && row < m_height
            && m_grid[row * m_width + col] == EMPTY_TILE;
        };

    const int inicioCol = static_cast<int>(std::floor(startWorld.x / TILE_SIZE));
    const int inicioRow = static_cast<int>(std::floor(startWorld.y / TILE_SIZE));
    const int destinoCol = static_cast<int>(std::floor(goalWorld.x / TILE_SIZE));
    const int destinoRow = static_cast<int>(std::floor(goalWorld.y / TILE_SIZE));

    if (!livre(inicioCol, inicioRow) || !livre(destinoCol, destinoRow)) {
        return {};
    }

    const int inicio = inicioRow * m_width + inicioCol;
    const int destino = destinoRow * m_width + destinoCol;
    const int totalCelulas = m_width * m_height;
    const std::size_t total = static_cast<std::size_t>(totalCelulas);

    std::vector<int>   g(total, INFINITO);
    std::vector<int>   pai(total, -1);
    std::vector<bool>  fechada(total, false);

    using Item = std::pair<int, int>;
    std::priority_queue<Item, std::vector<Item>, std::greater<Item>> abertos;

    g[static_cast<std::size_t>(inicio)] = 0;
    abertos.push({ heuristica(inicioCol, inicioRow, destinoCol, destinoRow), inicio });

    const int direcoes[8][2] = {
        { 1,  0}, {-1,  0}, { 0,  1}, { 0, -1},
        { 1,  1}, { 1, -1}, {-1,  1}, {-1, -1}
    };

    while (!abertos.empty()) {
        const int atual = abertos.top().second;
        abertos.pop();

        const std::size_t a = static_cast<std::size_t>(atual);
        if (fechada[a]) {
            continue;
        }
        fechada[a] = true;

        if (atual == destino) {
            break;
        }

        const int col = atual % m_width;
        const int row = atual / m_width;

        for (const auto& d : direcoes) {
            const int vCol = col + d[0];
            const int vRow = row + d[1];
            if (!livre(vCol, vRow)) {
                continue;
            }

            const bool diagonal = (d[0] != 0 && d[1] != 0);

            if (diagonal && (!livre(col + d[0], row) || !livre(col, row + d[1]))) {
                continue;
            }

            const int vizinho = vRow * m_width + vCol;
            const std::size_t v = static_cast<std::size_t>(vizinho);
            if (fechada[v]) {
                continue;
            }

            const int novoG = g[a] + (diagonal ? CUSTO_DIAGONAL : CUSTO_RETO);

            if (novoG < g[v]) {
                g[v] = novoG;
                pai[v] = atual;
                const int f = novoG + heuristica(vCol, vRow, destinoCol, destinoRow);
                abertos.push({ f, vizinho });
            }
        }
    }

    if (!fechada[static_cast<std::size_t>(destino)]) {
        return {};
    }

    std::vector<int> celulas;
    for (int c = destino; c != -1; c = pai[static_cast<std::size_t>(c)]) {
        celulas.push_back(c);
    }
    std::reverse(celulas.begin(), celulas.end());

    std::vector<Vector2D> caminho;
    caminho.reserve(celulas.size());
    for (int c : celulas) {
        const int cCol = c % m_width;
        const int cRow = c / m_width;
        caminho.push_back(Vector2D((static_cast<float>(cCol) + 0.5f) * TILE_SIZE,
            (static_cast<float>(cRow) + 0.5f) * TILE_SIZE));
    }
    return caminho;
}