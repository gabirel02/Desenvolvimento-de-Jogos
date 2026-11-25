#include <SFML/Graphics.hpp>
#include "Dungeon2D.hpp"
#include "TileMap.hpp"

const int LARGURA_MAPA = 25;
const int ALTURA_MAPA = 15;

void gerarSala(TileMap& mapa, unsigned semente) {
    Dungeon2D masmorra(LARGURA_MAPA, ALTURA_MAPA);
    masmorra.generate(semente);
    mapa.loadFromDungeon(masmorra);
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

int main() {
    sf::RenderWindow janela(sf::VideoMode(1000, 600), "Masmorra");
    janela.setFramerateLimit(60);

    sf::View camera(sf::FloatRect(0.0f, 0.0f, LARGURA_MAPA * TILE_SIZE, ALTURA_MAPA * TILE_SIZE));
    janela.setView(camera);

    unsigned semente = 1;
    TileMap mapa;
    gerarSala(mapa, semente);

    while (janela.isOpen()) {
        sf::Event evento;
        while (janela.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed) {
                janela.close();
            }
            if (evento.type == sf::Event::KeyPressed && evento.key.code == sf::Keyboard::R) {
                semente++;
                gerarSala(mapa, semente);
            }
        }

        janela.clear(sf::Color::Black);
        desenharMapa(janela, mapa);
        janela.display();
    }
    return 0;
}