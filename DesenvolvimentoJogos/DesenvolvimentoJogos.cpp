#include <iostream>
#include <cmath>
#include "Vector2D.hpp"
#include "Transform2D.hpp"

// --- Contadores de testes -------------------------------------------------
static int g_total = 0;
static int g_falhas = 0;

// Imprime um vetor no formato (x, y)
void imprimir(const Vector2D& v) {
    std::cout << "(" << v.x << ", " << v.y << ")";
}

// Compara dois vetores (usando o equals) e mostra OK ou ERRO
void testar_vetor(const char* nome, const Vector2D& obtido, const Vector2D& esperado) {
    ++g_total;
    const bool ok = obtido.equals(esperado);
    if (!ok) ++g_falhas;

    std::cout << (ok ? "[OK]   " : "[ERRO] ") << nome << " -> obtido ";
    imprimir(obtido);
    std::cout << ", esperado ";
    imprimir(esperado);
    std::cout << "\n";
}

// Compara dois numeros float com tolerancia e mostra OK ou ERRO
void testar_float(const char* nome, float obtido, float esperado) {
    ++g_total;
    const bool ok = std::abs(obtido - esperado) < EPSILON;
    if (!ok) ++g_falhas;

    std::cout << (ok ? "[OK]   " : "[ERRO] ") << nome
        << " -> obtido " << obtido << ", esperado " << esperado << "\n";
}

// Compara dois valores bool e mostra OK ou ERRO
void testar_bool(const char* nome, bool obtido, bool esperado) {
    ++g_total;
    const bool ok = (obtido == esperado);
    if (!ok) ++g_falhas;

    std::cout << (ok ? "[OK]   " : "[ERRO] ") << nome
        << " -> obtido " << (obtido ? "verdadeiro" : "falso")
        << ", esperado " << (esperado ? "verdadeiro" : "falso") << "\n";
}

