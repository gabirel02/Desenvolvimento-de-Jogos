#include "GerenteColisao.hpp"

EntityId GerenteColisao::novoId() {
    return m_proximoId++;
}

void GerenteColisao::registrar(EntityId id, const Collision2D& forma, const Vector2D& posicao) {
    Registro registro;
    registro.id = id;
    registro.caixa = forma.bounds(posicao);
    m_registros.push_back(registro);
}

void GerenteColisao::limpar() {
    m_registros.clear();
}

std::vector<ParColisao> GerenteColisao::verificar() const {
    std::vector<ParColisao> pares;
    const int total = static_cast<int>(m_registros.size());

    for (int i = 0; i < total; ++i) {
        for (int j = i + 1; j < total; ++j) {
            if (m_registros[i].caixa.intersects(m_registros[j].caixa)) {
                pares.push_back({ m_registros[i].id, m_registros[j].id });
            }
        }
    }
    return pares;
}