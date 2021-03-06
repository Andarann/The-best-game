
#include "OpenGLRenderer.hpp"

OpenGLRenderer::OpenGLRenderer()
{

    init = false;

    if(!gladLoadGL()) {
        printf("Something went wrong!\n");
        exit(-1);
    }

    if (!sf::Shader::isAvailable())
    {
        std::cerr << "Can't use shaders. Maybe your graphic card is too old ? Check your drivers too !\n";
    }
}

OpenGLRenderer::~OpenGLRenderer()
{

}

void OpenGLRenderer::Init(sf::RenderTarget& obj)
{
    obj.pushGLStates();

    dummyModel.loadModel("resources/objects/cube.obj");
    /*for (int i(-20) ; i < 20 ; i++)
        dummyModel.cutModelAccordingToPlane(1.0, 0.0, 0.0, i, glm::vec3(1.0,0.0,0.0));

    for (int i(-20) ; i < 20 ; i++)
        dummyModel.cutModelAccordingToPlane(0.0, 1.0, 0.0, i, glm::vec3(0.0,1.0,0.0));*/

    myShader.loadFromFile("src/shaders/model_loading_vertex.txt", "src/shaders/model_loading_frag.txt");

    firstPointSet = false;
    init = true;

    obj.popGLStates();
}

void OpenGLRenderer::render(sf::RenderTarget& target, Camera& camObj, sf::Window& window)
{
    if (!init)
    {
        Init(target);
    }

    if (init)
    {
        sf::Shader::bind(&myShader);

        sf::Vector3f lightPos(camObj.getX(), camObj.getY(), camObj.getZ());

        glm::mat4 model, view, projection;
        glm::mat3 normalMat;

        model = glm::mat4();
        model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));


        view = camObj.getViewMatrix();

        projection = glm::perspective(1.0f, (float)target.getSize().x / target.getSize().y, 0.1f, 100.0f);

        normalMat = glm::transpose(glm::inverse(glm::mat3(model)));

        myShader.setUniform("model", sf::Glsl::Mat4(glm::value_ptr(model)));
        myShader.setUniform("view", sf::Glsl::Mat4(glm::value_ptr(view)));
        myShader.setUniform("projection", sf::Glsl::Mat4(glm::value_ptr(projection)));
        myShader.setUniform("normalMat", sf::Glsl::Mat4(glm::value_ptr(normalMat)));

        myShader.setUniform("viewPos", sf::Glsl::Vec3(camObj.getX(), camObj.getY(), camObj.getZ()));
        myShader.setUniform("lightPos", sf::Glsl::Vec3(camObj.getX(), camObj.getY(), camObj.getZ()));


        dummyModel.Draw(myShader);

        for (int i(0) ; i < 10 ; i++)
        {
            model = glm::mat4();
            model = glm::translate(model, glm::vec3(0.0f, -1.75f, i));
            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

            myShader.setUniform("model", sf::Glsl::Mat4(glm::value_ptr(model)));

            dummyModel.Draw(myShader);
        }

        sf::Shader::bind(NULL);
    }
    else
    {
        std::cout << "Can't draw the OpenGLRenderer : its initialization failed.\n";
        exit(-1);
    }
}

