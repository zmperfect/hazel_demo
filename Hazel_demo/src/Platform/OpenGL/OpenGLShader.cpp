#include "hzpch.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace Hazel {

    static GLenum ShaderTypeFromString(const std::string& type)//类型读取TODO: change to switch case
    {
        if (type == "vertex")
            return GL_VERTEX_SHADER;//顶点着色器
        if (type == "fragment" || type == "pixel")
            return GL_FRAGMENT_SHADER;//片段着色器

        HZ_CORE_ASSERT(false, "Unknown shader type!");//未知着色器类型
        return 0;
    }

    OpenGLShader::OpenGLShader(const std::string& filePath)//从文件加载着色器
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        std::string source = ReadFile(filePath);//读取文件
        auto shaderSources = PreProcess(source);//预处理
        Compile(shaderSources);//编译

        //从文件名中提取着色器名称
        auto lastSlash = filePath.find_last_of("/\\");//查找最后一个斜杠
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;//如果没有斜杠，从0开始，否则从斜杠后开始
        auto lastDot = filePath.rfind('.');//查找最后一个点
        auto count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;//如果没有点，从斜杠后开始，否则从点前开始
        m_Name = filePath.substr(lastSlash, count);//获取名称
    }

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)//从源码加载着色器
        : m_Name(name)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        std::unordered_map<GLenum, std::string> sources;//着色器源码
        sources[GL_VERTEX_SHADER] = vertexSrc;//顶点着色器
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;//片段着色器
        Compile(sources);//编译
    }

    OpenGLShader::~OpenGLShader()
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        glDeleteProgram(m_RendererID);
    }

    std::string OpenGLShader::ReadFile(const std::string& filepath)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        std::string result;//结果
        std::ifstream in(filepath, std::ios::in | std::ios::binary);//打开文件
        if (in)//如果打开成功
        {
            in.seekg(0, std::ios::end);//定位到文件末尾
            size_t size = in.tellg();//获取文件大小
            if (size != 1)
            {
                result.resize(size);//调整结果大小
                in.seekg(0, std::ios::beg);//定位到文件开头
                in.read(&result[0], size);//读取文件
                in.close();//关闭文件
            }
            else
            {
                HZ_CORE_ASSERT(false, "Could not read from file '{0}'", filepath);//输出错误
            }
        }
        else//如果打开失败
        {
            HZ_CORE_ERROR("Could not open file '{0}'", filepath);//输出错误
        }

        return result;//返回结果
    }

    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        std::unordered_map<GLenum, std::string> shaderSources;//着色器源码

        const char* typeToken = "#type";//类型标记
        size_t typeTokenLength = strlen(typeToken);//类型标记长度
        size_t pos = source.find(typeToken, 0);//查找类型标记
        while (pos != std::string::npos)//npos 是一个常数，用来表示不存在的位置,string::npos代表字符串到头了结束了
        {
            size_t eol = source.find_first_of("\r\n", pos);//查找换行符
            HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");//如果换行符不是字符串结尾，输出错误
            size_t begin = pos + typeTokenLength + 1;//开始位置(在"#type"之后)
            std::string type = source.substr(begin, eol - begin);//获取类型
            HZ_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");//如果类型无效，输出错误

            size_t nextLinePos = source.find_first_not_of("\r\n", eol); //shader代码的开始位置(位于shader type描述符的下一行)
            HZ_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");//如果开始位置不是字符串结尾，输出错误
            pos = source.find(typeToken, nextLinePos);//Start of next shader type declaration line
            shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);//shader code 获取
        }
        return shaderSources;
    }

    void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        GLint program = glCreateProgram();//创建着色器程序
        HZ_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now");//如果着色器数量大于2，输出错误
        std::array<GLenum, 2> glShaderIDs;//着色器ID数组
        int glShaderIDIndex = 0;//着色器ID数组索引
        for (auto& kv : shaderSources)
        {
            GLenum type = kv.first;//着色器类型
            const std::string& source = kv.second;//着色器源码
            GLuint shader = glCreateShader(type);//创建着色器
            const GLchar* sourceCStr = source.c_str();//返回着色器源码字符串的首地址
            glShaderSource(shader, 1, &sourceCStr, 0);//设置着色器源码

            glCompileShader(shader);//编译着色器

            GLint isCompiled = 0;//是否编译成功
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);//获取编译状态
            if (isCompiled == GL_FALSE)//如果编译失败
            {
                GLint maxLength = 0;//最大长度
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);//获取日志长度

                std::vector<GLchar> infoLog(maxLength);//日志
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);//获取日志

                glDeleteShader(shader);//删除着色器

                HZ_CORE_ERROR("{0}", infoLog.data());//输出错误
                HZ_CORE_ASSERT(false, "Shader compilation failure!");//断言着色器编译失败
                break;
            }

            glAttachShader(program, shader);//附加着色器
            glShaderIDs[glShaderIDIndex++] = shader;//设置着色器ID
        }

        m_RendererID = program;//设置着色器程序ID

        //Link our program
        glLinkProgram(program);//链接着色器程序

        //Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;//是否链接成功
        glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);//获取链接状态
        if (isLinked == GL_FALSE)//如果链接失败
        {
            GLint maxLength = 0;//最大长度
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);//获取日志长度

            //maxLength 包含NULL字符 
            std::vector<GLchar> infoLog(maxLength);//日志
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);//获取日志

            //We don't need the program anymore.
            glDeleteProgram(program);//删除着色器程序

            for (auto id : glShaderIDs)//遍历着色器ID
                glDeleteShader(id);//删除着色器

            HZ_CORE_ERROR("{0}", infoLog.data());//输出错误
            HZ_CORE_ASSERT(false, "Shader link failure!");//断言着色器链接失败
            return;
        }

        for (auto id : glShaderIDs)//遍历着色器ID
        {
            glDetachShader(program, id);//分离着色器
            glDeleteShader(id);//删除着色器
        }
    }



    void OpenGLShader::Bind() const
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        glUseProgram(0);
    }

    void OpenGLShader::SetInt(const std::string& name, int value)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        UploadUniformInt(name, value);//上传整型,设置uniform
    }

    void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        UploadUniformFloat3(name, value);//上传浮点型,设置uniform
    }

    void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        UploadUniformFloat4(name, value);//上传浮点型,设置uniform
    }

    void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
    {
        HZ_PROFILE_FUNCTION();//获取函数签名

        UploadUniformMat4(name, value);//上传矩阵,设置uniform
    }

    void OpenGLShader::UploadUniformInt(const std::string& name, int value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());//获取uniform位置
        glUniform1i(location, value);
    }

    void OpenGLShader::UploadUniformFloat(const std::string& name, float value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(location, value);
    }

    void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform2f(location, value.x, value.y);
    }

    void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

    void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
    }

}