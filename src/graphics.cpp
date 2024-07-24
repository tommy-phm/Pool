#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "simulator.h"
#include "icon_data.h"

// Settings - W and H are in inches. X and Y ratios.
const float screenX = areaW + ball_d * 4.0f + 12.0f;
const float screenY = areaH + ball_d * 4.0f + 12.0f;
const float halfX = screenX / 2.0f;
const float halfY = screenY / 2.0f;
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = SCR_WIDTH * screenY / screenX;

// Ratio Scale
const float wallX = (areaW  + ball_d * 4.0f) / screenX;
const float wallY = (areaH + ball_d * 4.0f) / screenY;
const float edgeX = (areaW  + ball_d) / screenX;
const float edgeY = (areaH + ball_d) / screenY;
const float areaX = areaW / screenX;
const float areaY = areaH / screenY;
const float ballX = ball_d / screenX;
const float ballY = ball_d / screenY;
const float markerX = ballX / 3.0f;
const float markerY = ballY / 3.0f;
const float colX = (areaW  + ball_d * 2.5f)  / screenX; //Marker collium distance to center 
const float rowY = (areaH + ball_d * 2.5f) / screenY; //Marker row distance to center 
const float holeX = ballX * 2;
const float holeY = ballY * 2; 

//Ball color
const float r[] = {1.0f, 0.07f, 0.9f, 0.41f, 0.97f, 0.23f, 0.64f, 0.09f, 1.0f, 0.07f, 0.9f, 0.41f, 0.97f, 0.23f, 0.64f, 1.0f};
const float g[] = {0.91f, 0.3f, 0.23f, 0.22f, 0.46f, 0.54f, 0.15f, 0.08f, 0.91f, 0.3f, 0.23f, 0.22f, 0.46f, 0.54f, 0.15f, 1.0f};
const float b[] = {0.38f, 0.54f, 0.27f, 0.42f, 0.13f, 0.28f, 0.2f, 0.15f, 0.38f, 0.54f, 0.27f, 0.42f, 0.13f, 0.28f, 0.2f, 1.0f};

//Aimming Settings
const float aimmingLimit = 12.0f; // Max aiming line length
const float powerFactor = 0.15f; // Aiming power factor

// Object Model
float vertices[] = {
    //Ball
    wallX, wallY, 0.0f,
    wallX, -wallY, 0.0f,
    -wallX, -wallY, 0.0f,
    -wallX, wallY, 0.0f,

    //Edge
    edgeX, edgeY, 0.0f,
    edgeX, -edgeY, 0.0f,
    -edgeX, -edgeY, 0.0f,
    -edgeX, edgeY, 0.0f,

    //Area
    areaX, areaY, 0.0f,
    areaX, -areaY, 0.0f,
    -areaX, -areaY, 0.0f,
    -areaX, areaY, 0.0f,

    //Ball
    -0.70f * ballX, 0.70f * ballY, 0.0f,
    0.0f * ballX, 1.0f * ballY, 0.0f,
    0.70f * ballX, 0.70f * ballY, 0.0f,
    1.0f * ballX,  0.0f * ballY, 0.0f,
    0.70f * ballX, -0.70f * ballY, 0.0f,
    0.0f * ballX, -1.0f * ballY, 0.0f,
    -0.70f * ballX, -0.70f * ballY, 0.0f,
    -1.0f * ballX, 0.0f * ballY, 0.0f,
    
    //Marker
    0.0f * markerX, 1.0f * markerY, 0.0f,
    0.70f * markerX, 0.70f * markerY, 0.0f,
    1.0f * markerX,  0.0f * markerY, 0.0f,
    0.70f * markerX, -0.70f * markerY, 0.0f,
    0.0f * markerX, -1.0f * markerY, 0.0f,
    -0.70f * markerX, -0.70f * markerY, 0.0f,
    -1.0f * markerX, 0.0f * markerY, 0.0f,
    -0.70f * markerX, 0.70f * markerY, 0.0f,

    //Line
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,

    //Hole
    0.0f * holeX, 1.0f * holeY, 0.0f,
    0.70f * holeX, 0.70f * holeY, 0.0f,
    1.0f * holeX,  0.0f * holeY, 0.0f,
    0.70f * holeX, -0.70f * holeY, 0.0f,
    0.0f * holeX, -1.0f * holeY, 0.0f,
    -0.70f * holeX, -0.70f * holeY, 0.0f,
    -1.0f * holeX, 0.0f * holeY, 0.0f,
    -0.70f * holeX, 0.70f * holeY, 0.0f,
}; 

