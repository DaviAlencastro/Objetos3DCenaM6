# Objetos 3D - Módulo 6 — Trajetórias com Curvas de Bézier

Atividade acadêmica de Computação Gráfica — visualizador 3D com iluminação Phong, texturas via OBJ/MTL, câmera em primeira pessoa e trajetórias cíclicas por curvas de Bézier cúbicas (De Casteljau).

## Setup — Compilação e execução

Requisitos: **CMake 3.10+**, compilador **C++17** (MSVC recomendado), **Git**.

```powershell
# Na pasta raiz do projeto (Objetos3DCenaM2/)
mkdir build
cd build
cmake ..
cmake --build . --config Debug
cp -r ..\assets .\Debug\assets
cd Debug
.\Cubo.exe
```

> GLFW, GLM e stb_image são obtidos automaticamente pelo CMake na primeira compilação.

A cena é configurada pelo arquivo `assets/scene.txt`:
```
# formato: obj <arquivo_obj> <x> <y> <z> <escala>
obj assets/modelo.obj -0.65 0.0 0.0 0.2
```

## Controles

### Câmera
| Tecla / Input | Ação |
|---|---|
| `W` / `S` / `A` / `D` | Move câmera frente / trás / esquerda / direita |
| Mouse | Orienta câmera (yaw/pitch) |
| Scroll | Zoom (FOV entre 1° e 45°) |

### Seleção e modos de transformação
| Tecla | Ação |
|---|---|
| `TAB` | Alterna objeto selecionado: 0 → 1 → 2 → TODOS → 0 |
| `R` | Ativa modo **Rotação** |
| `T` | Ativa modo **Translação** |
| `P` | Ativa modo **Escala** (`S` reservado para câmera) |

### Transformações (conforme modo ativo)
| Tecla | Rotate | Translate | Scale |
|---|---|---|---|
| `←` / `→` | Eixo Y | Eixo X | — |
| `↑` / `↓` | Eixo X | Eixo Y | Aumenta / Diminui |
| `X` / `Y` / `Z` | Eixo correspondente | — | — |
| `+` / `-` | — | — | Aumenta / Diminui |

### Trajetórias (Bézier cúbica)
| Tecla | Ação |
|---|---|
| `C` | Adiciona waypoint na posição atual do objeto selecionado |
| `G` | Inicia / pausa animação (mínimo 2 pontos; ≥4 usa Bézier cúbica) |
| `U` | Remove todos os waypoints do objeto selecionado |

**Como usar:** modo `T` → mova com setas → `C` (repita para cada ponto) → `G` para animar.

### Luzes
| Tecla | Ação |
|---|---|
| `1` | Liga/desliga luz principal (key light) |
| `2` | Liga/desliga luz de preenchimento (fill light) |
| `3` | Liga/desliga luz de fundo (back light) |

`ESC` — fecha a janela

## Assets

| Asset | Procedência |
|---|---|
| `modelo.obj` | [Repositório de exemplos da disciplina](https://github.com/fellowsheep/FCG2025-1) |
| `texture.png` | [Repositório de exemplos da disciplina](https://github.com/fellowsheep/FCG2025-1) |

## Referências

- **OpenGL:** [learnopengl.com](https://learnopengl.com) — iluminação Phong, mapeamento de textura, câmera FPS
- **GLFW:** [glfw.org/docs](https://www.glfw.org/docs/latest/)
- **GLM:** [glm.g-truc.net](https://glm.g-truc.net/)
- **stb_image:** [github.com/nothings/stb](https://github.com/nothings/stb)
- **Curvas de Bézier (De Casteljau):** slides da disciplina — Módulo 6, Rossana B. Queiroz
- **Formato OBJ/MTL:** [paulbourke.net/dataformats/obj](http://paulbourke.net/dataformats/obj/)
