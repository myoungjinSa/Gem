#include "Renderer.h"
#include "Camera.h"
#include "Shader.h"
#include "GraphicsDevice.h"
#include "Object.h"
#include "Texture.h"
#include "Mesh.h"
#include "Logger.h"
#include <fstream>
#include <stdarg.h>


Renderer::Renderer()
	:drawPrimitive(ePrimitive::TRIANGLES)
{
}

Renderer::~Renderer() {

}

void Renderer::OutputLinkErrorMessage(GraphicsDevice& device, unsigned int programId) {
	char* pInfoLog = device.GetLinkerLog(programId);
	if (!pInfoLog)
		return;

	std::ofstream fout;
	fout.open("linker-error.txt");
	int logSize = strlen(pInfoLog);
	for (size_t iLog = 0; iLog < logSize; iLog++)
		fout << pInfoLog[iLog];

	fout.close();
	delete[] pInfoLog;

	//MessageBox(hWnd, L"Error compiling linker. Check linker-error.txt for message", L"Linker Error", MB_OK);
}

void Renderer::OutputShaderErrorMessage(GraphicsDevice& device, unsigned int shaderId, char* shaderFilename) {

	char* pInfoLog = device.GetShaderLog(shaderId);
	if (!pInfoLog)
		return;

	std::ofstream fout;
	//Open a file to write the error message to.
	fout.open("shader-error.txt");
	int logSize = strlen(pInfoLog);
	for (size_t iLog = 0; iLog < logSize; iLog++) {
		fout << pInfoLog[iLog];
	}

	fout.close();
	delete[] pInfoLog;

	//Convert the shader filename to a widechar string
	size_t convertedChars = 0;
	wchar_t newString[128];
	size_t error = mbstowcs_s(&convertedChars, newString, 128, shaderFilename, 128);
	if (error != 0)
		return;

	//MessageBox(hWnd, L"Error compile shader. Check shader-error.txt for message.", newString, MB_OK);
}


char* Renderer::LoadShaderSourceFile(const char* filename) {
	std::ifstream fin;

	//Open the shader source file.
	fin.open(filename);
	if (fin.fail())
		return 0;

	int fileSize = 0;

	//Read the first element of the file
	char input;
	fin.get(input);

	//Count the number of elements in the text file
	while (!fin.eof()) {
		fileSize++;
		fin.get(input);
	}
	//Close the file for now
	fin.close();

	//Initialize the buffer to read the shader source file into.
	char* buffer = new char[fileSize + 1];
	if (!buffer) {
		return 0;
	}

	fin.open(filename);
	fin.read(buffer, fileSize);
	fin.close();

	buffer[fileSize] = '\0';

	return buffer;
}

OpenGLRenderer::~OpenGLRenderer() {

}

bool OpenGLRenderer::Initialize(GraphicsDevice* pGraphicDevice) {
	Renderer::Initialize(pGraphicDevice);
	//Store a pointer to ther OpenGL class object.
	pDevice = dynamic_cast<OpenGL*>(pGraphicDevice);
	if (!pDevice)
		return false;

	return true;
}
void OpenGLRenderer::Shutdown(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader) {
	Renderer::Shutdown(shaderProgram, vertexShader, fragmentShader);

	glDetachShader(shaderProgram, vertexShader);
	glDetachShader(shaderProgram, fragmentShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glDeleteProgram(shaderProgram);
}

unsigned int OpenGLRenderer::CreateShader() {
	return glCreateProgram();
}

bool OpenGLRenderer::CreateRenderTarget(RenderTarget& renderTarget, const Size2u& screenSize, bool downSamplingTarget) {
	if (screenSize.Empty())
		return false;

	renderTarget.SetSize(screenSize);
	int iPreviousFrameBuffer = 0;

	if (renderTarget.IsNull()) {
		glGenFramebuffersEXT(1, &(renderTarget.iFrameBuffer));
		CheckError();
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, renderTarget.iFrameBuffer);
	CheckError();

	return true;
}

bool OpenGLRenderer::CreateTexture(Texture& texture, const Size2u& size, bool managed) {
	if (size.Empty())
		return false;

	if (texture.IsNull()) {
		AllocateTextures(texture.textureID, 1);
	}

	glBindTexture(GL_TEXTURE_2D, texture.textureID);
	CheckError();

	if (managed) {
		//texture.Initialize()
	}
	return true;
}

void OpenGLRenderer::DeleteTexture(Texture& texture) {
	glDeleteTextures(1, &texture.textureID);
}
void OpenGLRenderer::SetShader(unsigned int shaderPrgram) {
	glUseProgram(shaderPrgram);
}

bool OpenGLRenderer::CompileVertexShader(const char* vsFilename, unsigned int& vertexShader) {
	const char* vertexShaderBuffer;
	vertexShaderBuffer = LoadShaderSourceFile(vsFilename);
	if (!vertexShaderBuffer)
	{
		return false;
	}
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderBuffer, NULL);
	delete vertexShaderBuffer;
	vertexShaderBuffer = nullptr;

	int status = 0;
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
	if (status != 1) {
		// If it did not compile then write the syntax error message out to a text file for review.
		OutputShaderErrorMessage(*pDevice, vertexShader, const_cast<char*>(vsFilename));
		return false;
	}
	return true;
}

