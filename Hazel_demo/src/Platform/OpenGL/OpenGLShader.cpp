#include "hzpch.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

namespace Hazel {

    static GLenum ShaderTypeFromString(const std::string& type)//���Ͷ�ȡTODO: change to switch case
    {
        if (type == "vertex")
            return GL_VERTEX_SHADER;//������ɫ��
        if (type == "fragment" || type == "pixel")
            return GL_FRAGMENT_SHADER;//Ƭ����ɫ��

        HZ_CORE_ASSERT(false, "Unknown shader type!");//δ֪��ɫ������
        return 0;
    }

    OpenGLShader::OpenGLShader(const std::string& filePath)//���ļ�������ɫ��
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        std::string source = ReadFile(filePath);//��ȡ�ļ�
        auto shaderSources = PreProcess(source);//Ԥ����
        Compile(shaderSources);//����

        //���ļ�������ȡ��ɫ������
        auto lastSlash = filePath.find_last_of("/\\");//�������һ��б��
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;//���û��б�ܣ���0��ʼ�������б�ܺ�ʼ
        auto lastDot = filePath.rfind('.');//�������һ����
        auto count = lastDot == std::string::npos ? filePath.size() - lastSlash : lastDot - lastSlash;//���û�е㣬��б�ܺ�ʼ������ӵ�ǰ��ʼ
        m_Name = filePath.substr(lastSlash, count);//��ȡ����
    }

    OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)//��Դ�������ɫ��
        : m_Name(name)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        std::unordered_map<GLenum, std::string> sources;//��ɫ��Դ��
        sources[GL_VERTEX_SHADER] = vertexSrc;//������ɫ��
        sources[GL_FRAGMENT_SHADER] = fragmentSrc;//Ƭ����ɫ��
        Compile(sources);//����
    }

    OpenGLShader::~OpenGLShader()
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        glDeleteProgram(m_RendererID);
    }

    std::string OpenGLShader::ReadFile(const std::string& filepath)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        std::string result;//���
        std::ifstream in(filepath, std::ios::in | std::ios::binary);//���ļ�
        if (in)//����򿪳ɹ�
        {
            in.seekg(0, std::ios::end);//��λ���ļ�ĩβ
            size_t size = in.tellg();//��ȡ�ļ���С
            if (size != 1)
            {
                result.resize(size);//���������С
                in.seekg(0, std::ios::beg);//��λ���ļ���ͷ
                in.read(&result[0], size);//��ȡ�ļ�
                in.close();//�ر��ļ�
            }
            else
            {
                HZ_CORE_ASSERT(false, "Could not read from file '{0}'", filepath);//�������
            }
        }
        else//�����ʧ��
        {
            HZ_CORE_ERROR("Could not open file '{0}'", filepath);//�������
        }

        return result;//���ؽ��
    }

    std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        std::unordered_map<GLenum, std::string> shaderSources;//��ɫ��Դ��

        const char* typeToken = "#type";//���ͱ��
        size_t typeTokenLength = strlen(typeToken);//���ͱ�ǳ���
        size_t pos = source.find(typeToken, 0);//�������ͱ��
        while (pos != std::string::npos)//npos ��һ��������������ʾ�����ڵ�λ��,string::npos�����ַ�����ͷ�˽�����
        {
            size_t eol = source.find_first_of("\r\n", pos);//���һ��з�
            HZ_CORE_ASSERT(eol != std::string::npos, "Syntax error");//������з������ַ�����β���������
            size_t begin = pos + typeTokenLength + 1;//��ʼλ��(��"#type"֮��)
            std::string type = source.substr(begin, eol - begin);//��ȡ����
            HZ_CORE_ASSERT(ShaderTypeFromString(type), "Invalid shader type specified");//���������Ч���������

            size_t nextLinePos = source.find_first_not_of("\r\n", eol); //shader����Ŀ�ʼλ��(λ��shader type����������һ��)
            HZ_CORE_ASSERT(nextLinePos != std::string::npos, "Syntax error");//�����ʼλ�ò����ַ�����β���������
            pos = source.find(typeToken, nextLinePos);//Start of next shader type declaration line
            shaderSources[ShaderTypeFromString(type)] = (pos == std::string::npos) ? source.substr(nextLinePos) : source.substr(nextLinePos, pos - nextLinePos);//shader code ��ȡ
        }
        return shaderSources;
    }

    void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        GLint program = glCreateProgram();//������ɫ������
        HZ_CORE_ASSERT(shaderSources.size() <= 2, "We only support 2 shaders for now");//�����ɫ����������2���������
        std::array<GLenum, 2> glShaderIDs;//��ɫ��ID����
        int glShaderIDIndex = 0;//��ɫ��ID��������
        for (auto& kv : shaderSources)
        {
            GLenum type = kv.first;//��ɫ������
            const std::string& source = kv.second;//��ɫ��Դ��
            GLuint shader = glCreateShader(type);//������ɫ��
            const GLchar* sourceCStr = source.c_str();//������ɫ��Դ���ַ������׵�ַ
            glShaderSource(shader, 1, &sourceCStr, 0);//������ɫ��Դ��

            glCompileShader(shader);//������ɫ��

            GLint isCompiled = 0;//�Ƿ����ɹ�
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);//��ȡ����״̬
            if (isCompiled == GL_FALSE)//�������ʧ��
            {
                GLint maxLength = 0;//��󳤶�
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);//��ȡ��־����

                std::vector<GLchar> infoLog(maxLength);//��־
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);//��ȡ��־

                glDeleteShader(shader);//ɾ����ɫ��

                HZ_CORE_ERROR("{0}", infoLog.data());//�������
                HZ_CORE_ASSERT(false, "Shader compilation failure!");//������ɫ������ʧ��
                break;
            }

            glAttachShader(program, shader);//������ɫ��
            glShaderIDs[glShaderIDIndex++] = shader;//������ɫ��ID
        }

        m_RendererID = program;//������ɫ������ID

        //Link our program
        glLinkProgram(program);//������ɫ������

        //Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;//�Ƿ����ӳɹ�
        glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);//��ȡ����״̬
        if (isLinked == GL_FALSE)//�������ʧ��
        {
            GLint maxLength = 0;//��󳤶�
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);//��ȡ��־����

            //maxLength ����NULL�ַ� 
            std::vector<GLchar> infoLog(maxLength);//��־
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);//��ȡ��־

            //We don't need the program anymore.
            glDeleteProgram(program);//ɾ����ɫ������

            for (auto id : glShaderIDs)//������ɫ��ID
                glDeleteShader(id);//ɾ����ɫ��

            HZ_CORE_ERROR("{0}", infoLog.data());//�������
            HZ_CORE_ASSERT(false, "Shader link failure!");//������ɫ������ʧ��
            return;
        }

        for (auto id : glShaderIDs)//������ɫ��ID
        {
            glDetachShader(program, id);//������ɫ��
            glDeleteShader(id);//ɾ����ɫ��
        }
    }



    void OpenGLShader::Bind() const
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        glUseProgram(m_RendererID);
    }

    void OpenGLShader::Unbind() const
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        glUseProgram(0);
    }

    void OpenGLShader::SetInt(const std::string& name, int value)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        UploadUniformInt(name, value);//�ϴ�����,����uniform
    }

    void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        UploadUniformFloat3(name, value);//�ϴ�������,����uniform
    }

    void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        UploadUniformFloat4(name, value);//�ϴ�������,����uniform
    }

    void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
    {
        HZ_PROFILE_FUNCTION();//��ȡ����ǩ��

        UploadUniformMat4(name, value);//�ϴ�����,����uniform
    }

    void OpenGLShader::UploadUniformInt(const std::string& name, int value)
    {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());//��ȡuniformλ��
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