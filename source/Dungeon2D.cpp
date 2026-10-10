#include "Dungeon2D.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <random>

namespace {

    constexpr int TAMANHO_MINIMO_SALA = 5;
    constexpr int LARGURA_CORREDOR = 3;   
    constexpr int MARGEM = 1;   
    constexpr int TAMANHO_MINIMO_REGIAO = TAMANHO_MINIMO_SALA + 2 * MARGEM;  
    int larguraDe(const Rect& r) {
        return r.right - r.left + 1;
    }

    int alturaDe(const Rect& r) {
        return r.bottom - r.top + 1;
    }

    int sortear(std::mt19937& gerador, int minimo, int maximo) {
        const unsigned quantidade = static_cast<unsigned>(maximo - minimo + 1);
        return minimo + static_cast<int>(gerador() % quantidade);
    }

    void criarSala(BSPNode& no, std::mt19937& gerador) {
        const Rect& regiao = no.region;

        const int larguraSala = sortear(gerador, TAMANHO_MINIMO_SALA, larguraDe(regiao) - 2 * MARGEM);
        const int alturaSala = sortear(gerador, TAMANHO_MINIMO_SALA, alturaDe(regiao) - 2 * MARGEM);

        const int esquerda = sortear(gerador, regiao.left + MARGEM, regiao.right - MARGEM - larguraSala + 1);
        const int topo = sortear(gerador, regiao.top + MARGEM, regiao.bottom - MARGEM - alturaSala + 1);

        no.room.left = esquerda;
        no.room.top = topo;
        no.room.right = esquerda + larguraSala - 1;
        no.room.bottom = topo + alturaSala - 1;
    }

    void dividir(BSPNode& no, std::mt19937& gerador) {
        const int largura = larguraDe(no.region);
        const int altura = alturaDe(no.region);
        const bool podeCortarVertical = largura >= 2 * TAMANHO_MINIMO_REGIAO;
        const bool podeCortarHorizontal = altura >= 2 * TAMANHO_MINIMO_REGIAO;

        if (!podeCortarVertical && !podeCortarHorizontal) {
            criarSala(no, gerador);  
            return;
        }

        bool cortarVertical;
        if (podeCortarVertical && podeCortarHorizontal) {
            if (largura > altura) {
                cortarVertical = true;
            }
            else if (altura > largura) {
                cortarVertical = false;
            }
            else {
                cortarVertical = (sortear(gerador, 0, 1) == 0);
            }
        }
        else {
            cortarVertical = podeCortarVertical;
        }

        no.left = std::make_unique<BSPNode>();
        no.right = std::make_unique<BSPNode>();
        no.left->region = no.region;
        no.right->region = no.region;

        if (cortarVertical) {
            const int corte = sortear(gerador,
                no.region.left + TAMANHO_MINIMO_REGIAO - 1,
                no.region.right - TAMANHO_MINIMO_REGIAO);
            no.left->region.right = corte;
            no.right->region.left = corte + 1;
        }
        else {
            const int corte = sortear(gerador,
                no.region.top + TAMANHO_MINIMO_REGIAO - 1,
                no.region.bottom - TAMANHO_MINIMO_REGIAO);
            no.left->region.bottom = corte;
            no.right->region.top = corte + 1;
        }
        dividir(*no.left, gerador);
        dividir(*no.right, gerador);
    }

    void coletarSalas(const BSPNode& no, std::vector<Rect>& salas) {
        if (no.isLeaf()) {
            salas.push_back(no.room);
            return;
        }
        coletarSalas(*no.left, salas);
        coletarSalas(*no.right, salas);
    }

    struct Ponto {
        int col;
        int row;
    };

    Ponto centro(const Rect& r) {
        return Ponto{ (r.left + r.right) / 2, (r.top + r.bottom) / 2 };
    }

    void ligar(const Rect& salaA, const Rect& salaB, std::vector<Corridor>& corredores) {
        const Ponto a = centro(salaA);
        const Ponto b = centro(salaB);
        const int meio = LARGURA_CORREDOR / 2; 

        if (a.col != b.col) {
            Corridor horizontal;
            horizontal.left = std::min(a.col, b.col) - meio;
            horizontal.right = std::max(a.col, b.col) + meio;
            horizontal.top = a.row - meio;
            horizontal.bottom = a.row + meio;
            corredores.push_back(horizontal);
        }

        if (a.row != b.row) {
            Corridor vertical;
            vertical.left = b.col - meio;
            vertical.right = b.col + meio;
            vertical.top = std::min(a.row, b.row) - meio;
            vertical.bottom = std::max(a.row, b.row) + meio;
            corredores.push_back(vertical);
        }
    }

    void conectar(const BSPNode& no, std::vector<Corridor>& corredores) {
        if (no.isLeaf()) {
            return;
        }

        conectar(*no.left, corredores);
        conectar(*no.right, corredores);

        std::vector<Rect> salasEsquerda;
        std::vector<Rect> salasDireita;
        coletarSalas(*no.left, salasEsquerda);
        coletarSalas(*no.right, salasDireita);
        Rect melhorA = salasEsquerda[0];
        Rect melhorB = salasDireita[0];
        int menorDistancia = -1;

        for (const Rect& salaA : salasEsquerda) {
            for (const Rect& salaB : salasDireita) {
                const Ponto ca = centro(salaA);
                const Ponto cb = centro(salaB);
                const int distancia = std::abs(ca.col - cb.col) + std::abs(ca.row - cb.row);
                if (menorDistancia < 0 || distancia < menorDistancia) {
                    menorDistancia = distancia;
                    melhorA = salaA;
                    melhorB = salaB;
                }
            }
        }

        ligar(melhorA, melhorB, corredores);
    }

}

Dungeon2D::Dungeon2D(int widthTiles, int heightTiles)
    : width(widthTiles), height(heightTiles) {
    assert(widthTiles > 0 && heightTiles > 0 && "Dimensoes devem ser positivas");
}

void Dungeon2D::generate(unsigned seed) {
    assert(width >= TAMANHO_MINIMO_REGIAO && height >= TAMANHO_MINIMO_REGIAO
        && "Masmorra pequena demais para uma sala 5x5");
    std::mt19937 gerador(seed);
    corridors.clear();
    root = std::make_unique<BSPNode>();
    root->region.left = 0;
    root->region.top = 0;
    root->region.right = width - 1;
    root->region.bottom = height - 1;
    dividir(*root, gerador);
    conectar(*root, corridors);
}