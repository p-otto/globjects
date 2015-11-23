
#include <chrono>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include <glbinding/gl/gl.h>
#include <glbinding/ContextInfo.h>
#include <glbinding/Version.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <globjects/globjects.h>
#include <globjects/logging.h>

#include <globjects/Uniform.h>
#include <globjects/Program.h>
#include <globjects/Shader.h>
#include <globjects/Buffer.h>
#include <globjects/VertexArray.h>
#include <globjects/VertexAttributeBinding.h>
#include <globjects/TransformFeedback.h>


using namespace gl;
using namespace glm;
using namespace globjects;


void key_callback(GLFWwindow * window, int key, int /*scancode*/, int action, int /*modes*/)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE)
        glfwSetWindowShouldClose(window, true);
}

void draw(VertexArray * vao, Program * transformFeedbackProgram, TransformFeedback * transformFeedback, Program * shaderProgram, Buffer * vertexBuffer1, Buffer * vertexBuffer2, const std::chrono::high_resolution_clock::time_point & t_start)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const auto t_elapsed = std::chrono::high_resolution_clock::now() - t_start;

    Buffer * drawBuffer  = vertexBuffer1;
    Buffer * writeBuffer = vertexBuffer2;

    vao->bind();

    transformFeedbackProgram->setUniform("deltaT", static_cast<float>(t_elapsed.count()) * float(std::nano::num) / float(std::nano::den));

    vao->binding(0)->setBuffer(drawBuffer, 0, sizeof(vec4));

    transformFeedback->bind();
    writeBuffer->bindBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

    glEnable(GL_RASTERIZER_DISCARD);

    transformFeedbackProgram->use();
    transformFeedback->begin(GL_TRIANGLES);
    vao->drawArrays(GL_TRIANGLES, 0, 6);
    transformFeedback->end();
    glDisable(GL_RASTERIZER_DISCARD);

    transformFeedback->unbind();

    vao->binding(0)->setBuffer(writeBuffer, 0, sizeof(vec4));

    shaderProgram->use();
    transformFeedback->draw(GL_TRIANGLE_STRIP);
    shaderProgram->release();

    vao->unbind();

    std::swap(vertexBuffer1, vertexBuffer2);
}


int main(int /*argc*/, char * /*argv*/[])
{
    // Initialize GLFW with error callback and needed OpenGL version window hint
    glfwInit();
    glfwSetErrorCallback( [] (int /*error*/, const char * description) { puts(description); } );
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create a context and, if valid, make it current
    GLFWwindow * window = glfwCreateWindow(1024, 768, "", NULL, NULL);
    if (window == nullptr)
    {
        critical() << "Context creation failed. Terminate execution.";

        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);

    // Create callback that when user presses ESC, the context should be destroyed and window closed
    glfwSetKeyCallback(window, key_callback);

    // Initialize globjects (internally initializes glbinding, and registers the current context)
    globjects::init();

    // Dump information about context and graphics card
    info() << std::endl
        << "OpenGL Version:  " << glbinding::ContextInfo::version() << std::endl
        << "OpenGL Vendor:   " << glbinding::ContextInfo::vendor() << std::endl
        << "OpenGL Renderer: " << glbinding::ContextInfo::renderer() << std::endl;

    if (!hasExtension(GLextension::GL_ARB_transform_feedback3))
    {
        critical() << "Transform feedback not supported.";

        glfwTerminate();
        return 1;
    }

    glClearColor(0.2f, 0.3f, 0.4f, 1.f);

    {
        // Initialize
        // Create and setup shader
        ref_ptr<Program> shaderProgram = new Program();
        shaderProgram->attach(
            Shader::fromFile(GL_VERTEX_SHADER,   "data/transformfeedback/simple.vert")
          , Shader::fromFile(GL_FRAGMENT_SHADER, "data/transformfeedback/simple.frag"));

        ref_ptr<Program> transformFeedbackProgram = new Program();
        transformFeedbackProgram->attach(
            Shader::fromFile(GL_VERTEX_SHADER, "data/transformfeedback/transformfeedback.vert"));

        transformFeedbackProgram->setUniform("deltaT", 0.0f);


        const int width = 1024;
        const int height = 768;
        const int side = std::min<int>(width, height);
        glViewport((width - side) / 2, (height - side) / 2, side, side);
        shaderProgram->setUniform("modelView", mat4());
        shaderProgram->setUniform("projection", ortho(-0.4f, 1.4f, -0.4f, 1.4f, 0.f, 1.f));


        // Create and setup geometry
        auto vertexArray = std::vector<vec4>({
            vec4(0, 0, 0, 1)
          , vec4(1, 0, 0, 1)
          , vec4(0, 1, 0, 1)
          , vec4(1, 0, 0, 1)
          , vec4(0, 1, 0, 1)
          , vec4(1, 1, 0, 1) });

        auto colorArray = std::vector<vec4>({
            vec4(1, 0, 0, 1)
          , vec4(1, 1, 0, 1)
          , vec4(0, 0, 1, 1)
          , vec4(1, 1, 0, 1)
          , vec4(0, 0, 1, 1)
          , vec4(0, 1, 0, 1) });

        ref_ptr<Buffer> vertexBuffer1 = new Buffer();
        vertexBuffer1->setData(vertexArray, GL_STATIC_DRAW);
        ref_ptr<Buffer> vertexBuffer2 = new Buffer();
        vertexBuffer2->setData(vertexArray, GL_STATIC_DRAW);
        ref_ptr<Buffer> colorBuffer = new Buffer();
        colorBuffer->setData(colorArray, GL_STATIC_DRAW);

        ref_ptr<VertexArray> vao = new VertexArray();

        vao->binding(0)->setAttribute(0);
        vao->binding(0)->setFormat(4, GL_FLOAT);

        vao->binding(1)->setAttribute(1);
        vao->binding(1)->setBuffer(colorBuffer, 0, sizeof(vec4));
        vao->binding(1)->setFormat(4, GL_FLOAT);

        vao->enable(0);
        vao->enable(1);


        // Create and setup TransformFeedback
        ref_ptr<TransformFeedback> transformFeedback = new TransformFeedback();
        transformFeedback->setVaryings(transformFeedbackProgram, { { "next_position" } }, GL_INTERLEAVED_ATTRIBS);


        auto t_start = std::chrono::high_resolution_clock::now();


        // Main loop
        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();
            draw(vao, transformFeedbackProgram, transformFeedback, shaderProgram, vertexBuffer1, vertexBuffer2, t_start);
            glfwSwapBuffers(window);
        }

    }

    // Properly shutdown GLFW
    glfwTerminate();

    return 0;
}
