#include <glow/Array.hpp>
#include <glow/ShaderFile.h>
#include <glow/Error.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <Window.h>

Vertex::Vertex()
{
}

Vertex::Vertex(const glm::vec3& pos, const glm::vec2& texCoord)
: position(pos)
, texCoord0(texCoord)
{
}

Window::Window()
: shaderProgram(nullptr)
, vertexArrayObject(nullptr)
, texture(nullptr)
{
}

Window::~Window()
{
	delete shaderProgram;
	delete vertexArrayObject;
	delete texture;
}

void Window::initializeShaders(const std::string& applicationPath)
{
	std::string path = applicationPath.substr(0, applicationPath.rfind('/'));

	glow::ShaderFile* vertexShaderFile = new glow::ShaderFile(path+"/data/test.vert");
	glow::ShaderFile* fragmentShaderFile = new glow::ShaderFile(path+"/data/test.frag");

	glow::Shader* vertexShader = new glow::Shader(GL_VERTEX_SHADER);
	vertexShader->setSourceFile(vertexShaderFile, true);

	glow::Shader* fragmentShader = new glow::Shader(GL_FRAGMENT_SHADER);
	fragmentShader->setSourceFile(fragmentShaderFile, true);

	shaderProgram = new glow::Program();
	shaderProgram->attach(vertexShader);
	shaderProgram->attach(fragmentShader);
	shaderProgram->bindFragDataLocation(0, "outColor");
	shaderProgram->link();

	shaderProgram->setUniform("texture", 0);
}

void Window::initializeGL(const std::string& applicationPath)
{
	glClearColor(1,1,1,1);

	unsigned char img_data[16*4] = {
		255,0,0,100,
		255,255,0,255,
		255,0,255,255,
		255,0,100,255,
		0,0,80,255,
		255,0,0,255,
		255,255,0,0,
		255,0,255,255,
		255,0,100,255,
		0,0,80,255,
		255,0,0,255,
		255,255,0,0,
		255,0,255,255,
		255,0,100,255,
		0,0,80,255,
		0,0,80,255
	};

	texture = new glow::Texture(GL_TEXTURE_2D);

	texture->setParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	texture->setParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	texture->setParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	texture->setParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	texture->setParameter(GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	texture->image2D(0, GL_RGBA8, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data);

	initializeShaders(applicationPath);

	glow::Array<Vertex> vertexArray;

	vertexArray
		<< Vertex(glm::vec3(0,0,0), glm::vec2(0,0))
		<< Vertex(glm::vec3(1,0,0), glm::vec2(1,0))
		<< Vertex(glm::vec3(1,1,0), glm::vec2(1,1))
		<< Vertex(glm::vec3(0,1,0), glm::vec2(0,1));

	vertexArrayObject = new glow::VertexArrayObject();

	glow::VertexBuffer* vertices = vertexArrayObject->createVertexBuffer("vertices");
	vertices->setData(vertexArray);

	auto binding = vertexArrayObject->binding(shaderProgram->getAttributeLocation("position"));
	binding->setBuffer(vertices, 0, sizeof(Vertex));
	binding->setFormat(3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
	binding->enable();

	auto binding2 = vertexArrayObject->binding(shaderProgram->getAttributeLocation("texCoord0"));
	binding2->setBuffer(vertices, 0, sizeof(Vertex));
	binding2->setFormat(2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoord0));
	binding2->enable();
}

void Window::resizeGL(int width, int height)
{
	int side = std::min(width, height);
	glViewport((width - side) / 2, (height - side) / 2, side, side);

	projection = glm::mat4();
	modelView = glm::ortho(0, 1, 0, 1, 0, 1);

	shaderProgram->setUniform("modelView", modelView);
	shaderProgram->setUniform("projection", projection);
}

void Window::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	texture->bind();
	shaderProgram->use();

	vertexArrayObject->bind();
	vertexArrayObject->vertexBuffer("vertices")->drawArrays(GL_TRIANGLE_FAN, 0, 4);
	vertexArrayObject->unbind();

        shaderProgram->release();
	texture->unbind();
}