#include "TileMap.hpp"
#include "Dungeon2D.hpp"

#include <cassert>
#include <cmath>
#include <fstream>
#include <random>
#include <sstream>
#include <string>
#include <vector>

namespace {
    void cavar(std::vector<int>& celulas, int largura, int altura, const Rect& r) {
        for (int row = r.top; row <= r.bottom; ++row) {
            for (int col = r.left; col <= r.right; ++col) {
                if (col >= 0 && col < largura && row >= 0 && row < altura) {
                    celulas[row * largura + col] = EMPTY_TILE;
                }
            }
        }
    }

    void cavarSalas(const BSPNode* no, std::vector<int>& celulas, int largura, int altura) {
        if (no == nullptr) {
            return;
        }
        if (no->isLeaf()) {
            cavar(celulas, largura, altura, no->room);
            return;
        }
        cavarSalas(no->left.get(), celulas, largura, altura);
        cavarSalas(no->right.get(), celulas, largura, altura);
    }

} 

void TileMap::loadFromDungeon(const Dungeon2D& dungeon) {
    m_width = dungeon.width;
    m_height = dungeon.height;
    m_cells.assign(static_cast<std::size_t>(m_width) * m_height, WALL_TILE);
    cavarSalas(dungeon.root.get(), m_cells, m_width, m_height);
    for (const Corridor& corredor : dungeon.corridors) {
        cavar(m_cells, m_width, m_height, corredor);
    }
}

bool TileMap::saveCSV(const std::string& path) const {
    std::ofstream arquivo(path);
    if (!arquivo) {
        return false;
    }

    for (int row = 0; row < m_height; ++row) {
        for (int col = 0; col < m_width; ++col) {
            if (col > 0) {
                arquivo << ',';
            }
            arquivo << m_cells[row * m_width + col];
        }
        arquivo << '\n';
    }

    return arquivo.good();
}

bool TileMap::loadCSV(const std::string& path) {
    std::ifstream arquivo(path);
    if (!arquivo) {
        return false;
    }
    std::vector<int> novasCelulas;
    int novaLargura = 0;
    int novaAltura = 0;

    std::string linha;
    while (std::getline(arquivo, linha)) {
        if (!linha.empty() && linha.back() == '\r') {
            linha.pop_back();
        }
        if (linha.empty() || linha.back() == ',') {
            return false;
        }
        int colunasNestaLinha = 0;
        std::stringstream leitor(linha);
        std::string valorTexto;
        while (std::getline(leitor, valorTexto, ',')) {
            if (valorTexto == std::to_string(EMPTY_TILE)) {
                novasCelulas.push_back(EMPTY_TILE);
            }
            else if (valorTexto == std::to_string(WALL_TILE)) {
                novasCelulas.push_back(WALL_TILE);
            }
            else {
                return false;
            }
            ++colunasNestaLinha;
        }
        if (novaAltura == 0) {
            novaLargura = colunasNestaLinha;
        }
        else if (colunasNestaLinha != novaLargura) {
            return false;
        }
        ++novaAltura;
    }

    if (novaAltura == 0) {
        return false;
    }
    m_width = novaLargura;
    m_height = novaAltura;
    m_cells = novasCelulas;
    return true;
}

int TileMap::width() const noexcept {
    return m_width;
}

int TileMap::height() const noexcept {
    return m_height;
}

int TileMap::at(int col, int row) const {
    assert(col >= 0 && col < m_width && row >= 0 && row < m_height
        && "Indices fora do mapa");
    return m_cells[row * m_width + col];
}

bool TileMap::isWall(int col, int row) const {
    return at(col, row) == WALL_TILE;
}

const int* TileMap::data() const noexcept {
    return m_cells.data();
}

void TileMap::toIndices(const Vector2D& world, int& col, int& row) const noexcept {
    col = static_cast<int>(std::floor(world.x / TILE_SIZE));
    row = static_cast<int>(std::floor(world.y / TILE_SIZE));
}

Vector2D TileMap::toWorld(int col, int row) const noexcept {
    return Vector2D((static_cast<float>(col) + 0.5f) * TILE_SIZE,
        (static_cast<float>(row) + 0.5f) * TILE_SIZE);
}

void TileMap::randomEmptyCell(int& col, int& row) const {
    std::vector<int> livres;
    for (int i = 0; i < static_cast<int>(m_cells.size()); ++i) {
        if (m_cells[i] == EMPTY_TILE) {
            livres.push_back(i);
        }
    }
    assert(!livres.empty() && "Nao existe nenhuma celula vazia");
    static std::mt19937 gerador(std::random_device{}());
    std::uniform_int_distribution<int> sorteio(0, static_cast<int>(livres.size()) - 1);
    const int indice = livres[sorteio(gerador)];
    col = indice % m_width;
    row = indice / m_width;
}