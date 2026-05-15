/* Cubo 3D - Atividade Acadêmica Computação Gráfica - Módulo 3
 *
 * Controles:
 *   TAB       - alterna objeto selecionado: 0 -> 1 -> 2 -> TODOS -> 0 -> ...
 *   R         - ativa modo Rotate  -> X/Y/Z rotacionam no eixo respectivo
 *   T         - ativa modo Translate -> W/A/D/Seta↑↓←→ transladam
 *   S         - ativa modo Scale   -> +/- escala uniforme
 *   ESC       - fecha a janela
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
GLuint loadSimpleOBJ(const string& filePATH, int& nVertices, glm::vec3 color);
int setupShader();

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
    GLuint VAO;
    int nVertices;
    glm::vec3 position;
    float scale;
    float rotAngleX, rotAngleY, rotAngleZ;

    Cube(GLuint vao, int nv, glm::vec3 pos, float s = 0.3f)
        : VAO(vao), nVertices(nv), position(pos), scale(s),
          rotAngleX(0.0f), rotAngleY(0.0f), rotAngleZ(0.0f) {}
};

vector<Cube> cubes;

// 0, 1, 2 = cubo específico; 3 = todos os cubos
int selectionMode = 0;

enum Mode { ROTATE, TRANSLATE, SCALE };
Mode currentMode = TRANSLATE;

const float TRANSLATE_STEP = 0.1f;
const float SCALE_STEP     = 0.05f;
const float SCALE_MIN      = 0.05f;
const float ROT_STEP       = glm::radians(5.0f);

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

    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    GLuint shaderID = setupShader();
    glUseProgram(shaderID);
    GLint modelLoc = glGetUniformLocation(shaderID, "model");

    glEnable(GL_DEPTH_TEST);

    // Carregar 3 instâncias de Suzanne com cores distintas (RGB)
    int nv;
    GLuint vao0 = loadSimpleOBJ("assets/modelo.obj", nv, glm::vec3(0.9f, 0.2f, 0.2f));
    cubes.push_back(Cube(vao0, nv, glm::vec3(-0.65f, 0.0f, 0.0f), 0.2f));

    GLuint vao1 = loadSimpleOBJ("assets/modelo.obj", nv, glm::vec3(0.2f, 0.85f, 0.2f));
    cubes.push_back(Cube(vao1, nv, glm::vec3( 0.00f, 0.0f, 0.0f), 0.2f));

    GLuint vao2 = loadSimpleOBJ("assets/modelo.obj", nv, glm::vec3(0.2f, 0.4f, 1.0f));
    cubes.push_back(Cube(vao2, nv, glm::vec3( 0.65f, 0.0f, 0.0f), 0.2f));

    if (vao0 == (GLuint)-1 || vao1 == (GLuint)-1 || vao2 == (GLuint)-1) {
        cerr << "Falha ao carregar modelos OBJ." << endl;
        glfwTerminate();
        return -1;
    }

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        string sel = (selectionMode < (int)cubes.size())
                     ? "Cubo " + to_string(selectionMode)
                     : "TODOS";
        string modeName = (currentMode == ROTATE) ? "ROTATE"
                        : (currentMode == TRANSLATE) ? "TRANSLATE"
                        : "SCALE";
        string title = "Cubo 3D  |  [" + sel + "]  [" + modeName + "]"
                     + "  |  R/T/S=modo  TAB=selecionar"
                     + (currentMode == ROTATE    ? "  setas=rotacionar XYZ=eixo" :
                        currentMode == TRANSLATE ? "  setas=mover" :
                                                   "  setas/+/-=escala");
        glfwSetWindowTitle(window, title.c_str());

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for (int i = 0; i < (int)cubes.size(); i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubes[i].position);
            model = glm::rotate(model, cubes[i].rotAngleX, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, cubes[i].rotAngleY, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, cubes[i].rotAngleZ, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(cubes[i].scale));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glBindVertexArray(cubes[i].VAO);
            glDrawArrays(GL_TRIANGLES, 0, cubes[i].nVertices);
        }
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int modeFlag)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    // Ciclo de seleção: Cubo 0 -> 1 -> 2 -> TODOS -> 0
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
        selectionMode = (selectionMode + 1) % ((int)cubes.size() + 1);

    // Alternar modo com R / T / S
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_R) currentMode = ROTATE;
        if (key == GLFW_KEY_T) currentMode = TRANSLATE;
        if (key == GLFW_KEY_S) currentMode = SCALE;
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (currentMode == TRANSLATE) {
            if (key == GLFW_KEY_LEFT)
                applyToSelected([](Cube& c){ c.position.x -= TRANSLATE_STEP; });
            if (key == GLFW_KEY_RIGHT)
                applyToSelected([](Cube& c){ c.position.x += TRANSLATE_STEP; });
            if (key == GLFW_KEY_UP)
                applyToSelected([](Cube& c){ c.position.y += TRANSLATE_STEP; });
            if (key == GLFW_KEY_DOWN)
                applyToSelected([](Cube& c){ c.position.y -= TRANSLATE_STEP; });
        }

        if (currentMode == SCALE) {
            if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_UP)
                applyToSelected([](Cube& c){ c.scale += SCALE_STEP; });
            if (key == GLFW_KEY_MINUS || key == GLFW_KEY_DOWN)
                applyToSelected([](Cube& c){
                    c.scale -= SCALE_STEP;
                    if (c.scale < SCALE_MIN) c.scale = SCALE_MIN;
                });
        }

        if (currentMode == ROTATE) {
            if (key == GLFW_KEY_LEFT)
                applyToSelected([](Cube& c){ c.rotAngleY -= ROT_STEP; });
            if (key == GLFW_KEY_RIGHT)
                applyToSelected([](Cube& c){ c.rotAngleY += ROT_STEP; });
            if (key == GLFW_KEY_UP)
                applyToSelected([](Cube& c){ c.rotAngleX -= ROT_STEP; });
            if (key == GLFW_KEY_DOWN)
                applyToSelected([](Cube& c){ c.rotAngleX += ROT_STEP; });
            if (key == GLFW_KEY_X)
                applyToSelected([](Cube& c){ c.rotAngleX += ROT_STEP; });
            if (key == GLFW_KEY_Y)
                applyToSelected([](Cube& c){ c.rotAngleY += ROT_STEP; });
            if (key == GLFW_KEY_Z)
                applyToSelected([](Cube& c){ c.rotAngleZ += ROT_STEP; });
        }
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

GLuint loadSimpleOBJ(const string& filePATH, int& nVertices, glm::vec3 color)
{
    vector<glm::vec3> vertices;
    vector<glm::vec2> texCoords;
    vector<glm::vec3> normals;
    vector<GLfloat> vBuffer;

    ifstream arqEntrada(filePATH);
    if (!arqEntrada.is_open()) {
        cerr << "Erro ao abrir o arquivo " << filePATH << endl;
        return (GLuint)-1;
    }

    string line;
    while (getline(arqEntrada, line)) {
        istringstream ssline(line);
        string word;
        ssline >> word;

        if (word == "v") {
            glm::vec3 v;
            ssline >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        } else if (word == "vt") {
            glm::vec2 vt;
            ssline >> vt.s >> vt.t;
            texCoords.push_back(vt);
        } else if (word == "vn") {
            glm::vec3 vn;
            ssline >> vn.x >> vn.y >> vn.z;
            normals.push_back(vn);
        } else if (word == "f") {
            while (ssline >> word) {
                int vi = 0, ti = 0, ni = 0;
                istringstream ss(word);
                string index;
                if (getline(ss, index, '/')) vi = !index.empty() ? stoi(index) - 1 : 0;
                if (getline(ss, index, '/')) ti = !index.empty() ? stoi(index) - 1 : 0;
                if (getline(ss, index))      ni = !index.empty() ? stoi(index) - 1 : 0;

                vBuffer.push_back(vertices[vi].x);
                vBuffer.push_back(vertices[vi].y);
                vBuffer.push_back(vertices[vi].z);
                vBuffer.push_back(color.r);
                vBuffer.push_back(color.g);
                vBuffer.push_back(color.b);
            }
        }
    }
    arqEntrada.close();

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vBuffer.size() * sizeof(GLfloat), vBuffer.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    nVertices = (int)(vBuffer.size() / 6);
    return VAO;
}