int main() {
    std::cout << "=== Testes do Vector2D ===\n\n";

    // --- equals ----------------------------------------------------------
    // Testado primeiro, porque os testes de vetor abaixo dependem dele.
    std::cout << "-- equals --\n";
    testar_bool("equals: (1, 1) e (1.00001, 1) sao iguais",
        Vector2D(1.0f, 1.0f).equals(Vector2D(1.00001f, 1.0f)), true);
    testar_bool("equals: (1, 1) e (1.1, 1) sao diferentes",
        Vector2D(1.0f, 1.0f).equals(Vector2D(1.1f, 1.0f)), false);
    testar_bool("equals: (1, 1) e (1, 1.1) sao diferentes",
        Vector2D(1.0f, 1.0f).equals(Vector2D(1.0f, 1.1f)), false);
    testar_bool("equals: tolerancia 0.1 aceita (1.05, 1)",
        Vector2D(1.0f, 1.0f).equals(Vector2D(1.05f, 1.0f), 0.1f), true);

    // --- Funcoes que ja vinham prontas no header -------------------------
    std::cout << "\n-- funcoes do header --\n";
    testar_float("length_squared de (3, 4)", Vector2D(3.0f, 4.0f).length_squared(), 25.0f);
    testar_float("length de (3, 4)", Vector2D(3.0f, 4.0f).length(), 5.0f);
    testar_float("dot de (1, 2) e (3, 4)", Vector2D(1.0f, 2.0f).dot(Vector2D(3.0f, 4.0f)), 11.0f);
    testar_float("cross de (1, 0) e (0, 1)", Vector2D(1.0f, 0.0f).cross(Vector2D(0.0f, 1.0f)), 1.0f);

    // --- Operadores aritmeticos (criam um vetor novo) --------------------
    std::cout << "\n-- operadores aritmeticos --\n";
    testar_vetor("operator+  (1, 2) + (3, 4)",
        Vector2D(1.0f, 2.0f) + Vector2D(3.0f, 4.0f), Vector2D(4.0f, 6.0f));
    testar_vetor("operator-  (5, 7) - (2, 3)",
        Vector2D(5.0f, 7.0f) - Vector2D(2.0f, 3.0f), Vector2D(3.0f, 4.0f));
    testar_vetor("operator*  (2, 3) * 2",
        Vector2D(2.0f, 3.0f) * 2.0f, Vector2D(4.0f, 6.0f));
    testar_vetor("operator*  2 * (2, 3)  [funcao livre]",
        2.0f * Vector2D(2.0f, 3.0f), Vector2D(4.0f, 6.0f));
    testar_vetor("operator/  (4, 6) / 2",
        Vector2D(4.0f, 6.0f) / 2.0f, Vector2D(2.0f, 3.0f));

    // Os operadores aritmeticos NAO podem alterar o vetor original
    const Vector2D intacto(1.0f, 2.0f);
    const Vector2D soma = intacto + Vector2D(3.0f, 4.0f);
    (void)soma; // evita aviso de variavel nao usada
    testar_vetor("operator+  nao altera o original", intacto, Vector2D(1.0f, 2.0f));

    // --- Operadores compostos (alteram o proprio vetor) ------------------
    std::cout << "\n-- operadores compostos --\n";

    Vector2D a(1.0f, 1.0f);
    Vector2D& ref = (a += Vector2D(2.0f, 3.0f));
    testar_vetor("operator+= (1, 1) += (2, 3)", a, Vector2D(3.0f, 4.0f));
    // Confere se a funcao retornou o PROPRIO vetor (return *this),
    // comparando os enderecos de memoria
    testar_bool("operator+= retorna o proprio vetor", &ref == &a, true);

    Vector2D b(5.0f, 5.0f);
    b -= Vector2D(2.0f, 1.0f);
    testar_vetor("operator-= (5, 5) -= (2, 1)", b, Vector2D(3.0f, 4.0f));

    Vector2D c(1.0f, 2.0f);
    c *= 3.0f;
    testar_vetor("operator*= (1, 2) *= 3", c, Vector2D(3.0f, 6.0f));

    Vector2D d(4.0f, 6.0f);
    d /= 2.0f;
    testar_vetor("operator/= (4, 6) /= 2", d, Vector2D(2.0f, 3.0f));

    // --- Normalizacao ----------------------------------------------------
    std::cout << "\n-- normalizacao --\n";

    const Vector2D original(3.0f, 4.0f);
    const Vector2D n = original.normalized();
    testar_vetor("normalized de (3, 4)", n, Vector2D(0.6f, 0.8f));
    testar_float("normalized: comprimento do resultado", n.length(), 1.0f);
    testar_vetor("normalized: original nao muda", original, Vector2D(3.0f, 4.0f));
    testar_vetor("normalized de (10, 0)", Vector2D(10.0f, 0.0f).normalized(), Vector2D(1.0f, 0.0f));
    testar_vetor("normalized de (0, -2)", Vector2D(0.0f, -2.0f).normalized(), Vector2D(0.0f, -1.0f));

    Vector2D e(3.0f, 4.0f);
    e.normalize();
    testar_vetor("normalize (3, 4) altera o proprio vetor", e, Vector2D(0.6f, 0.8f));

    // --- Pre-condicoes (assert) ------------------------------------------
    // Cada linha abaixo deve PARAR o programa com uma mensagem de erro,
    // mostrando que o assert esta funcionando. Descomente UMA por vez,
    // rode com Ctrl+F5, veja o erro e comente de novo.
    //
    // Vector2D z1 = Vector2D(1.0f, 1.0f) / 0.0f;
    // Vector2D z2 = Vector2D(0.0f, 0.0f).normalized();
    // Vector2D z3(1.0f, 1.0f); z3 /= 0.0f;
    // Vector2D z4(0.0f, 0.0f); z4.normalize();

    // =====================================================================
    std::cout << "\n=== Testes do Transform2D ===\n";

    // --- Identidade ------------------------------------------------------
    std::cout << "\n-- identidade --\n";
    const Transform2D identidade; // construtor padrao: nao muda nada
    testar_vetor("identidade: ponto (3, 4)",
        identidade.transform_point(Vector2D(3.0f, 4.0f)), Vector2D(3.0f, 4.0f));
    testar_vetor("identidade: direcao (3, 4)",
        identidade.transform_vector(Vector2D(3.0f, 4.0f)), Vector2D(3.0f, 4.0f));

    // --- Translacao ------------------------------------------------------
    std::cout << "\n-- translacao --\n";
    const Transform2D t = Transform2D::translation(5.0f, 3.0f);
    testar_vetor("translation(5, 3): ponto (1, 1) se move",
        t.transform_point(Vector2D(1.0f, 1.0f)), Vector2D(6.0f, 4.0f));
    testar_vetor("translation(5, 3): direcao (1, 1) NAO se move",
        t.transform_vector(Vector2D(1.0f, 1.0f)), Vector2D(1.0f, 1.0f));
    testar_vetor("translation(-2, 0): ponto (0, 0)",
        Transform2D::translation(-2.0f, 0.0f).transform_point(Vector2D(0.0f, 0.0f)),
        Vector2D(-2.0f, 0.0f));

    // --- Escala ----------------------------------------------------------
    std::cout << "\n-- escala --\n";
    const Transform2D s = Transform2D::scale(2.0f, 3.0f);
    testar_vetor("scale(2, 3): ponto (1, 1)",
        s.transform_point(Vector2D(1.0f, 1.0f)), Vector2D(2.0f, 3.0f));
    testar_vetor("scale(2, 3): direcao (1, 1) tambem escala",
        s.transform_vector(Vector2D(1.0f, 1.0f)), Vector2D(2.0f, 3.0f));
    testar_vetor("scale(2, 3): ponto (0, 0) fica na origem",
        s.transform_point(Vector2D(0.0f, 0.0f)), Vector2D(0.0f, 0.0f));

    // --- Rotacao ---------------------------------------------------------
    std::cout << "\n-- rotacao --\n";
    const float PI = std::acos(-1.0f); // 180 graus em radianos

    testar_vetor("rotation(0): ponto (1, 0) nao muda",
        Transform2D::rotation(0.0f).transform_point(Vector2D(1.0f, 0.0f)),
        Vector2D(1.0f, 0.0f));
    testar_vetor("rotation(90 graus): ponto (1, 0) vai para (0, 1)",
        Transform2D::rotation(PI / 2.0f).transform_point(Vector2D(1.0f, 0.0f)),
        Vector2D(0.0f, 1.0f));
    testar_vetor("rotation(90 graus): ponto (0, 1) vai para (-1, 0)",
        Transform2D::rotation(PI / 2.0f).transform_point(Vector2D(0.0f, 1.0f)),
        Vector2D(-1.0f, 0.0f));
    testar_vetor("rotation(180 graus): ponto (1, 0) vai para (-1, 0)",
        Transform2D::rotation(PI).transform_point(Vector2D(1.0f, 0.0f)),
        Vector2D(-1.0f, 0.0f));
    testar_vetor("rotation(90 graus): direcao (1, 0) tambem gira",
        Transform2D::rotation(PI / 2.0f).transform_vector(Vector2D(1.0f, 0.0f)),
        Vector2D(0.0f, 1.0f));
    testar_float("rotation(45 graus): comprimento de (3, 4) se mantem",
        Transform2D::rotation(PI / 4.0f).transform_point(Vector2D(3.0f, 4.0f)).length(),
        5.0f);

    // --- Composicao (operator* e operator*=) ------------------------------
    std::cout << "\n-- composicao --\n";
    const Transform2D S = Transform2D::scale(2.0f, 2.0f);
    const Transform2D T = Transform2D::translation(5.0f, 0.0f);

    // A ordem importa: A * B aplica A primeiro, depois B
    testar_vetor("scale * translation: (1, 0) dobra e depois anda 5",
        (S * T).transform_point(Vector2D(1.0f, 0.0f)), Vector2D(7.0f, 0.0f));
    testar_vetor("translation * scale: (1, 0) anda 5 e depois dobra",
        (T * S).transform_point(Vector2D(1.0f, 0.0f)), Vector2D(12.0f, 0.0f));

    // Aplicar separado deve dar o mesmo que aplicar a matriz combinada
    const Vector2D p(3.0f, -2.0f);
    testar_vetor("aplicar S e depois T separadamente = aplicar S * T",
        T.transform_point(S.transform_point(p)), (S * T).transform_point(p));

    // A identidade nao muda nada na composicao
    testar_vetor("identidade * translation = translation",
        (identidade * t).transform_point(Vector2D(1.0f, 1.0f)), Vector2D(6.0f, 4.0f));
    testar_vetor("translation * identidade = translation",
        (t * identidade).transform_point(Vector2D(1.0f, 1.0f)), Vector2D(6.0f, 4.0f));

    // Duas rotacoes de 90 graus = uma rotacao de 180 graus
    const Transform2D r90 = Transform2D::rotation(PI / 2.0f);
    testar_vetor("rotation(90) * rotation(90): (1, 0) vai para (-1, 0)",
        (r90 * r90).transform_point(Vector2D(1.0f, 0.0f)), Vector2D(-1.0f, 0.0f));

    // operator*= deve dar o mesmo resultado que operator*
    Transform2D acumulada = S;
    Transform2D& ref_t = (acumulada *= T);
    testar_vetor("operator*=: S *= T leva (1, 0) a (7, 0)",
        acumulada.transform_point(Vector2D(1.0f, 0.0f)), Vector2D(7.0f, 0.0f));
    testar_bool("operator*= retorna a propria transformacao", &ref_t == &acumulada, true);

    // --- Resumo ----------------------------------------------------------
    std::cout << "\n=== Resultado: " << (g_total - g_falhas) << " de " << g_total
        << " testes passaram ===\n";

    return g_falhas == 0 ? 0 : 1;
}

// Executar programa: Ctrl + F5 ou Menu Depurar > Iniciar Sem Depuração
// Depurar programa: F5 ou menu Depurar > Iniciar Depuração

// Dicas para Começar: 
//   1. Use a janela do Gerenciador de Soluções para adicionar/gerenciar arquivos
//   2. Use a janela do Team Explorer para conectar-se ao controle do código-fonte
//   3. Use a janela de Saída para ver mensagens de saída do build e outras mensagens
//   4. Use a janela Lista de Erros para exibir erros
//   5. Ir Para o Projeto > Adicionar Novo Item para criar novos arquivos de código, ou Projeto > Adicionar Item Existente para adicionar arquivos de código existentes ao projeto
//   6. No futuro, para abrir este projeto novamente, vá para Arquivo > Abrir > Projeto e selecione o arquivo. sln
