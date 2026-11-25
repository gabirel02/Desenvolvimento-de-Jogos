#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow janela(sf::VideoMode(800, 600), "Masmorra");
    janela.setFramerateLimit(60);

    sf::CircleShape circulo(50.0f);
    circulo.setFillColor(sf::Color::Green);
    circulo.setPosition(350.0f, 250.0f);

    while (janela.isOpen()) {
        sf::Event evento;
        while (janela.pollEvent(evento)) {
            if (evento.type == sf::Event::Closed) {
                janela.close();
            }
        }

        janela.clear(sf::Color::Black);
        janela.draw(circulo);
        janela.display();
    }
    return 0;
}