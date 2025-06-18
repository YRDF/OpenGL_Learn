#include<GL/glew.h>
#include <GLFW/glfw3.h>

#include<iostream>

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
        std::cout << "Failed to compile"<<
            (type == GL_VERTEX_SHADER ?"vertex" :"fragment")
            << "shader! " << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}


static unsigned int CreateShader(const std::string& vetexShader,const std::string& fragmentShader) {
    //��д������������ɫ������Ĵ���:
    //����һ���µ���ɫ���������
    unsigned int program = glCreateProgram();

    //������ɫ������
    unsigned int vetexS = ConpileShader(GL_VERTEX_SHADER, vetexShader);
    unsigned int fragmentS = ConpileShader(GL_FRAGMENT_SHADER,fragmentShader);

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
    float positions[6] = {
        -0.5f, -0.5f,
         0.0f,  0.5f,
         0.5f, -0.5f
    };

    //��CPU���嶥�㻺����
    unsigned int buffer;
    glGenBuffers(1,&buffer);
    //�󶨶��㻺������֪�������������GL_ARRAY_BUFFER��
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    //����������GPU����
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
    //���ö�������(ָ�����㻺�����Ĳ��ֲ�����Щ���԰󶨵�����0)
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float)*2,0);
    glEnableVertexAttribArray(0);

    //��󶥵㻺����
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    
    //��д������ɫ��
    std::string vertexShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) in vec4 position;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "}\n";

    //��дƬ����ɫ��
    std::string fragmentShader =
        "#version 330 core\n"
        "\n"
        "layout(location = 0) out vec4 color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   color = vec4(1.0,0.0,0.0,1.0);\n"
        "}\n";

    //�����Զ��������ɫ������
    unsigned int shader = CreateShader(vertexShader,fragmentShader);
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

        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}