bool OpenGLRenderer::CompileFragmentShader(const char* fsFilename, unsigned int& fragmentShader) {
	const char* fragmentShaderBuffer;
	fragmentShaderBuffer = LoadShaderSourceFile(fsFilename);
	if (!fragmentShaderBuffer)
	{
		return false;
	}
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderBuffer, NULL);
	delete fragmentShaderBuffer;
	fragmentShaderBuffer = nullptr;

	int status = 0;
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
	if (status != 1) {
		// If it did not compile then write the syntax error message out to a text file for review.
		OutputShaderErrorMessage(*pDevice, fragmentShader, const_cast<char*>(fsFilename));
		return false;
	}
	return true;
}

bool OpenGLRenderer::CompileGeometryShader(const char* gsFilename, unsigned int& geometryShader) {
	const char* geometryShaderBuffer;
	geometryShaderBuffer = LoadShaderSourceFile(gsFilename);
	if (!geometryShaderBuffer)
		return false;

	geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometryShader, 1, &geometryShaderBuffer, NULL);
	delete geometryShaderBuffer;
	geometryShaderBuffer = nullptr;

	int status = 0;
	glCompileShader(geometryShader);
	glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &status);
	if (status != 1) {
		OutputShaderErrorMessage(*pDevice, geometryShader, const_cast<char*>(gsFilename));
		return false;
	}

	return true;
}


bool OpenGLRenderer::BindVertexAttrib(unsigned int shaderProgram, unsigned int vertexShader, unsigned int fragmentShader, int vertexArgs, ...) {
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	va_list ap;
	va_start(ap, vertexArgs);
	String attrib;
	for (size_t iAttrib = 0; iAttrib < vertexArgs; iAttrib++) {
		attrib = va_arg(ap, String);

		glBindAttribLocation(shaderProgram, iAttrib, attrib.c_str());
	}
	glLinkProgram(shaderProgram);

	GLint status;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		OutputLinkErrorMessage(*pDevice, shaderProgram);
		return false;
	}
	return true;
}

bool OpenGLRenderer::BindVertexAttrib(unsigned int shaderProgram, unsigned int vertexShader, unsigned int geometryShader, unsigned int fragmentShader, int vertexArgs, ...) {
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, geometryShader);
	glAttachShader(shaderProgram, fragmentShader);

	va_list ap;
	va_start(ap, vertexArgs);
	String attrib;
	for (size_t iAttrib = 0; iAttrib < vertexArgs; iAttrib++) {
		attrib = va_arg(ap, String);

		glBindAttribLocation(shaderProgram, iAttrib, attrib.c_str());
	}
	glLinkProgram(shaderProgram);

	GLint status;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		OutputLinkErrorMessage(*pDevice, shaderProgram);
		return false;
	}
	return true;
}

void OpenGLRenderer::SetWindingOrder(Renderer::eWindingOrder order) {
	switch (order) {
	case eWindingOrder::CW:
		glFrontFace(GL_CW);
		break;
	case eWindingOrder::CCW:
		glFrontFace(GL_CCW);
		break;
	}
}

void OpenGLRenderer::SetCullFace(eCullFace _face) {
	int face = GetCullFace(_face);
	glCullFace(face);
}

void OpenGLRenderer::SetDepthTest(bool bEnable) {
	bEnable ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, Matrix<float, 4, 4>& matrix, String variableName) {
	unsigned int location = glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	glUniformMatrix4fv(location, 1, false, (float*)&matrix.value);
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, const Matrix<float, 4, 4>& matrix, String variableName) {
	unsigned int location = glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	glUniformMatrix4fv(location, 1, false, (float*)&matrix.value);
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, const Matrix<float, 3, 3>& matrix, String variableName) {
	unsigned int location = glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	glUniformMatrix3fv(location, 1, false, (float*)&matrix.value);
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, Matrix<float, 3, 3>& matrix, String variableName) {
	unsigned int location = glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	glUniformMatrix3fv(location, 1, false, (float*)&matrix.value);
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, const Vec3f& vec3, String variableName) {
	unsigned int location = glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	glUniform3fv(location, 1, (float*)const_cast<Vec3f&>(vec3).ConvertToValue());
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, Vec3f& vec3, String variableName) {
	unsigned int location = glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	glUniform3fv(location, 1, (float*)vec3.ConvertToValue());
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, const Vec4f& vec4, String variableName) {
	unsigned int location = glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	glUniform4fv(location, 1, (float*)const_cast<Vec4f&>(vec4).ConvertToValue());
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, Vec4f& vec4, String variableName) {
	unsigned int location = glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	glUniform4fv(location, 1, (float*)vec4.ConvertToValue());
	return true;
}