const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    uniform mat4 model;
    void main() {
        gl_Position = model * vec4(aPos.x, aPos.y, 0.0, 1.0);
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    uniform vec4 color; // Color
    void main() {
        FragColor = color;
    }
)";

GLFWwindow* window;
unsigned int VBO, VAO;
bool aiming = false;


// Limits aimming line to 12in
static void cursorPositionCallback( GLFWwindow *window, double xpos, double ypos ){
    if(aiming){
        vertices[87] = (static_cast<float>(xpos) / SCR_WIDTH) * 2.0f - 1.0f;
        vertices[88] = (1.0f -(static_cast<float>(ypos) / SCR_HEIGHT)) * 2.0f -1.0f;
        float x = (vertices[84] - vertices[87]);
        float y = (vertices[85] - vertices[88]);
        float normalX = x * screenX / 2.0f;
        float normalY = y * screenY / 2.0f;
        float distance = sqrt(pow(normalX, 2) + pow(normalY, 2));
        if(distance > 10.0f){
            float ratio = distance / 10.0f;
            vertices[87] = vertices[84] - x / ratio;
            vertices[88] = vertices[85] - y / ratio;
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    }
}

// Handle Mouse Input
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {

    if (button == GLFW_MOUSE_BUTTON_LEFT && !active) {
        if (action == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            float x1 = balls[ball_n - 1].x / halfX;
            float y1 = balls[ball_n - 1].y / halfY;
            float x2 = (static_cast<float>(xpos) / SCR_WIDTH) * 2.0f - 1.0f;
            float y2 = (1.0f -(static_cast<float>(ypos) / SCR_HEIGHT)) * 2.0f -1.0f;

            //Aiming is only enable if cursor is within 12in of white ball
            if((pow((x2 - x1), 2) / pow(24 / screenX, 2) +  pow((y2 - y1), 2) / pow(24 / screenY, 2)) < 1){
                aiming = true;
                vertices[84] = x1;
                vertices[85] = y1;
                vertices[87] = x2;
                vertices[88] = y2;
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
            }
        } 
        else if (action == GLFW_RELEASE && aiming) {
            aiming = false;
            float i = (vertices[84] - vertices[87]) * screenX / 2.0f * powerFactor;
            float j = (vertices[85] - vertices[88]) * screenY / 2.0f * powerFactor;
            hit(i, j);
        }
    }
}

// Setup and display graphics
void render(){
    if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Pool Simulator", NULL, NULL);

    GLFWimage icon;
    icon.width = 72; 
    icon.height = 72;
    icon.pixels = icon_data;
    glfwSetWindowIcon(window, 1, &icon);
    glfwSetCursorPosCallback(window, cursorPositionCallback );
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwMakeContextCurrent(window);
    
    if (window == NULL){
        std::cout << "[Pool Simulator] ERROR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "[Pool Simulator] ERROR: Failed to initialize GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success){
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "[Pool Simulator] ERROR: SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success){
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "[Pool Simulator] ERROR: SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "[Pool Simulator] ERROR: SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        exit(EXIT_FAILURE);
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    glm::mat4 wallModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 markerModel[20];
    glm::mat4 holeModel[6];
    glm::mat4 edgeModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 areaModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 ballModel[ball_n];
    glm::mat4 lineModel = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));


    GLint modelLocation = glGetUniformLocation(shaderProgram, "model");
    GLint colorLocation = glGetUniformLocation(shaderProgram, "color");

    std::cout << "[Pool Simulator] Graphic module has sucessfully loaded\n";
    std::cout << "[Pool Simulator] Game is now ready. Have fun!\n";
    while (!glfwWindowShouldClose(window)){
        glClearColor(0.15f, 0.17f, 0.27f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO); 

        // Draw Wall
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(wallModel));
        glUniform4f(colorLocation, 0.72f, 0.44f, 0.31f, 0.23f);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        //Draw Markers
        int n = 0;

        // Draw markers along the X-axis
        for (int i = 0; i < 7; i++) {
            for (int j = -1; j <= 1; j += 2) {
                markerModel[n] = glm::translate(glm::mat4(1.0f), glm::vec3(((screenX / 2 - 36.0f + 12.0f * i) / screenX * 2.0f - 1.0), j * rowY, 0.0f));
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(markerModel[n]));
                glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
                glDrawArrays(GL_TRIANGLE_FAN, 20, 8);
                n++;
            }
        }

        // Draw markers along the Y-axis
        for (int i = 0; i < 3; i++) {
            for (int j = -1; j <= 1; j += 2) {
                markerModel[n] = glm::translate(glm::mat4(1.0f), glm::vec3(j * colX, ((screenY / 2 - 12.0f + 12.0f * i) / screenY * 2.0f - 1.0f), 0.0f));
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(markerModel[n]));
                glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
                glDrawArrays(GL_TRIANGLE_FAN, 20, 8);
                n++;
            }
        }

        // Draw Edge
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(edgeModel));
        glUniform4f(colorLocation, 0.35f, 0.41f, 0.53f, 1.0f);
        glDrawArrays(GL_TRIANGLE_FAN, 4, 4);

        // Draw Hole
        n = 0;
        for (int i = 0; i < 3; i++) {
            holeModel[n] = glm::translate(glm::mat4(1.0f), glm::vec3(((screenX / 2 - 48.0f + 48.0f * i) / screenX * 2.0f - 1.0), ((screenY / 2 - 24.0f) / screenY * 2.0f - 1.0f), 0.0f));
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(holeModel[n]));
            glUniform4f(colorLocation, 0.0f, 0.0f, 0.0f, 1.0f);
            glDrawArrays(GL_TRIANGLE_FAN, 30, 8);
            n++;
            holeModel[n] = glm::translate(glm::mat4(1.0f), glm::vec3(((screenX / 2 - 48.0f + 48.0f * i) / screenX * 2.0f - 1.0), ((screenY / 2 + 24.0f) / screenY * 2.0f - 1.0f), 0.0f));
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(holeModel[n]));
            glUniform4f(colorLocation, 0.0f, 0.0f, 0.0f, 1.0f);
            glDrawArrays(GL_TRIANGLE_FAN, 30, 8);
            n++;
        }

        // Draw Area
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(areaModel));
        glUniform4f(colorLocation, 0.55f, 0.61f, 0.71f, 1.0f);
        glDrawArrays(GL_TRIANGLE_FAN, 8, 4);

        //Draw Ball
        for(int i = 0; i < ball_n; i++){
            if(balls[i].show){
                ballModel[i] = glm::mat4(1.0f);
                ballModel[i] = glm::translate(ballModel[i], glm::vec3(balls[i].x / halfX, balls[i].y / halfY, 0.0f));
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(ballModel[i]));
                glUniform4f(colorLocation, r[i], g[i], b[i], 1.0f);
                glDrawArrays(GL_TRIANGLE_FAN, 12, 8);
                if(i > 7){
                    glUniform4f(colorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
                    glDrawArrays(GL_TRIANGLE_FAN, 12, 3);
                    glDrawArrays(GL_TRIANGLE_FAN, 16, 3);
                }
            }
        }

        //Draw aiming line
        if(aiming){
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(lineModel));
            glDrawArrays(GL_LINES, 28, 3);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    endGame = true;
}

void closeWindow() {
    if (window) 
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

