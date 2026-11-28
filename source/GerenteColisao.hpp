#ifndef GAME_GERENTECOLISAO_HPP
#define GAME_GERENTECOLISAO_HPP

#include <vector>
#include "Global.hpp"
#include "Vector2D.hpp"
#include "Collision2D.hpp"

/// Um par de entidades que estao se tocando.
struct ParColisao {
    EntityId a;
    EntityId b;
};

/**
 * Guarda a lista de tudo que existe no jogo (cada coisa com um identificador)
 * e devolve os pares que colidiram.
 */
class GerenteColisao {
public:
    /// Devolve um identificador novo, a partir de INITIAL_ENTITY_ID.
    [[nodiscard]] EntityId novoId();

    /// Coloca uma entidade na lista, com a sua forma e a sua posicao atual.
    void registrar(EntityId id, const Collision2D& forma, const Vector2D& posicao);

    /// Esvazia a lista (os identificadores ja entregues continuam valendo).
    void limpar();

    /// Testa todos os pares da lista e devolve os que estao se tocando.
    [[nodiscard]] std::vector<ParColisao> verificar() const;

private:
    struct Registro {
        EntityId id;
        AABB caixa;
    };

    std::vector<Registro> m_registros;
    EntityId m_proximoId{ INITIAL_ENTITY_ID };
};

#endif // GAME_GERENTECOLISAO_HPP