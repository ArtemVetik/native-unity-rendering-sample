#include "Unity/IUnityGraphics.h"
#include "OpenGLRenderApi.h"

#include <assert.h>
#include <math.h>
#include <vector>

static OpenGLRenderApi* s_CurrentAPI = NULL;
static UnityGfxRenderer s_DeviceType = kUnityGfxRendererNull;

static IUnityInterfaces* s_UnityInterfaces = NULL;
static IUnityGraphics* s_Graphics = NULL;

static const char* vertShader;
static const char* fragShader;

static void UNITY_INTERFACE_API OnGraphicsDeviceEvent(UnityGfxDeviceEventType eventType)
{
	if (eventType == kUnityGfxDeviceEventInitialize)
	{
		assert(s_CurrentAPI == NULL);
		s_DeviceType = s_Graphics->GetRenderer();
		s_CurrentAPI = new OpenGLRenderApi(s_DeviceType);
	}

	if (s_CurrentAPI)
	{
		s_CurrentAPI->ProcessDeviceEvent(eventType);
	}

	if (eventType == kUnityGfxDeviceEventShutdown)
	{
		delete s_CurrentAPI;
		s_CurrentAPI = NULL;
		s_DeviceType = kUnityGfxRendererNull;
	}
}

extern "C" void	UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginLoad(IUnityInterfaces * unityInterfaces)
{
	s_UnityInterfaces = unityInterfaces;
	s_Graphics = s_UnityInterfaces->Get<IUnityGraphics>();
	s_Graphics->RegisterDeviceEventCallback(OnGraphicsDeviceEvent);

	OnGraphicsDeviceEvent(kUnityGfxDeviceEventInitialize);
	assert(glGetError() == GL_NO_ERROR);
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API UnityPluginUnload()
{
	s_Graphics->UnregisterDeviceEventCallback(OnGraphicsDeviceEvent);
}

static void DrawColoredTriangle()
{
	assert(glGetError() == GL_NO_ERROR);
	struct MyVertex
	{
		float x, y, z;
		unsigned int color;
	};

	MyVertex verts[4] =
	{
		{ -0.5f, -0.25f,  0, 0xFFff0000 },
		{ 0.5f, -0.25f,  0, 0xFF00ff00 },
		{ 0,     0.5f ,  0, 0xFF0000ff },
	};

	float worldMatrix[16] = {
		0,-1,0,0,
		1,0,0,0,
		0,0,1,0,
		0,0,0.7f,1,
	};

	s_CurrentAPI->DrawSimpleTriangles(worldMatrix, 1, verts);
}

static void SetShadersFunc(const char* vertexShader, const char* fragmentShader)
{
	if (s_CurrentAPI)
	{
		s_CurrentAPI->CreateProgramm(vertexShader, fragmentShader);
	}
}

static void UNITY_INTERFACE_API OnRenderEvent(int eventID)
{
	if (s_CurrentAPI == NULL)
		return;

	if (eventID == 1) 
	{
		DrawColoredTriangle();
	}
	else if (eventID == 2)
	{
		SetShadersFunc(vertShader, fragShader);
	}
}

extern "C" UnityRenderingEvent UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API GetRenderEventFunc()
{
	return OnRenderEvent;
}

extern "C" void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API SetShaders(const char* vert, const char* frag) 
{
	vertShader = vert; 
	fragShader = frag; 
}
