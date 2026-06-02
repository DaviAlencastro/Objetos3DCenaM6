/* Cubo 3D - Atividade Acadêmica Computação Gráfica - Módulo 5
 * Câmera em primeira pessoa com classe Camera.
 * Inclui: View Matrix (lookAt), Projection Matrix (perspective),
 *         movimento WASD com deltaTime, mouse look (yaw/pitch), zoom com scroll.
 *
 * Controles:
 *   TAB       - alterna objeto selecionado: 0 -> 1 -> 2 -> TODOS -> 0
 *   R         - modo Rotate  -> X/Y/Z rotacionam o objeto
 *   T         - modo Translate -> setas transladam o objeto
 *   S (modo)  - modo Scale   -> setas/+/- escalam o objeto
 *   1/2/3     - liga/desliga luz principal / preenchimento / fundo
 *   W/A/S/D   - move câmera (frente/esquerda/trás/direita)
 *   Mouse     - orienta câmera (yaw/pitch)
 *   Scroll    - zoom (altera FOV)
 *   ESC       - fecha a janela
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// ---------------------------------------------------------------------------
// Classe Camera — encapsula posição, orientação, movimento e rotação
// ---------------------------------------------------------------------------
class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;

    float yaw;
    float pitch;
    float fov;
    float speed;
    float sensitivity;

    bool firstMouse;
    float lastX, lastY;

    Camera(float screenW, float screenH,
           glm::vec3 startPos = glm::vec3(0.0f, 0.0f, 3.0f))
        : position(startPos),
          front(glm::vec3(0.0f, 0.0f, -1.0f)),
          up(glm::vec3(0.0f, 1.0f, 0.0f)),
          yaw(-90.0f), pitch(0.0f),
          fov(45.0f), speed(2.5f), sensitivity(0.05f),
          firstMouse(true),
          lastX(screenW / 2.0f), lastY(screenH / 2.0f)
    {
        updateVectors();
    }

    glm::mat4 getViewMatrix() const {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 getProjectionMatrix(float aspect) const {
        return glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);
    }

    // Move a câmera com base nas teclas WASD pressionadas no frame
    void processKeyboard(GLFWwindow* window, float deltaTime) {
        float vel = speed * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            position += front * vel;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            position -= front * vel;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            position -= right * vel;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            position += right * vel;
    }

    // Orienta a câmera a partir do deslocamento do mouse
    void processMouse(double xpos, double ypos) {
        if (firstMouse) {
            lastX = (float)xpos;
            lastY = (float)ypos;
            firstMouse = false;
        }
        float xoffset = ((float)xpos - lastX) * sensitivity;
        float yoffset = (lastY - (float)ypos) * sensitivity; // invertido: y cresce para baixo
        lastX = (float)xpos;
        lastY = (float)ypos;

        yaw   += xoffset;
        pitch += yoffset;
        if (pitch >  89.0f) pitch =  89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        updateVectors();
    }

    // Zoom via scroll — altera o FOV
    void processScroll(double yoffset) {
        fov -= (float)yoffset;
        if (fov <  1.0f) fov =  1.0f;
        if (fov > 45.0f) fov = 45.0f;
    }

private:
    void updateVectors() {
        glm::vec3 f;
        f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        f.y = sin(glm::radians(pitch));
        f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(f);
        right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
        up    = glm::normalize(glm::cross(right, front));
    }
};

// ---------------------------------------------------------------------------
// Structs auxiliares
// ---------------------------------------------------------------------------
struct Material {
    string texFile;
    glm::vec3 ka = {0.2f, 0.2f, 0.2f};
    glm::vec3 kd = {0.8f, 0.8f, 0.8f};
    glm::vec3 ks = {0.5f, 0.5f, 0.5f};
    float ns = 32.0f;
};

struct Light {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    bool enabled = true;
};

struct Cube {
    GLuint VAO;
    GLuint textureID;
    int nVertices;
    glm::vec3 position;
    float scale;
    float rotAngleX, rotAngleY, rotAngleZ;

    Cube(GLuint vao, GLuint texID, int nv, glm::vec3 pos, float s = 0.3f)
        : VAO(vao), textureID(texID), nVertices(nv), position(pos), scale(s),
          rotAngleX(0.0f), rotAngleY(0.0f), rotAngleZ(0.0f) {}
};

// ---------------------------------------------------------------------------
// Protótipos
// ---------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
GLuint loadSimpleOBJ(const string& filePath, int& nVertices, GLuint& outTexID, Material& outMat);
Material parseMTL(const string& mtlPath);
GLuint loadTexture(const string& texPath);
int setupShader();
void updateLightPositions();

// ---------------------------------------------------------------------------
// Constantes e globais
// ---------------------------------------------------------------------------
const GLuint WIDTH = 1000, HEIGHT = 1000;

// Vertex shader: posição (0), UV (1), normal (2); aplica model/view/projection
const GLchar* vertexShaderSource =
    "#version 450\n"
    "layout (location = 0) in vec3 position;\n"
    "layout (location = 1) in vec2 texCoord;\n"
    "layout (location = 2) in vec3 normal;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 projection;\n"
    "out vec2 fragTexCoord;\n"
    "out vec3 fragPos;\n"
    "out vec3 fragNormal;\n"
    "void main()\n"
    "{\n"
    "    vec4 worldPos = model * vec4(position, 1.0);\n"
    "    gl_Position = projection * view * worldPos;\n"
    "    fragPos = vec3(worldPos);\n"
    "    fragNormal = mat3(transpose(inverse(model))) * normal;\n"
    "    fragTexCoord = texCoord;\n"
    "}\0";

// Fragment shader: Phong com 3 luzes pontuais e atenuação na difusa
const GLchar* fragmentShaderSource =
    "#version 450\n"
    "in vec2 fragTexCoord;\n"
    "in vec3 fragPos;\n"
    "in vec3 fragNormal;\n"
    "out vec4 color;\n"
    "uniform sampler2D textureSampler;\n"
    "uniform vec3 viewPos;\n"
    "uniform vec3 ka;\n"
    "uniform vec3 kd;\n"
    "uniform vec3 ks;\n"
    "uniform float ns;\n"
    "struct PointLight {\n"
    "    vec3 position;\n"
    "    vec3 color;\n"
    "    float intensity;\n"
    "    bool enabled;\n"
    "};\n"
    "uniform PointLight lights[3];\n"
    "void main()\n"
    "{\n"
    "    vec3 texColor = vec3(texture(textureSampler, fragTexCoord));\n"
    "    vec3 norm = normalize(fragNormal);\n"
    "    vec3 viewDir = normalize(viewPos - fragPos);\n"
    "    vec3 result = ka * 0.15 * texColor;\n"
    "    for (int i = 0; i < 3; i++) {\n"
    "        if (!lights[i].enabled) continue;\n"
    "        vec3 lightVec = lights[i].position - fragPos;\n"
    "        float dist = length(lightVec);\n"
    "        vec3 lightDir = lightVec / dist;\n"
    "        float attenuation = 1.0 / (1.0 + 1.0 * dist + 0.5 * dist * dist);\n"
    "        float diff = max(dot(norm, lightDir), 0.0);\n"
    "        result += kd * texColor * lights[i].color * lights[i].intensity * diff * attenuation;\n"
    "        vec3 reflectDir = reflect(-lightDir, norm);\n"
    "        float spec = pow(max(dot(viewDir, reflectDir), 0.0), ns);\n"
    "        result += ks * lights[i].color * lights[i].intensity * spec;\n"
    "    }\n"
    "    color = vec4(result, 1.0);\n"
    "}\n\0";

vector<Cube> cubes;
Light lights[3];
Camera* camera = nullptr;

// 0, 1, 2 = cubo específico; 3 = todos os cubos
int selectionMode = 0;

enum Mode { ROTATE, TRANSLATE, SCALE };
Mode currentMode = TRANSLATE;

const float TRANSLATE_STEP = 0.1f;
const float SCALE_STEP     = 0.05f;
const float SCALE_MIN      = 0.05f;
const float ROT_STEP       = glm::radians(5.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

template<typename Fn>
void applyToSelected(Fn fn) {
    if (selectionMode < (int)cubes.size())
        fn(cubes[selectionMode]);
    else
        for (auto& c : cubes) fn(c);
}

void updateLightPositions() {
    float s = cubes[0].scale;
    glm::vec3 p = cubes[0].position;
    lights[0].position = p + glm::vec3( s * 2.0f,  s * 2.0f,  s * 3.0f);
    lights[1].position = p + glm::vec3(-s * 2.0f,  s * 0.5f,  s * 2.0f);
    lights[2].position = p + glm::vec3( s * 0.0f,  s * 1.5f, -s * 3.0f);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main()
{
    glfwInit();

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Cubo 3D - Camera FPS", nullptr, nullptr);
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        cout << "Falha ao inicializar GLAD" << endl;
        return -1;
    }

    cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
    cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);

    camera = new Camera((float)WIDTH, (float)HEIGHT);

    GLuint shaderID = setupShader();
    glUseProgram(shaderID);

    GLint modelLoc      = glGetUniformLocation(shaderID, "model");
    GLint viewLoc       = glGetUniformLocation(shaderID, "view");
    GLint projLoc       = glGetUniformLocation(shaderID, "projection");
    GLint viewPosLoc    = glGetUniformLocation(shaderID, "viewPos");
    GLint kaLoc         = glGetUniformLocation(shaderID, "ka");
    GLint kdLoc         = glGetUniformLocation(shaderID, "kd");
    GLint ksLoc         = glGetUniformLocation(shaderID, "ks");
    GLint nsLoc         = glGetUniformLocation(shaderID, "ns");
    glUniform1i(glGetUniformLocation(shaderID, "textureSampler"), 0);

    GLint lightPosLoc[3], lightColorLoc[3], lightIntLoc[3], lightEnLoc[3];
    for (int i = 0; i < 3; i++) {
        string b = "lights[" + to_string(i) + "].";
        lightPosLoc[i]   = glGetUniformLocation(shaderID, (b + "position").c_str());
        lightColorLoc[i] = glGetUniformLocation(shaderID, (b + "color").c_str());
        lightIntLoc[i]   = glGetUniformLocation(shaderID, (b + "intensity").c_str());
        lightEnLoc[i]    = glGetUniformLocation(shaderID, (b + "enabled").c_str());
    }

    glEnable(GL_DEPTH_TEST);

    // Projection matrix — passada uma única vez (não muda a menos que o FOV mude)
    glm::mat4 projection = camera->getProjectionMatrix((float)WIDTH / HEIGHT);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    int nv;
    GLuint tex0, tex1, tex2;
    Material mat0, mat1, mat2;
    GLuint vao0 = loadSimpleOBJ("assets/modelo.obj", nv, tex0, mat0);
    cubes.push_back(Cube(vao0, tex0, nv, glm::vec3(-0.65f, 0.0f, 0.0f), 0.2f));

    GLuint vao1 = loadSimpleOBJ("assets/modelo.obj", nv, tex1, mat1);
    cubes.push_back(Cube(vao1, tex1, nv, glm::vec3( 0.00f, 0.0f, 0.0f), 0.2f));

    GLuint vao2 = loadSimpleOBJ("assets/modelo.obj", nv, tex2, mat2);
    cubes.push_back(Cube(vao2, tex2, nv, glm::vec3( 0.65f, 0.0f, 0.0f), 0.2f));

    if (vao0 == (GLuint)-1 || vao1 == (GLuint)-1 || vao2 == (GLuint)-1) {
        cerr << "Falha ao carregar modelos OBJ." << endl;
        glfwTerminate();
        return -1;
    }

    glUniform3fv(kaLoc, 1, glm::value_ptr(mat0.ka));
    glUniform3fv(kdLoc, 1, glm::value_ptr(mat0.kd));
    glUniform3fv(ksLoc, 1, glm::value_ptr(mat0.ks));
    glUniform1f(nsLoc, mat0.ns);

    lights[0].color = glm::vec3(1.0f, 1.0f, 0.95f); lights[0].intensity = 1.0f;
    lights[1].color = glm::vec3(0.8f, 0.9f, 1.0f);  lights[1].intensity = 0.5f;
    lights[2].color = glm::vec3(1.0f, 0.9f, 0.8f);  lights[2].intensity = 0.7f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        camera->processKeyboard(window, deltaTime);

        // Re-envia projection se o FOV mudou via scroll
        glm::mat4 proj = camera->getProjectionMatrix((float)WIDTH / HEIGHT);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

        string sel = (selectionMode < (int)cubes.size())
                     ? "Cubo " + to_string(selectionMode) : "TODOS";
        string modeName = (currentMode == ROTATE)    ? "ROTATE"
                        : (currentMode == TRANSLATE) ? "TRANSLATE" : "SCALE";
        string luzesSt = string(lights[0].enabled ? "1" : "_")
                       + (lights[1].enabled ? "2" : "_")
                       + (lights[2].enabled ? "3" : "_");
        glfwSetWindowTitle(window,
            ("Cubo 3D | Camera FPS | [" + sel + "] [" + modeName + "]"
             + " | WASD=cam Mouse=olhar Scroll=zoom | [luzes:" + luzesSt + "]").c_str());

        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Atualiza view matrix e posição da câmera para o Phong
        glm::mat4 view = camera->getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera->position));

        updateLightPositions();
        for (int i = 0; i < 3; i++) {
            glUniform3fv(lightPosLoc[i],   1, glm::value_ptr(lights[i].position));
            glUniform3fv(lightColorLoc[i], 1, glm::value_ptr(lights[i].color));
            glUniform1f (lightIntLoc[i],      lights[i].intensity);
            glUniform1i (lightEnLoc[i],        lights[i].enabled ? 1 : 0);
        }

        for (int i = 0; i < (int)cubes.size(); i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubes[i].position);
            model = glm::rotate(model, cubes[i].rotAngleX, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, cubes[i].rotAngleY, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, cubes[i].rotAngleZ, glm::vec3(0.0f, 0.0f, 1.0f));
            model = glm::scale(model, glm::vec3(cubes[i].scale));

            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, cubes[i].textureID);
            glBindVertexArray(cubes[i].VAO);
            glDrawArrays(GL_TRIANGLES, 0, cubes[i].nVertices);
        }
        glBindVertexArray(0);

        glfwSwapBuffers(window);
    }

    delete camera;
    glfwTerminate();
    return 0;
}

// ---------------------------------------------------------------------------
// Callbacks
// ---------------------------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (camera) camera->processMouse(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (camera) camera->processScroll(yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int modeFlag)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
        selectionMode = (selectionMode + 1) % ((int)cubes.size() + 1);

    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_R) currentMode = ROTATE;
        if (key == GLFW_KEY_T) currentMode = TRANSLATE;
        // S como modo Scale só ativa em GLFW_KEY_LEFT_SHIFT+S ou via tecla dedicada
        // para não conflitar com WASD. Usamos P para Scale neste módulo.
        if (key == GLFW_KEY_P) currentMode = SCALE;
        if (key == GLFW_KEY_1) lights[0].enabled = !lights[0].enabled;
        if (key == GLFW_KEY_2) lights[1].enabled = !lights[1].enabled;
        if (key == GLFW_KEY_3) lights[2].enabled = !lights[2].enabled;
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
        if (currentMode == TRANSLATE) {
            if (key == GLFW_KEY_LEFT)  applyToSelected([](Cube& c){ c.position.x -= TRANSLATE_STEP; });
            if (key == GLFW_KEY_RIGHT) applyToSelected([](Cube& c){ c.position.x += TRANSLATE_STEP; });
            if (key == GLFW_KEY_UP)    applyToSelected([](Cube& c){ c.position.y += TRANSLATE_STEP; });
            if (key == GLFW_KEY_DOWN)  applyToSelected([](Cube& c){ c.position.y -= TRANSLATE_STEP; });
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
            if (key == GLFW_KEY_LEFT)  applyToSelected([](Cube& c){ c.rotAngleY -= ROT_STEP; });
            if (key == GLFW_KEY_RIGHT) applyToSelected([](Cube& c){ c.rotAngleY += ROT_STEP; });
            if (key == GLFW_KEY_UP)    applyToSelected([](Cube& c){ c.rotAngleX -= ROT_STEP; });
            if (key == GLFW_KEY_DOWN)  applyToSelected([](Cube& c){ c.rotAngleX += ROT_STEP; });
            if (key == GLFW_KEY_X)     applyToSelected([](Cube& c){ c.rotAngleX += ROT_STEP; });
            if (key == GLFW_KEY_Y)     applyToSelected([](Cube& c){ c.rotAngleY += ROT_STEP; });
            if (key == GLFW_KEY_Z)     applyToSelected([](Cube& c){ c.rotAngleZ += ROT_STEP; });
        }
    }
}

// ---------------------------------------------------------------------------
// setupShader, parseMTL, loadTexture, loadSimpleOBJ
// ---------------------------------------------------------------------------
int setupShader()
{
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);
    GLint ok; GLchar log[512];
    glGetShaderiv(vs, GL_COMPILE_STATUS, &ok);
    if (!ok) { glGetShaderInfoLog(vs, 512, NULL, log); cout << "ERRO::VS: " << log << endl; }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, &ok);
    if (!ok) { glGetShaderInfoLog(fs, 512, NULL, log); cout << "ERRO::FS: " << log << endl; }

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) { glGetProgramInfoLog(prog, 512, NULL, log); cout << "ERRO::PROG: " << log << endl; }

    glDeleteShader(vs);
    glDeleteShader(fs);
    return prog;
}

Material parseMTL(const string& mtlPath)
{
    Material mat;
    ifstream f(mtlPath);
    if (!f.is_open()) { cerr << "MTL nao encontrado: " << mtlPath << endl; return mat; }
    string line;
    while (getline(f, line)) {
        istringstream ss(line);
        string word; ss >> word;
        if      (word == "map_Kd") ss >> mat.texFile;
        else if (word == "Ka")     ss >> mat.ka.r >> mat.ka.g >> mat.ka.b;
        else if (word == "Kd")     ss >> mat.kd.r >> mat.kd.g >> mat.kd.b;
        else if (word == "Ks")     ss >> mat.ks.r >> mat.ks.g >> mat.ks.b;
        else if (word == "Ns")     ss >> mat.ns;
    }
    return mat;
}

GLuint loadTexture(const string& texPath)
{
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_set_flip_vertically_on_load(true);
    int w, h, ch;
    unsigned char* data = stbi_load(texPath.c_str(), &w, &h, &ch, 0);
    if (data) {
        GLenum fmt = (ch == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        cout << "Textura carregada: " << texPath << " (" << w << "x" << h << ")" << endl;
    } else {
        unsigned char px[8*8*3];
        for (int i = 0; i < 8; i++) for (int j = 0; j < 8; j++) {
            int c = ((i/4)+(j/4))%2;
            px[(i*8+j)*3+0]=c?220:40; px[(i*8+j)*3+1]=c?0:40; px[(i*8+j)*3+2]=c?220:40;
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 8, 8, 0, GL_RGB, GL_UNSIGNED_BYTE, px);
        glGenerateMipmap(GL_TEXTURE_2D);
        cerr << "Textura nao encontrada (" << texPath << "), usando checkerboard." << endl;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    return texID;
}

GLuint loadSimpleOBJ(const string& filePath, int& nVertices, GLuint& outTexID, Material& outMat)
{
    vector<glm::vec3> vertices;
    vector<glm::vec2> texCoords;
    vector<glm::vec3> normals;
    vector<GLfloat>   vBuffer;

    string dir = filePath.substr(0, filePath.find_last_of("/\\") + 1);
    string mtllibName;

    ifstream arq(filePath);
    if (!arq.is_open()) { cerr << "Erro ao abrir: " << filePath << endl; outTexID=0; return (GLuint)-1; }

    string line;
    while (getline(arq, line)) {
        istringstream ss(line); string word; ss >> word;
        if      (word == "mtllib") ss >> mtllibName;
        else if (word == "v")  { glm::vec3 v; ss>>v.x>>v.y>>v.z; vertices.push_back(v); }
        else if (word == "vt") { glm::vec2 vt; ss>>vt.s>>vt.t; texCoords.push_back(vt); }
        else if (word == "vn") { glm::vec3 vn; ss>>vn.x>>vn.y>>vn.z; normals.push_back(vn); }
        else if (word == "f") {
            while (ss >> word) {
                int vi=0, ti=0, ni=0;
                istringstream sf(word); string idx;
                if (getline(sf,idx,'/')) vi=!idx.empty()?stoi(idx)-1:0;
                if (getline(sf,idx,'/')) ti=!idx.empty()?stoi(idx)-1:0;
                if (getline(sf,idx))    ni=!idx.empty()?stoi(idx)-1:0;
                vBuffer.push_back(vertices[vi].x); vBuffer.push_back(vertices[vi].y); vBuffer.push_back(vertices[vi].z);
                vBuffer.push_back(texCoords.empty()?0.0f:texCoords[ti].s);
                vBuffer.push_back(texCoords.empty()?0.0f:texCoords[ti].t);
                vBuffer.push_back(normals.empty()?0.0f:normals[ni].x);
                vBuffer.push_back(normals.empty()?0.0f:normals[ni].y);
                vBuffer.push_back(normals.empty()?1.0f:normals[ni].z);
            }
        }
    }
    arq.close();

    if (!mtllibName.empty()) outMat = parseMTL(dir + mtllibName);
    outTexID = loadTexture(outMat.texFile.empty() ? "" : dir + outMat.texFile);

    GLuint VBO, VAO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vBuffer.size()*sizeof(GLfloat), vBuffer.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(5*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    nVertices = (int)(vBuffer.size() / 8);
    return VAO;
}
