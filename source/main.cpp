#include <SFML/Graphics.hpp>
#include "Dungeon2D.hpp"
#include "TileMap.hpp"
#include "RigidBody2D.hpp"
#include "Collision2D.hpp"

const int LARGURA_MAPA = 25;
const int ALTURA_MAPA = 15;

const float DT = 1.0f / 60.0f;
const float VELOCIDADE_JOGADOR = 250.0f;

struct Jogador {
    RigidBody2D corpo;
    Collision2D forma;
};

void gerarSala(TileMap& mapa, unsigned semente) {
    Dungeon2D masmorra(LARGURA_MAPA, ALTURA_MAPA);
    masmorra.generate(semente);
    mapa.loadFromDungeon(masmorra);
}

void posicionarJogador(Jogador& jogador, const TileMap& mapa) {
    int col = 0;
    int row = 0;
    mapa.randomEmptyCell(col, row);
    jogador.corpo.position = mapa.toWorld(col, row);
    jogador.corpo.velocity = Vector2D(0.0f, 0.0f);
}

Vector2D lerDirecao() {
    Vector2D direcao(0.0f, 0.0f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  direcao.x -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) direcao.x += 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))    direcao.y -= 1.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))  direcao.y += 1.0f;

    if (direcao.length_squared() > 0.0f) {
        direcao.normalize();
    }
    return direcao;
}

bool colideComParede(const TileMap& mapa, const Collision2D& forma, const Vector2D& posicao) {
    const AABB caixa = forma.bounds(posicao);

    int colInicio = 0;
    int rowInicio = 0;
    int colFim = 0;
    int rowFim = 0;
    mapa.toIndices(caixa.min, colInicio, rowInicio);
    mapa.toIndices(caixa.max - Vector2D(0.01f, 0.01f), colFim, rowFim);

    for (int row = rowInicio; row <= rowFim; ++row) {
        for (int col = colInicio; col <= colFim; ++col) {
            const bool foraDoMapa = col < 0 || col >= mapa.width() || row < 0 || row >= mapa.height();
            if (foraDoMapa || mapa.isWall(col, row)) {
                return true;
            }
        }
    }
    return false;
}

void atualizarJogador(Jogador& jogador, const TileMap& mapa) {
    jogador.corpo.velocity = lerDirecao() * VELOCIDADE_JOGADOR;

    const Vector2D antes = jogador.corpo.position;
    jogador.corpo.integrate(DT);
    const Vector2D depois = jogador.corpo.position;

    Vector2D posicao(depois.x, antes.y);
    if (colideComParede(mapa, jogador.forma, posicao)) {
        posicao.x = antes.x;
        jogador.corpo.velocity.x = 0.0f;
    }

    posicao.y = depois.y;
    if (colideComParede(mapa, jogador.forma, posicao)) {
        posicao.y = antes.y;
        jogador.corpo.velocity.y = 0.0f;
    }

    jogador.corpo.position = posicao;
}

void desenharMapa(sf::RenderWindow& janela, const TileMap& mapa) {
    sf::RectangleShape tile(sf::Vector2f(TILE_SIZE, TILE_SIZE));

    for (int row = 0; row < mapa.height(); ++row) {
        for (int col = 0; col < mapa.width(); ++col) {
            if (mapa.isWall(col, row)) {
                tile.setFillColor(sf::Color(70, 70, 90));
            }
            else {
                tile.setFillColor(sf::Color(30, 30, 35));
            }
            tile.setPosition(col * TILE_SIZE, row * TILE_SIZE);
            janela.draw(tile);
        }
    }
}

void desenharJogador(sf::RenderWindow& janela, const Jogador& jogador) {
    const Vector2D meio = jogador.forma.halfExtents;
    sf::RectangleShape quadrado(sf::Vector2f(meio.x * 2.0f, meio.y * 2.0f));
    quadrado.setOrigin(meio.x, meio.y);
    quadrado.setPosition(jogador.corpo.position.x, jogador.corpo.position.y);
    quadrado.setFillColor(sf::Color(60, 140, 230));
    janela.draw(quadrado);
}

int main() {
    sf::RenderWindow janela(sf::VideoMode(1000, 600), "Masmorra");
    janela.setFramerateLimit(60);

    sf::View camera(sf::FloatRect(0.0f, 0.0f, LARGURA_MAPA * TILE_SIZE, ALTURA_MAPA * TILE_SIZE));
    janela.setView(camera);

    unsigned semente = 1;
    TileMap mapa;
    gerarSala(mapa, semente);

    Jogador jogador;
    jogador.forma.halfExtents = Vector2D(24.0f, 24.0f);
    posicionarJogador(jogador, mapa);

    while (janela.isOpen()) {
        sf::Event evento;
        while (janela.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed) {
                janela.close();
            }
            if (evento.type == sf::Event::KeyPressed && evento.key.code == sf::Keyboard::R) {
                semente++;
                gerarSala(mapa, semente);
                posicionarJogador(jogador, mapa);
            }
        }

        if (janela.hasFocus()) {
            atualizarJogador(jogador, mapa);
        }

        janela.clear(sf::Color::Black);
        desenharMapa(janela, mapa);
        desenharJogador(janela, jogador);
        janela.display();
    }
    return 0;
}