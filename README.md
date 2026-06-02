# Objetos 3D - Módulo 5

Atividade acadêmica de Computação Gráfica — câmera em primeira pessoa com classe Camera, iluminação Phong com técnica de 3 luzes pontuais e texturas via OBJ/MTL.

## Dependências

- CMake 3.10+
- Compilador C++17 (MSVC recomendado)
- Git (para o FetchContent baixar GLFW e GLM automaticamente)
- `stb_image.h` — baixado automaticamente pelo CMake

## Como compilar e executar

```powershell
cd build
cmake ..
cmake --build . --config Debug
cd Debug
.\Cubo.exe
```

> **Atenção:** se a textura aparecer errada após trocar o arquivo `assets/texture.png`,
> copie-a manualmente para a pasta do executável:
> ```powershell
> cp ..\assets\texture.png assets\texture.png
> ```

## Controles

### Câmera
| Tecla / Input | Ação |
|---|---|
| `W` / `S` / `A` / `D` | Move câmera frente / trás / esquerda / direita |
| Mouse | Orienta câmera (yaw/pitch) |
| Scroll | Zoom (altera FOV entre 1° e 45°) |

### Seleção e modos de transformação
| Tecla | Ação |
|---|---|
| `TAB` | Alterna objeto selecionado: 0 → 1 → 2 → TODOS → 0 |
| `R` | Ativa modo **Rotação** |
| `T` | Ativa modo **Translação** |
| `P` | Ativa modo **Escala** (`S` reservado para câmera) |

### Modo Rotação (`R`)
| Tecla | Ação |
|---|---|
| `←` / `→` | Rotaciona no eixo Y |
| `↑` / `↓` | Rotaciona no eixo X |
| `X` / `Y` / `Z` | Rotaciona no eixo correspondente |

### Modo Translação (`T`)
| Tecla | Ação |
|---|---|
| `←` / `→` | Move em X |
| `↑` / `↓` | Move em Y |

### Modo Escala (`P`)
| Tecla | Ação |
|---|---|
| `↑` / `+` | Aumenta escala |
| `↓` / `-` | Diminui escala |

### Luzes
| Tecla | Ação |
|---|---|
| `1` | Liga/desliga luz principal (key) |
| `2` | Liga/desliga luz de preenchimento (fill) |
| `3` | Liga/desliga luz de fundo (back) |

`ESC` — fecha a janela

O modo ativo, objeto selecionado e estado das luzes são exibidos no título da janela.

## Tecnologias

- OpenGL 4.5 + GLSL 450
- GLFW 3.4 — janela e entrada
- GLM — matemática 3D
- GLAD — loader de funções OpenGL
- stb_image — carregamento de texturas
