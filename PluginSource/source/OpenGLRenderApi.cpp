#include "OpenGLRenderApi.h";

static GLuint CreateShader(GLenum type, const char* sourceText)
{
	GLuint ret = glCreateShader(type);
	glShaderSource(ret, 1, &sourceText, NULL);
	glCompileShader(ret);

	GLint compileStatus;
	glGetShaderiv(ret, GL_COMPILE_STATUS, &compileStatus);

	assert(compileStatus == GL_TRUE);
	return ret;
}

OpenGLRenderApi::OpenGLRenderApi(UnityGfxRenderer apiType)
	: m_APIType(apiType)
{ }

OpenGLRenderApi::~OpenGLRenderApi() 
{ }

void OpenGLRenderApi::ProcessDeviceEvent(UnityGfxDeviceEventType type)
{
	if (type == kUnityGfxDeviceEventInitialize)
	{
		if (m_APIType == kUnityGfxRendererOpenGLCore)
			gl3wInit();
	}
	else if (type == kUnityGfxDeviceEventShutdown)
	{
		if (m_Program)
		{
			glDeleteProgram(m_Program);
			m_Program = 0;
		}
	}
}

void OpenGLRenderApi::CreateProgramm(const char* vertexShader, const char* fragmentShader)
{
	if (m_Program)
	{
		glDetachShader(m_Program, m_VertexShader);
		glDetachShader(m_Program, m_FragmentShader);
		glDeleteProgram(m_Program);
	}

	assert(glGetError() == GL_NO_ERROR);

	m_VertexShader = CreateShader(GL_VERTEX_SHADER, vertexShader);
	m_FragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentShader);

	m_Program = glCreateProgram();
	assert(m_Program != 0);

	glBindAttribLocation(m_Program, kVertexInputPosition, "pos");
	glBindAttribLocation(m_Program, kVertexInputColor, "color");
	glAttachShader(m_Program, m_VertexShader);
	glAttachShader(m_Program, m_FragmentShader);

	if (m_APIType == kUnityGfxRendererOpenGLCore)
		glBindFragDataLocation(m_Program, 0, "fragColor");

	glLinkProgram(m_Program);

	glDeleteShader(m_VertexShader);
	glDeleteShader(m_FragmentShader);

	GLint status = 0;
	glGetProgramiv(m_Program, GL_LINK_STATUS, &status);
	assert(status == GL_TRUE);

	m_UniformWorldMatrix = glGetUniformLocation(m_Program, "worldMatrix");
	m_UniformProjMatrix = glGetUniformLocation(m_Program, "projMatrix");

	glGenBuffers(1, &m_VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, 1024, NULL, GL_STREAM_DRAW);

	assert(glGetError() == GL_NO_ERROR);
}


void OpenGLRenderApi::DrawSimpleTriangles(const float worldMatrix[16], int triangleCount, const void* verticesFloat3Byte4)
{
	if (m_Program == 0)
		return;

	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	float projectionMatrix[16] = {
		1,0,0,0,
		0,1,0,0,
		0,0,2,0,
		0,0,-1,1,
	};

	glUseProgram(m_Program);
	glUniformMatrix4fv(m_UniformWorldMatrix, 1, GL_FALSE, worldMatrix);
	glUniformMatrix4fv(m_UniformProjMatrix, 1, GL_FALSE, projectionMatrix);

	if (m_APIType == kUnityGfxRendererOpenGLCore)
	{
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);
	}

	const int kVertexSize = 12 + 4;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferSubData(GL_ARRAY_BUFFER, 0, kVertexSize * triangleCount * 3, verticesFloat3Byte4);

	glEnableVertexAttribArray(kVertexInputPosition);
	glVertexAttribPointer(kVertexInputPosition, 3, GL_FLOAT, GL_FALSE, kVertexSize, (char*)NULL + 0);
	glEnableVertexAttribArray(kVertexInputColor);
	glVertexAttribPointer(kVertexInputColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, kVertexSize, (char*)NULL + 12);

	glDrawArrays(GL_TRIANGLES, 0, triangleCount * 3);

	if (m_APIType == kUnityGfxRendererOpenGLCore)
	{
		glDeleteVertexArrays(1, &m_VertexArray);
	}
}
