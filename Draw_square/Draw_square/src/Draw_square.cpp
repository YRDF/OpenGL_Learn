#include<GL/glew.h>
#include <GLFW/glfw3.h>

#include<iostream>
#include<fstream>
#include<string>
#include<sstream>

struct ShaderProgramSouces {
    std::string VertexSource;
    std::string FragmentSouces;
};

static ShaderProgramSouces ParseShader(const std::string& filepath) {
    std::ifstream stream(filepath);

    enum class ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                //设置模式为顶点
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                //设置片段模式
                type = ShaderType::FRAGMENT;
        }
        else if (type != ShaderType::NONE) {
            ss[(int)type] << line << '\n';
        }
    }
    return { ss[0].str(),ss[1].str() };
}

static unsigned int ConpileShader(unsigned int type, const std::string& source) {
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    //将 GLSL 源代码关联到着色器对象(这里指顶点着色器和片段着色器的代码) 
    glShaderSource(id, 1, &src, nullptr);
    //编译GLSL代码为GPU指令
    glCompileShader(id);

    //TODO:error handling
    //获取着色器对象（shader object）的相关参数信息
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    //判断 如果错了就获取错误信息
    if (result == GL_FALSE) {
        //获取错误信息长度
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        //char message[length];
        //char* message = new char[length];
        char* message = (char*)alloca(length * sizeof(char));
        //获取log
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " <<
            (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
            << "shader!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}


static unsigned int CreateShader(const std::string& vetexShader, const std::string& fragmentShader) {
    //编写编译这两个着色器所需的代码:
    //创建一个新的着色器程序对象
    unsigned int program = glCreateProgram();

    //创建着色器对象
    unsigned int vetexS = ConpileShader(GL_VERTEX_SHADER, vetexShader);
    unsigned int fragmentS = ConpileShader(GL_FRAGMENT_SHADER, fragmentShader);

    //着色器对象放入着色器程序对象
    glAttachShader(program, vetexS);
    glAttachShader(program, fragmentS);

    //连接到程序
    glLinkProgram(program);
    //验证着色器程序在当前 OpenGL 状态下的有效性
    glValidateProgram(program);

    //删除已创建的着色器对象，因为已经存放到着色器程序对象中了
    glDeleteShader(vetexS);
    glDeleteShader(fragmentS);

    return program;

}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;


    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;

    //浮点数数组存储顶点
    float positions[ ] = {
        -0.5f, -0.5f,   //0
         0.5f, -0.5f,   //1
         0.5f,  0.5f,   //2
        -0.5f,  0.5f,   //3
    };

    //索引缓冲区
    unsigned int indices[ ] = {
        0,1,2,
        2,3,0
    };

    //在CPU定义并且生成顶点缓冲区
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    //绑定顶点缓冲区告知后续操作是针对GL_ARRAY_BUFFER的
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    //缓冲区放入GPU数据
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    //配置顶点属性(指定顶点缓冲区的布局并把这些属性绑定到索引0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glEnableVertexAttribArray(0);


    //在CPU定义并生成索引缓冲区
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    //绑定索引缓冲区告知后续操作是针对GL_ELEMENT_ARRAY_BUFFER的
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    //缓冲区放入GPU数据
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    //解绑顶点缓冲区
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    ShaderProgramSouces source = ParseShader("res/shaders/Basic.shader");
    //调用自定义编译着色器代码
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSouces);
    //绑定着色器(指定当前要使用的着色器程序)
    glUseProgram(shader);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        /*glBegin(GL_TRIANGLES);
        glVertex2f(-0.5f, -0.5f);
        glVertex2f(0.0f,   0.5f);
        glVertex2f(0.5f,  -0.5f);
        glEnd();*/

        //glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,nullptr);


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    //删除着色器
    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}