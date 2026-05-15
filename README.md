# Objetos 3D - Módulo 3

Atividade acadêmica de Computação Gráfica — seleção e transformação de múltiplos objetos 3D com OpenGL.

## Dependências

- CMake 3.10+
- Compilador C++17 (MinGW/MSVC)
- Git (para o FetchContent baixar GLFW e GLM automaticamente)

## Como compilar e executar

```bash
mkdir build
cd build
cmake ..
cmake --build .
.\Cubo.exe
```

## Controles

| Tecla | Ação |
|-------|------|
| `TAB` | Alterna o objeto selecionado: Cubo 0 → 1 → 2 → TODOS → 0 |
| `R` | Ativa modo **Rotação** |
| `T` | Ativa modo **Translação** |
| `S` | Ativa modo **Escala** |

### Modo Rotação (`R`)
| Tecla | Ação |
|-------|------|
| `←` / `→` | Rotaciona no eixo Y |
| `↑` / `↓` | Rotaciona no eixo X |
| `X` | Rotaciona no eixo X |
| `Y` | Rotaciona no eixo Y |
| `Z` | Rotaciona no eixo Z |

### Modo Translação (`T`)
| Tecla | Ação |
|-------|------|
| `←` / `→` | Move em X |
| `↑` / `↓` | Move em Y |

### Modo Escala (`S`)
| Tecla | Ação |
|-------|------|
| `↑` / `+` | Aumenta escala |
| `↓` / `-` | Diminui escala |

`ESC` — fecha a janela

O modo ativo e o objeto selecionado são exibidos no título da janela.

## Tecnologias

- OpenGL 4.5 + GLSL 450
- GLFW 3.4 — janela e entrada
- GLM — matemática 3D
- GLAD — loader de funções OpenGL
