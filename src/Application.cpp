#include <cstdlib>
#include <complex>
#include <ctime>
#include <ratio>
#include <chrono>
#define GLEW_STATIC
#include <iostream>
#include "include/glew.h"
#include "include/glfw3.h"
#include <exception>
#include <sstream>
#include <fstream>
#include <string>

struct ShaderSourceStruct {

    std::string vertexSource;
    std::string fragmentSource;
};

static ShaderSourceStruct GetShaderSource(std::string &filePath){

    std::ifstream fileStream;
    fileStream.open(filePath);
    if(!fileStream.is_open()){
        std::cout << "Failed to open shaderFile: " << filePath << "\n";
    }
    std::stringstream stringBuilder[2];
    std::string line;
    enum class ShaderEnum {
        NONE = -1, Vertex = 0, Fragment = 1 
    };
    ShaderEnum currentShader = ShaderEnum::NONE;
    while(getline(fileStream, line)){
        if(line.find("#shader") != std::string::npos){
            if(line.find("vertex") != std::string::npos){
                currentShader = ShaderEnum::Vertex;
            }
            if(line.find("fragment") != std::string::npos){
                currentShader = ShaderEnum::Fragment;
            }
        } else if(currentShader != ShaderEnum::NONE) {
            stringBuilder[(int)currentShader] << line << "\n";
        }
    }
    return {stringBuilder[(int)ShaderEnum::Vertex].str(), stringBuilder[(int)ShaderEnum::Fragment].str()};
}


inline float RandFloat(){
    return ((float) rand()) / RAND_MAX;
}
class Block {
    public:
        float scale;
};
class Vector2{

    public:
        float X,Y;
        Vector2(float X = 0, float Y = 0){
            this->X = X;
            this->Y = Y;
        }
        Vector2 operator+(Vector2 obj){
            Vector2 tmp;
            tmp.X = X + obj.X;
            tmp.Y = Y + obj.Y;
            return tmp;
        }
        
        Vector2 operator-(Vector2 obj){
            Vector2 tmp;
            tmp.X = X - obj.X;
            tmp.Y = Y - obj.Y;
            return tmp;
        }
        
        Vector2 operator*(Vector2 obj){
            Vector2 tmp;
            tmp.X = X * obj.X;
            tmp.Y = Y * obj.Y;
            return tmp;
        }
        Vector2 operator*(int num){
            Vector2 tmp;
            tmp.X = X * num;
            tmp.Y = Y * num;
            return tmp;
        }
};
class GameBlock{

    public:
        unsigned int indices[6] = {
            0,1,3,
            2,1,3
        };
        float Scale = 1;
        Vector2 position;
        Vector2 size;
        GameBlock(float x, float y, float width, float height){
            position = Vector2(x,y);
            size = Vector2(width, height);
        }
        GameBlock(Vector2 Position, Vector2 Size){
            position = Position;
            size = Size;
        }
        float Left(){ return position.X - size.X / 2;}
        float Right(){ return position.X + size.X / 2;}
        float Top(){ return position.Y - size.Y / 2;}
        float Bottom(){ return position.Y + size.Y / 2;}

        void Vertecies(float *outArray){
            outArray[0] = Left();
            outArray[1] = Top();
            outArray[2] = Right();
            outArray[3] = Top();
            outArray[4] = Right();
            outArray[5] = Bottom();
            outArray[6] = Left();
            outArray[7] = Bottom();
        }

};
class CustomException : public std::exception
{
    public:
        std::string Message;
    CustomException(std::string message){
        Message = message;
    }
    virtual const char* what() const throw()
    {
        return Message.c_str();
    }
};

