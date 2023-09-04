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
#include <vector>
#include <sstream>
#include <fstream>
#include <string>

#define GLCall(x) clearErrors();\
    x;\
    if(writeErrors()) LogLine(#x, __FILE__, __LINE__); 
    
template<typename T>
void coutArray(T* array, unsigned int arrayLength){
    for(int i = 0; i < arrayLength; i++){
        std::cout << array[i] << ", ";
    }
    std::cout << "\n";
}
static void LogLine(const char* function, const char* file, int line) {
    std::cout << function << ":" << line <<  "\nfile: " << file <<"\n\n";
}
inline void clearErrors() {
    while(glGetError() != GL_NO_ERROR);
}
inline bool writeErrors(){
    bool flag = false;
    GLenum errorCode = glGetError();
    if(errorCode != GL_NO_ERROR){
        std::cout << "*** OPEN_GL ERROR ***\n";
        flag = true;
    }
    while(errorCode != GL_NO_ERROR){
        std::string output = ""; 
        switch(errorCode){
            case GL_INVALID_ENUM:
                output += "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                output += "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                output += "GL_INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                output += "GL_STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                output += "GL_STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                output += "GL_OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                output += "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            case GL_CONTEXT_LOST:
                output += "GL_CONTEXT_LOST";
                break;
            case GL_TABLE_TOO_LARGE:
                output += "GL_TABLE_TOO_LARGE";
                break;
            default:
                output += "Unkown error <" + std::to_string((int)errorCode) + ">";
                break;
        }
        std::cout << "Error Code: " << output << "\n";


        errorCode = glGetError();
    }
    return flag;
}

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
        Vector2 operator/(Vector2 obj){
            Vector2 tmp;
            tmp.X = X / obj.X;
            tmp.Y = Y / obj.Y;
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
    private:
        unsigned int indecies[6] = {
            0,1,3,
            2,1,3
        };
    public:
        float Scale = 1;
        Vector2 position;
        Vector2 size;
        static const int VERTEX_COUNT = 8;
        static const int INDECIE_COUNT = 6;
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

        void Indecies(unsigned int offset, unsigned int * outArray){
            for(int i = 0; i < INDECIE_COUNT; i++){
                outArray[i] = offset + indecies[i];
            }
        }
        

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
class BufferMaintainer{

    public:
        std::vector<GameBlock*> blocks;
        unsigned int amount = 0;
        unsigned int vertexBlockCount;
        unsigned int indeciesBlockCount;
        unsigned int indeciesOffset;

        BufferMaintainer(unsigned int vertexBlockCount, unsigned int indeciesBlockCount, unsigned int indeciesOffset){
            this->vertexBlockCount = vertexBlockCount;
            this->indeciesBlockCount = indeciesBlockCount;
            this->indeciesOffset = indeciesOffset;
        }
        void addPointer(GameBlock* gameBlockPointer){
            amount++;
            blocks.push_back(gameBlockPointer);
            std::cout << blocks.at(0) << "\n";
        }
        unsigned int GetVertexBufferByteSize(){
            return GetVertexCount() * sizeof(float);
        }
        unsigned int GetVertexCount(){
            if(amount == 0) return 0;
            return amount * vertexBlockCount;
        }
        unsigned int GetIndeciesByteSize(){
            return GetIndeciesCount() * sizeof(unsigned int);
        }
        unsigned int GetIndeciesCount(){
            if(amount == 0) return 0;
            return amount * indeciesBlockCount;
        }
        void GetVerteciesBuffer(float* BufferOut) {
            for(int i = 0; i < amount; i++){
                blocks[i]->Vertecies(&BufferOut[i * vertexBlockCount]);
            }
        }
        void GetIndeciesBuffer(unsigned int* BufferOut) {
            for(int i = 0; i < amount; i++){
                blocks[i]->Indecies(i*indeciesOffset, &BufferOut[i*indeciesBlockCount]);
            }
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
GameBlock gameSquare(0,0,0.3f,0.3f);
GameBlock gameSquare2(0,0,0.3f,0.3f);
void keyEvent(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        gameSquare.size = gameSquare.size / Vector2(2,2);
    }

}

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


    BufferMaintainer bufferManager(8, 6, 4);
    GameBlock squares[] {
        GameBlock(1,1,0.1f,0.1f),
        GameBlock(-1,-1,0.1f,0.1f),
        GameBlock(-0.8f,0.9f,0.5f,0.1f),
    };
        
    for(int i = 0; i < 3; i++){
        bufferManager.addPointer(&squares[i]);
    }
    
    bufferManager.addPointer(&gameSquare);
    bufferManager.addPointer(&gameSquare2);

    unsigned int buffer;
    float dataBuffer[bufferManager.GetVertexCount()];
    bufferManager.GetVerteciesBuffer(&dataBuffer[0]);

    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, bufferManager.GetVertexBufferByteSize(), &dataBuffer, GL_STATIC_DRAW);

    unsigned int indexcesBuffer;
    unsigned int indeciexDataBuffer[bufferManager.GetIndeciesCount()];
    bufferManager.GetIndeciesBuffer(&indeciexDataBuffer[0]);

    glGenBuffers(2, &indexcesBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexcesBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferManager.GetIndeciesByteSize(), &indeciexDataBuffer, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);

    coutArray<float>(&dataBuffer[0], bufferManager.GetVertexCount());
    coutArray<unsigned int>(&indeciexDataBuffer[0], bufferManager.GetIndeciesCount());

    std::cout << "memory Comparison\n";

    std::cout << bufferManager.blocks.at(0) << "\n";
    std::cout << &gameSquare << ", " << &gameSquare2 << "\n";

    unsigned int Shader = CreateShader(shadersSource.vertexSource, shadersSource.fragmentSource);

    int uniform_WindowSize = glGetUniformLocation(Shader, "WindowSize");

    glUseProgram(Shader);

    glfwGetWindowSize(window, &width, &height);
    int _PastSize = width + height;
    glUniform2f(uniform_WindowSize, width, height);
    bool _flipV = RandFloat() > 0.5f;
    bool _flipH =  RandFloat() > 0.5f;
    Vector2 Speed(0.3f * (_flipH ? -1 : 1),0.3f * (_flipV ? -1 : 1));
    Vector2 Speed2(0.3f * (_flipH ? 1 : -1),0.3f * (_flipV ? -1 : 1));
    float speedLimit = 0.2f;
    const Vector2 START_SPEED(abs(Speed.X), abs(Speed.Y));

    std::chrono::high_resolution_clock::time_point pastTime = std::chrono::high_resolution_clock::now();
    float deltaTime = 0.00001f;
    glfwSetKeyCallback(window, keyEvent);
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
        gameSquare2.position.X += Speed2.X * deltaTime;
        gameSquare2.position.Y += Speed2.Y * deltaTime;

        if(gameSquare2.Right() > 1 && Speed2.X > 0 || gameSquare2.Left() < -1 && Speed2.X < 0) {
            Speed2.X *= -1;
            Speed2.Y = START_SPEED.Y * (0.7f + 0.6f * RandFloat()) * (Speed2.Y / abs(Speed2.Y)); 
        }
        if(gameSquare2.Bottom() > 1 && Speed2.Y > 0 || gameSquare2.Top() < -1 && Speed2.Y < 0) { 
            Speed2.Y *= -1;
            Speed2.X = START_SPEED.X * (0.7f + 0.6f * RandFloat()) * (Speed2.X / abs(Speed2.X)); 
        }
        

        bufferManager.GetVerteciesBuffer(&dataBuffer[0]);



        glBufferData(GL_ARRAY_BUFFER,bufferManager.GetVertexBufferByteSize(), &dataBuffer, GL_STATIC_DRAW);
        //updatesShader uniform
        glfwGetWindowSize(window, &width, &height);
        if(width + height != _PastSize){
            glUniform2f(uniform_WindowSize, width, height);
            _PastSize = width + height;
        }
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        GLCall(glDrawElements(GL_TRIANGLES,bufferManager.GetIndeciesCount(), GL_UNSIGNED_INT, nullptr));

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();

        // writes all error codes.
        writeErrors();
    }

    glDeleteProgram(Shader);
    glfwTerminate();
    return 0;
}