bool OpenGLRenderer::SetShaderParameter(unsigned int shaderProgram, int integer, String variableName) {
	unsigned int location = glGetUniformLocation(shaderProgram, variableName.c_str());
	if (location == -1)
		return false;

	glUniform1i(location, integer);
	return true;
}

bool OpenGLRenderer::AllocateVertexBuffer(unsigned int& vertexArrayId, unsigned int& vertexBufferId, void* vertexData, VertexBufferBindCallback* pBindFunction, unsigned int numVertices, unsigned int sizeofVertex) {
	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);
	glGenBuffers(1, &vertexBufferId);


	pBindFunction(vertexData, vertexBufferId, numVertices, sizeofVertex);
	return true;
}


bool OpenGLRenderer::AllocateIndexBuffer(unsigned int& indexBufferId, size_t indexCount, unsigned int* indexData) {
	if (indexCount == 0)
		return false;

	glGenBuffers(1, &indexBufferId);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indexData, GL_STATIC_DRAW);
	return true;
}

bool OpenGLRenderer::AllocateFrameBuffer(unsigned int& colorBuffer, unsigned int& depthBuffer, unsigned int& fbo) {
	glGenFramebuffersEXT(1, &fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER, fbo);

	return true;
}

void OpenGLRenderer::CheckError(){
	GLenum error(glGetError());

	while (error != GL_NO_ERROR) {

		switch (error) {
		case GL_INVALID_OPERATION:				LogError(L"GL-INVALID_OPERATION");	break;
		case GL_INVALID_ENUM:					LogError(L"GL-INVALID_ENUM");		break;
		case GL_INVALID_VALUE:					LogError(L"GL-INVALID_VALUD");		break;
		case GL_OUT_OF_MEMORY:					LogError(L"GL-OUT_OF_MEMORY");		break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:	LogError(L"GL-INVALID_FRAMEBUFFER_OPERATION"); break;
		}
		error = glGetError();
	}
}
bool OpenGLRenderer::AllocateTextures(unsigned int& textureId, unsigned int textureCount) {
	//Generate an ID for the texture.
	glGenTextures(textureCount, &textureId);

	CheckError();
	return true;
}


