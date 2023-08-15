#define GLEW_STATIC
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <exception>

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

static void ValidateShader(unsigned int ShaderID){
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
static void ValidateProgram(unsigned int ProgramID){
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

int main(void)
{
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

    float Positions[8] = {
        0.5f,
        0.5f,

        -0.5f,
        0.5f,

        -0.5f,
        -0.5f,
    };

    float* PostingButHere = Positions;

    std::cout << std::endl << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), &Positions, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);


    std::string _vertexShader = 
    "#version 120\n"
    "\n"
    "attribute vec4 in_Position;\n"
    "\n"
    "void main()\n"
    "{\n"
    "   gl_Position = in_Position;\n"
    "}\n";


    std::string _fragmentShader = 
    "#version 120\n"
    "uniform vec2 WindowSize;\n"
    "void main()\n"
    "{\n"
    "   gl_FragColor = vec4(0.0, 1.0 - smoothstep(0, WindowSize.x, gl_FragCoord.x), mod(gl_FragCoord.x, 20)/19.0, 1.0);\n"
    "}\n";

    unsigned int Shader = CreateShader(_vertexShader, _fragmentShader);
    int uniform_WindowSize = glGetUniformLocation(Shader, "WindowSize");

    glUseProgram(Shader);

    glfwGetWindowSize(window, &width, &height);
    int _PastSize = width + height;
    glUniform2f(uniform_WindowSize, width, height);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glfwGetWindowSize(window, &width, &height);
        if(width + height != _PastSize){
            glUniform2f(uniform_WindowSize, width, height);
            _PastSize = width + height;
        }
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteProgram(Shader);
    glfwTerminate();
    return 0;
}