static void ValidateShader(unsigned int ShaderID) {
    int status = 0;
    glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &status);
    if(status == GL_FALSE){
        int logLength;
        glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &logLength);
        char* Message = (char*)alloca(logLength * sizeof(char));
        glGetShaderInfoLog(ShaderID, logLength, NULL, Message);
        std::string errorMessage = "Shader Failed to compile: ";
        errorMessage.append(Message);
        throw CustomException(errorMessage);
    }
} 
static void ValidateProgram(unsigned int ProgramID) {
    int status = 0;
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &status);
    if(status == GL_FALSE){
        int logLength;
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &logLength);
        char* Message = (char*)alloca(logLength * sizeof(char));
        glGetProgramInfoLog(ProgramID, logLength, NULL, Message);
        std::string errorMessage = "Program Failed to link shaders: ";
        errorMessage.append(Message);
        throw CustomException(errorMessage);
    }
}
static unsigned int CompileShader(unsigned int type, const std::string &source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    ValidateShader(id);

    return id;
}
static unsigned int CreateShader(const std::string &vertexShader, const std::string &fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int _vertexShader = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int _fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, _vertexShader);
    glAttachShader(program, _fragmentShader);

    glLinkProgram(program);

    glValidateProgram(program);
    
    ValidateProgram(program);

    glDeleteShader(_vertexShader);
    glDeleteShader(_fragmentShader);

    return program;
}

int main(int argc, char* argv[])
{
    int slashPos = std::string(argv[0]).find_last_of("/");
    std::string WORKING_DIR = ".";
    if(slashPos != std::string::npos){
        WORKING_DIR = std::string(argv[0]).substr(0,slashPos);
    }


    std::srand(std::time(NULL));
    
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    int width = 640;
    int height = 480;
    std::string programName = "My Program";

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, programName.c_str(), NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }


    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if(glewInit() != GLEW_OK){
        return -1;
    }
    
    std::cout << "OpenGl Version on graphics card: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLS Shader Version on graphics card: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    std::string shaderPath = WORKING_DIR + "/shaders/basic.shader";
    ShaderSourceStruct shadersSource = GetShaderSource(shaderPath);

    float positionsArray[8];

    GameBlock gameSquare(0,0,0.3f,0.3f);

    gameSquare.Vertecies(&positionsArray[0]);
    
    
    unsigned int buffer;

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), &positionsArray, GL_STATIC_DRAW);

    unsigned int indexcesBuffer;

    glGenBuffers(2, &indexcesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexcesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), &gameSquare.indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);



    unsigned int Shader = CreateShader(shadersSource.vertexSource, shadersSource.fragmentSource);

    int uniform_WindowSize = glGetUniformLocation(Shader, "WindowSize");

    glUseProgram(Shader);

    glfwGetWindowSize(window, &width, &height);
    int _PastSize = width + height;
    glUniform2f(uniform_WindowSize, width, height);
    bool _flipV = RandFloat() > 0.5f;
    bool _flipH =  RandFloat() > 0.5f;
    Vector2 Speed(0.3f * (_flipH ? -1 : 1),0.3f * (_flipV ? -1 : 1));
    float speedLimit = 0.2f;
    const Vector2 START_SPEED(abs(Speed.X), abs(Speed.Y));

    std::chrono::high_resolution_clock::time_point pastTime = std::chrono::high_resolution_clock::now();
    float deltaTime = 0.00001f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        std::chrono::high_resolution_clock::time_point tempTime = std::chrono::high_resolution_clock::now();

        deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(tempTime - pastTime).count();
        if(deltaTime == 0) deltaTime += 0.00001f;
        pastTime = tempTime;
        gameSquare.position.X += Speed.X * deltaTime;
        gameSquare.position.Y += Speed.Y * deltaTime;

        if(gameSquare.Right() > 1 && Speed.X > 0 || gameSquare.Left() < -1 && Speed.X < 0) {
            Speed.X *= -1;
            Speed.Y = START_SPEED.Y * (0.7f + 0.6f * RandFloat()) * (Speed.Y / abs(Speed.Y)); 
        }
        if(gameSquare.Bottom() > 1 && Speed.Y > 0 || gameSquare.Top() < -1 && Speed.Y < 0) { 
            Speed.Y *= -1;
            Speed.X = START_SPEED.X * (0.7f + 0.6f * RandFloat()) * (Speed.X / abs(Speed.X)); 
        }
        
        gameSquare.Vertecies(&positionsArray[0]);

        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), &positionsArray, GL_STATIC_DRAW);
        //updatesShader uniform
        glfwGetWindowSize(window, &width, &height);
        if(width + height != _PastSize){
            glUniform2f(uniform_WindowSize, width, height);
            _PastSize = width + height;
        }
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(Shader);
    glfwTerminate();
    return 0;
}