void OpenGLRenderer::SetSampleMode(bool bCubemap) {
	if (bCubemap) {
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
}

void OpenGLRenderer::SetFiltering(bool bCubemap) {
	glTexParameteri(bCubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(bCubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//pGL->glGenerateMipmap(bCubemap ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D);
}
void OpenGLRenderer::BindTexture(unsigned int textureId) {
	//glEnable(GL_TEXTURE_2D);
	////Set the unique texture unit in which to store the data
	glActiveTexture(GL_TEXTURE0 + textureId);

	glBindTexture(GL_TEXTURE_2D, textureId);
}

void OpenGLRenderer::BindCubemapTexture(unsigned int textureId) {
	glActiveTexture(GL_TEXTURE0 + textureId);

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
}

void OpenGLRenderer::SetImage(unsigned int target, void* pImage, unsigned int width, unsigned int height) {
	glTexImage2D(target, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, pImage);
}

void OpenGLRenderer::DisableVertexAttribArray(size_t vertexAttribCount) {
	for (size_t iVertexAttrib = 0; iVertexAttrib < vertexAttribCount; iVertexAttrib++) {
		glDisableVertexAttribArray(iVertexAttrib);
	}
}


void OpenGLRenderer::ReleaseVertexArray(unsigned int& vertexArrayId, size_t arrayCount) {
	glBindVertexArray(0);
	glDeleteVertexArrays(arrayCount, &vertexArrayId);
}
void OpenGLRenderer::ReleaseVertexBuffers(unsigned int& vertexBufferId, size_t bufferCount) {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(bufferCount, &vertexBufferId);
}
void OpenGLRenderer::ReleaseIndexBuffers(unsigned int& indexBufferId, size_t indexCount) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(indexCount, &indexBufferId);
}

void OpenGLRenderer::DrawVertexBuffer(unsigned int vertexArrayId, size_t startPos, size_t vertexCount) {
	glBindVertexArray(vertexArrayId);

	switch (drawPrimitive) {
	case ePrimitive::TRIANGLE_STRIP:
		glDrawArrays(GL_TRIANGLE_STRIP, startPos, vertexCount);
		break;
	case ePrimitive::TRIANGLES:
		glDrawArrays(GL_TRIANGLES, startPos, vertexCount);
		break;
	case ePrimitive::LINES:
		glDrawArrays(GL_LINES, startPos, vertexCount);
		break;
	}
}

void OpenGLRenderer::DrawIndexBuffer(unsigned int vertexArrayId, size_t indexCount) {
	glBindVertexArray(vertexArrayId);

	switch (drawPrimitive) {
	case ePrimitive::TRIANGLE_STRIP:
		glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
		break;
	case ePrimitive::TRIANGLES:
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		break;
	case ePrimitive::LINES:
		glDrawElements(GL_LINES, indexCount, GL_UNSIGNED_INT, 0);
		break;
	}
}

int OpenGLRenderer::ClearStencilBuffer(int clearVal) {
	int ret = 0;
	glClearBufferiv(GL_STENCIL, 0, &ret);
	return ret;
}

int OpenGLRenderer::GetCullFace(eCullFace _face)const {
	int face = GL_BACK;

	switch (_face) {
	case eCullFace::Back:    face = GL_BACK; break;
	case eCullFace::Front:   face = GL_FRONT; break;
	case eCullFace::FrontAndBack: face = GL_FRONT_AND_BACK; break;
	}

	return face;
}
int OpenGLRenderer::GetDepthCompare(eCompare _depthCmp) const {
	int depthCmp = -1;
	switch (_depthCmp) {
	case eCompare::ALWAYS:	depthCmp = GL_ALWAYS;		break;
	case eCompare::NEVER:	depthCmp = GL_NEVER;			break;
	case eCompare::LESS:	depthCmp = GL_LESS;			break;
	case eCompare::LEQUAL:	depthCmp = GL_LEQUAL;		break;
	case eCompare::EQUAL:	depthCmp = GL_EQUAL;			break;
	case eCompare::NOTEQUAL: depthCmp = GL_NOTEQUAL;		break;
	case eCompare::GREATER:	depthCmp = GL_GREATER;		break;
	case eCompare::GEQUAL:	depthCmp = GL_GEQUAL;		break;
	default:
		break;
	}
	return depthCmp;
}

int OpenGLRenderer::GetStencilCompare(eStencilOp _stencilOp) const {
	int stencilOp = -1;
	switch (_stencilOp) {
	case eStencilOp::KEEP:			stencilOp = GL_KEEP;		break;
	case eStencilOp::ZERO:			stencilOp = GL_ZERO;		break;
	case eStencilOp::REPLACE:		stencilOp = GL_REPLACE;		break;
	case eStencilOp::INCREMENT:		stencilOp = GL_INCR;		break;
	case eStencilOp::DECREMENT:		stencilOp = GL_DECR;		break;
	case eStencilOp::INVERT:		stencilOp = GL_INVERT;		break;
	case eStencilOp::INCREMENT_WRAP: stencilOp = GL_INCR_WRAP;	break;
	case eStencilOp::DECREMENT_WRAP: stencilOp = GL_DECR_WRAP;	break;
	default:
		break;
	}
	return stencilOp;
}


void OpenGLRenderer::OperateAfterStencilTest(int face, eStencilOp stencilFailed, eStencilOp depthFailed, eStencilOp depthPassed) {
	int sFailed = GetStencilCompare(stencilFailed);
	int dFailed = GetStencilCompare(depthFailed);
	int dPassed = GetStencilCompare(depthPassed);

	glStencilOpSeparate(face, sFailed, dFailed, dPassed);
}

void OpenGLRenderer::SetStencilTest(int face, eCompare _stencilFunc, int ref, unsigned int mask) {
	int stencilFunc = GetDepthCompare(_stencilFunc);
	glStencilFuncSeparate(face, stencilFunc, ref, mask);
}

void OpenGLRenderer::SetDepthFunc(eCompare _depthCmp) {
	int depthFunc = GetDepthCompare(_depthCmp);
	glDepthFunc(depthFunc);
}


void OpenGLRenderer::EnableCulling(bool bEnable) {
	bEnable == true ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

bool OpenGLRenderer::BeginRender() {
	pDevice->ClearWindow(0.0f, 0.0f, 0.0f, 1.0f);
	return true;
}

void OpenGLRenderer::EndRender() {
	pDevice->SwapBuffers();
}

