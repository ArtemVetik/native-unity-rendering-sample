#include "Unity/IUnityGraphics.h"

#include <assert.h>
#include "gl3w/gl3w.h"
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>

class OpenGLRenderApi
{
public:
	OpenGLRenderApi(UnityGfxRenderer apiType);
	~OpenGLRenderApi();

	void ProcessDeviceEvent(UnityGfxDeviceEventType type);
	void CreateProgramm(const char* vertexShader, const char* fragmentShader);
	void DrawSimpleTriangles(const float worldMatrix[16], int triangleCount, const void* verticesFloat3Byte4);
private:
	UnityGfxRenderer m_APIType;
	GLuint m_VertexShader;
	GLuint m_FragmentShader;
	GLuint m_Program;
	GLuint m_VertexArray;
	GLuint m_VertexBuffer;
	int m_UniformWorldMatrix;
	int m_UniformProjMatrix;
};

enum VertexInputs
{
	kVertexInputPosition = 0,
	kVertexInputColor = 1
};