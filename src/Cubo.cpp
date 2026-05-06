/* Cubo 3D - Atividade Acadêmica Computação Gráfica
 *
 * Controles:
 *   W/S       - translação em Y (cima/baixo)
 *   A/D       - translação em X (esquerda/direita)
 *   +/-       - escala uniforme (aumentar/diminuir)
 *   X/Y/Z     - rotação incremental no respectivo eixo (acumula, segure para girar)
 *   TAB       - alterna cubo selecionado: 0 -> 1 -> 2 -> TODOS -> 0 -> ...
 *   ESC       - fecha a janela
 */

#include <iostream>
#include <string>
#include <vector>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
int setupShader();
int setupGeometry();

const GLuint WIDTH = 1000, HEIGHT = 1000;

const GLchar* vertexShaderSource =
    "#version 450\n"
    "layout (location = 0) in vec3 position;\n"
    "layout (location = 1) in vec3 color;\n"
    "uniform mat4 model;\n"
    "out vec4 finalColor;\n"
    "void main()\n"
    "{\n"
    "    gl_Position = model * vec4(position, 1.0);\n"
    "    finalColor = vec4(color, 1.0);\n"
    "}\0";

const GLchar* fragmentShaderSource =
    "#version 450\n"
    "in vec4 finalColor;\n"
    "out vec4 color;\n"
    "void main()\n"
    "{\n"
    "    color = finalColor;\n"
    "}\n\0";

struct Cube {
    glm::vec3 position;
    float scale;
    float rotAngleX, rotAngleY, rotAngleZ;

    Cube(glm::vec3 pos, float s = 0.3f)
        : position(pos), scale(s),
          rotAngleX(0.0f), rotAngleY(0.0f), rotAngleZ(0.0f) {}
};

vector<Cube> cubes;

// 0, 1, 2 = cubo específico; 3 = todos os cubos
int selectionMode = 0;

const float TRANSLATE_STEP = 0.1f;
const float SCALE_STEP     = 0.05f;
const float SCALE_MIN      = 0.05f;
const float ROT_STEP       = glm::radians(5.0f);

// Aplica uma função ao cubo selecionado ou a todos se selectionMode == cubes.size()
template<typename Fn>
void applyToSelected(Fn fn) {
    if (selectionMode < (int)cubes.size())
        fn(cubes[selectionMode]);
    else
        for (auto& c : cubes) fn(c);
}

int main()
{
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Cubo 3D", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Falha ao inicializar GLAD" << endl;
        return -1;
    }

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version  = glGetString(GL_VERSION);
    cout << "Renderer: " << renderer << endl;
    cout << "OpenGL version: " << version << endl;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    GLuint shaderID = setupShader();
    GLuint VAO      = setupGeometry();

    glUseProgram(shaderID);
    GLint modelLoc = glGetUniformLocation(shaderID, "model");

    glEnable(GL_DEPTH_TEST);

    // Três cubos lado a lado em X
    cubes.push_back(Cube(glm::vec3(-0.65f, 0.0f, 0.0f)));
    cubes.push_back(Cube(glm::vec3( 0.00f, 0.0f, 0.0f)));
    cubes.push_back(Cube(glm::vec3( 0.65f, 0.0f, 0.0f)));

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Título mostra qual cubo está selecionado e lista os controles
        string sel = (selectionMode < (int)cubes.size())
                     ? "Cubo " + to_string(selectionMode)
                     : "TODOS";
        string title = "Cubo 3D  |  Selecionado: " + sel
                     + "  |  TAB=alternar WASD=mover +/-=escala XYZ=rotacao";
        glfwSetWindowTitle(window, title.c_str());

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        for (int i = 0; i < (int)cubes.size(); i++)
        {
            glm::mat4 model = glm::mat4(1.0f);

            // Ordem TRS: primeiro escala, depois rotação, depois translação
            model = glm::translate(model, cubes[i].position);

            // Rotação acumulada nos três eixos independentemente
            model = glm::rotate(model, cubes[i].rotAngleX, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, cubes[i].rotAngleY, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, cubes[i].rotAngleZ, glm::vec3(0.0f, 0.0f, 1.0f));

            model = glm::scale(model, glm::vec3(cubes[i].scale));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // Ciclo: Cubo 0 -> Cubo 1 -> Cubo 2 -> TODOS -> Cubo 0 -> ...
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
        selectionMode = (selectionMode + 1) % ((int)cubes.size() + 1);

    // Translação e escala respondem a PRESS e REPEAT (segurar a tecla)
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_A)
            applyToSelected([](Cube& c){ c.position.x -= TRANSLATE_STEP; });
        if (key == GLFW_KEY_D)
            applyToSelected([](Cube& c){ c.position.x += TRANSLATE_STEP; });
        if (key == GLFW_KEY_W)
            applyToSelected([](Cube& c){ c.position.y += TRANSLATE_STEP; });
        if (key == GLFW_KEY_S)
            applyToSelected([](Cube& c){ c.position.y -= TRANSLATE_STEP; });

        if (key == GLFW_KEY_EQUAL)
            applyToSelected([](Cube& c){ c.scale += SCALE_STEP; });
        if (key == GLFW_KEY_MINUS)
            applyToSelected([](Cube& c){
                c.scale -= SCALE_STEP;
                if (c.scale < SCALE_MIN) c.scale = SCALE_MIN;
            });
    }

    // Rotação incremental: acumula ângulo, responde a PRESS e REPEAT
    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (key == GLFW_KEY_X)
            applyToSelected([](Cube& c){ c.rotAngleX += ROT_STEP; });
        if (key == GLFW_KEY_Y)
            applyToSelected([](Cube& c){ c.rotAngleY += ROT_STEP; });
        if (key == GLFW_KEY_Z)
            applyToSelected([](Cube& c){ c.rotAngleZ += ROT_STEP; });
    }
}

int setupShader()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cout << "ERRO::VERTEX_SHADER: " << infoLog << endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cout << "ERRO::FRAGMENT_SHADER: " << infoLog << endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cout << "ERRO::SHADER_PROGRAM: " << infoLog << endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

int setupGeometry()
{
    // 6 faces × 2 triângulos × 3 vértices = 36 vértices
    // Formato por vértice: x, y, z, r, g, b
    GLfloat vertices[] = {

        // Face frontal (+Z) — Vermelho
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,

        // Face traseira (-Z) — Verde
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,

        // Face esquerda (-X) — Azul
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,

        // Face direita (+X) — Amarelo
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,

        // Face superior (+Y) — Ciano
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,

        // Face inferior (-Y) — Magenta
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
    };

    GLuint VBO, VAO;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Atributo 0: posição (x, y, z)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Atributo 1: cor (r, g, b)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}
