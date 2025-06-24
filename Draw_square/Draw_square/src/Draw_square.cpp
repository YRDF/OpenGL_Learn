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
                //����ģʽΪ����
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                //����Ƭ��ģʽ
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
    //�� GLSL Դ�����������ɫ������(����ָ������ɫ����Ƭ����ɫ���Ĵ���) 
    glShaderSource(id, 1, &src, nullptr);
    //����GLSL����ΪGPUָ��
    glCompileShader(id);

    //TODO:error handling
    //��ȡ��ɫ������shader object������ز�����Ϣ
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    //�ж� ������˾ͻ�ȡ������Ϣ
    if (result == GL_FALSE) {
        //��ȡ������Ϣ����
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        //char message[length];
        //char* message = new char[length];
        char* message = (char*)alloca(length * sizeof(char));
        //��ȡlog
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
    //��д������������ɫ������Ĵ���:
    //����һ���µ���ɫ���������
    unsigned int program = glCreateProgram();

    //������ɫ������
    unsigned int vetexS = ConpileShader(GL_VERTEX_SHADER, vetexShader);
    unsigned int fragmentS = ConpileShader(GL_FRAGMENT_SHADER, fragmentShader);

    //��ɫ�����������ɫ���������
    glAttachShader(program, vetexS);
    glAttachShader(program, fragmentS);

    //���ӵ�����
    glLinkProgram(program);
    //��֤��ɫ�������ڵ�ǰ OpenGL ״̬�µ���Ч��
    glValidateProgram(program);

    //ɾ���Ѵ�������ɫ��������Ϊ�Ѿ���ŵ���ɫ�������������
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

    //����������洢����
    float positions[ ] = {
        -0.5f, -0.5f,   //0
         0.5f, -0.5f,   //1
         0.5f,  0.5f,   //2
        -0.5f,  0.5f,   //3
    };

    //����������
    unsigned int indices[ ] = {
        0,1,2,
        2,3,0
    };

    //��CPU���岢�����ɶ��㻺����
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    //�󶨶��㻺������֪�������������GL_ARRAY_BUFFER��
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    //����������GPU����
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    //���ö�������(ָ�����㻺�����Ĳ��ֲ�����Щ���԰󶨵�����0)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    glEnableVertexAttribArray(0);


    //��CPU���岢��������������
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    //��������������֪�������������GL_ELEMENT_ARRAY_BUFFER��
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    //����������GPU����
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    //��󶥵㻺����
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    ShaderProgramSouces source = ParseShader("res/shaders/Basic.shader");
    //�����Զ��������ɫ������
    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSouces);
    //����ɫ��(ָ����ǰҪʹ�õ���ɫ������)
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
    //ɾ����ɫ��
    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}