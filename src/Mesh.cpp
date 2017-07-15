
#include "Mesh.hpp"

Mesh::Mesh()
{

}

Mesh::Mesh(std::vector<Vertex>& vertices_, std::vector<unsigned int>& indices_, std::vector<Texture>& textures_)
{
    VAO = 0;
    VBO = 0;
    EBO = 0;

    vertices = vertices_;
    indices = indices_;
    textures = textures_;

    setupMesh();
}

Mesh::~Mesh()
{

}

void Mesh::Draw(sf::Shader& shader)
{
    sf::Shader::bind(&shader);

    unsigned int diffuseNr  = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int heightNr = 1;

    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::stringstream ss;
        std::string number;
        std::string name = textures[i].type;

        if(name == "texture_diffuse")
            ss << diffuseNr++;
        else if(name == "texture_specular")
            ss << specularNr++;
        else if(name == "texture_normal")
            ss << normalNr++;
        else if(name == "texture_height")
            ss << heightNr++;

        number = ss.str();

        shader.setUniform(("material." + name + number), (float)i);

        glUniform1i(glGetUniformLocation(shader.getNativeHandle(), (name + number).c_str()), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }



    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);

    sf::Shader::bind(NULL);
}


void Mesh::setupMesh()
{

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);


        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, BiTangent));

    glBindVertexArray(0);

}

#define EQUALITY_THRESHOLD 0.01

Mesh Mesh::cutMeshInTwo(float a, float b, float c, float d)
{
    if (vertices.size() % 3 == 0)
    {
        /**Adding the necessary vertices*/

        int interNumber(0);
        Vertex newVertices[2];
        int edgeIntersected[2];

        for (int iii(0) ; iii < vertices.size() ; iii+=3)
        {
            interNumber = 0;

            for (int jjj(0) ; jjj < 3 ; jjj++)
            {
                float e,f,g,h,i,j;//Just recopying my calculations, too bothered to use theStruct.whatev
                e = vertices[iii + jjj].Position.x;//First point
                f = vertices[iii + jjj].Position.y;
                g = vertices[iii + jjj].Position.z;
                h = vertices[iii + ((jjj + 1) % 3)].Position.x;//Second point
                i = vertices[iii + ((jjj + 1) % 3)].Position.y;
                j = vertices[iii + ((jjj + 1) % 3)].Position.z;

                //Tests are done edge by edge
                float t = (-(a * e + b * f + c * g + d))/(a * (h - e) + b * (i - f) + c * (j - g));

                Vertex small, high;

                if (e < h)
                {
                    small = vertices[iii + jjj];
                    high = vertices[iii + ((jjj + 1) % 3)];
                }
                else
                {
                    small = vertices[iii + ((jjj + 1) % 3)];
                    high = vertices[iii + jjj];
                }

                float xInter(e + t * (h - e));

                if (small.Position.x < xInter && xInter < high.Position.x)//If yes, we found an intersection
                {

                    float factor = (xInter - small.Position.x) / (high.Position.x - small.Position.x);

                    //Doing weighted averages to get new vertices data
                    newVertices[interNumber].Position = factor * high.Position + (1 - factor) * small.Position;
                    newVertices[interNumber].BiTangent = factor * high.BiTangent + (1 - factor) * small.BiTangent;
                    newVertices[interNumber].Normal = factor * high.Normal + (1 - factor) * small.Normal;
                    newVertices[interNumber].Tangent = factor * high.Tangent + (1 - factor) * small.Tangent;
                    newVertices[interNumber].TexCoords = factor * high.TexCoords + (1 - factor) * small.TexCoords;

                    edgeIntersected[interNumber] = jjj;
                    interNumber++;
                }
            }

            if (interNumber == 1)
            {
                Vertex layout[4];
                Vertex finalLayout[6];

                layout[0] = vertices[iii +  edgeIntersected[0]];
                layout[1] = newVertices[0];
                layout[2] = vertices[iii + ((edgeIntersected[0] + 1) % 3)];
                layout[3] = vertices[iii + ((edgeIntersected[0] + 2) % 3)];

                finalLayout[0] = layout[2];
                finalLayout[1] = layout[0];
                finalLayout[2] = layout[1];
                finalLayout[3] = layout[3];
                finalLayout[4] = layout[0];
                finalLayout[5] = layout[2];

                //Now lets finally insert the new vertices
                vertices.insert(vertices.begin() + iii, finalLayout, finalLayout+3);


                for (int jjj(0) ; jjj < 6 ; jjj++)
                    vertices[iii + jjj] = finalLayout[jjj];

                iii += 3;//We skip the two added triangles

            }
            else if (interNumber == 2)
            {
                Vertex layout[5];
                Vertex finalLayout[9];

                if (edgeIntersected[1] == ((edgeIntersected[0] + 1)%3))
                {
                    layout[0] = vertices[iii + edgeIntersected[0]];
                    layout[1] = newVertices[0];
                    layout[2] = vertices[iii + edgeIntersected[1]];
                    layout[3] = newVertices[1];
                    layout[4] = vertices[iii + ((edgeIntersected[1] + 1)%3)];
                }
                else if (((edgeIntersected[1] + 1)%3) == edgeIntersected[0])
                {
                    layout[0] = vertices[iii + edgeIntersected[1]];
                    layout[1] = newVertices[1];
                    layout[2] = vertices[iii + edgeIntersected[0]];
                    layout[3] = newVertices[0];
                    layout[4] = vertices[iii + ((edgeIntersected[0] + 1)%3)];
                }
                else
                {
                    std::cout << "NOOOOOOOOOO\n";
                    exit(-1);
                }

                finalLayout[0] = layout[4];
                finalLayout[1] = layout[0];
                finalLayout[2] = layout[1];

                finalLayout[3] = layout[3];
                finalLayout[4] = layout[2];
                finalLayout[5] = layout[1];

                finalLayout[6] = layout[4];
                finalLayout[7] = layout[1];
                finalLayout[8] = layout[3];

                //Now lets finally insert the new vertices

                vertices.insert(vertices.begin() + iii, finalLayout, finalLayout+6);

                for (int jjj(0) ; jjj < 9 ; jjj++)
                    vertices[iii + jjj] = finalLayout[jjj];

                iii += 6;//We skip the two added triangles
            }

        }
        while (indices.size() < vertices.size())
        {
            indices.push_back(0);
            indices[indices.size() - 1] = indices[indices.size() - 2] + 1;
        }

        std::vector<Vertex> verticesS1;
        std::vector<unsigned int> indicesS1;
        std::vector<Texture> texturesS1;

        std::vector<Vertex> verticesS2;
        std::vector<unsigned int> indicesS2;
        std::vector<Texture> texturesS2;
        texturesS1 = textures;
        texturesS2 = textures;

        ///Now we spread the triangles around the plane
        for (int iii(0) ; iii < vertices.size() ; iii+=3)
        {
            float dPrimeAVG = (putPlaneOnPoint(vertices[iii].Position, a,b,c) +
                              putPlaneOnPoint(vertices[iii + 1].Position, a,b,c) +
                              putPlaneOnPoint(vertices[iii + 2].Position, a,b,c)) / 3;

            if (d - dPrimeAVG >= EQUALITY_THRESHOLD)
            {
                for (int kkk(0) ; kkk < 3 ; kkk++)
                {
                    verticesS1.push_back(vertices[iii + kkk]);
                    indicesS1.push_back(indices[iii + kkk]);
                }
            }
            else if (d - dPrimeAVG <= -EQUALITY_THRESHOLD)
            {
                for (int kkk(0) ; kkk < 3 ; kkk++)
                {
                    verticesS2.push_back(vertices[iii + kkk]);
                    verticesS2[verticesS2.size() - 1].Position.z += 1;
                    indicesS2.push_back(indices[iii + kkk]);
                }
            }
            else
            {
                std::cout << "ouch\n";
            }
        }

        for (int i(0) ; i < indicesS1.size() ; i++)
            indicesS1[i] = i;

        for (int i(0) ; i < indicesS2.size() ; i++)
            indicesS2[i] = i;

        vertices = verticesS1;
        indices = indicesS1;
        textures = texturesS1;

        setupMesh();
        return Mesh(verticesS2, indicesS2, texturesS2);
    }
    else
    {
        std::cout << "Mesh has non-triangular faces, see to fix it.\n";
        return;
    }
}

Mesh Mesh::spreadTrianglesAroundPlane(float a, float b, float c, float d)
{
    if (vertices.size() % 3 == 0)
    {
        /**Adding the necessary vertices*/

        std::vector<Vertex> verticesS1;
        std::vector<unsigned int> indicesS1;
        std::vector<Texture> texturesS1;

        std::vector<Vertex> verticesS2;
        std::vector<unsigned int> indicesS2;
        std::vector<Texture> texturesS2;
        texturesS1 = textures;
        texturesS2 = textures;

        int interNumber(0);
        Vertex newVertices[2];
        int edgeIntersected[2];

        for (int iii(0) ; iii < vertices.size() ; iii+=3)
        {
            interNumber = 0;

            /**We will count the number of intersections and on what edge they are*/
            for (int jjj(0) ; jjj < 3 ; jjj++)
            {

                float e,f,g,h,i,j;//Just recopying my calculations, too bothered to use theStruct.whatev
                e = vertices[iii + jjj].Position.x;//First point
                f = vertices[iii + jjj].Position.y;
                g = vertices[iii + jjj].Position.z;
                h = vertices[iii + ((jjj + 1) % 3)].Position.x;//Second point
                i = vertices[iii + ((jjj + 1) % 3)].Position.y;
                j = vertices[iii + ((jjj + 1) % 3)].Position.z;

                //Tests are done edge by edge
                float t = (-(a * e + b * f + c * g + d))/(a * (h - e) + b * (i - f) + c * (j - g));

                Vertex small, high;

                if (e < h)
                {
                    small = vertices[iii + jjj];
                    high = vertices[iii + ((jjj + 1) % 3)];
                }
                else
                {
                    small = vertices[iii + ((jjj + 1) % 3)];
                    high = vertices[iii + jjj];
                }

                float xInter(e + t * (h - e));

                if (small.Position.x < xInter && xInter < high.Position.x)
                {
                    float factor = (xInter - small.Position.x) / (high.Position.x - small.Position.x);

                    //Doing weighted averages to get new vertices data
                    newVertices[interNumber].Position = factor * high.Position + (1 - factor) * small.Position;
                    newVertices[interNumber].BiTangent = factor * high.BiTangent + (1 - factor) * small.BiTangent;
                    newVertices[interNumber].Normal = factor * high.Normal + (1 - factor) * small.Normal;
                    newVertices[interNumber].Tangent = factor * high.Tangent + (1 - factor) * small.Tangent;
                    newVertices[interNumber].TexCoords = factor * high.TexCoords + (1 - factor) * small.TexCoords;

                    edgeIntersected[interNumber] = jjj;
                    interNumber++;
                }
            }

            if (interNumber == 0)
            {
                for (int jjj(0) ; jjj < 3 ; jjj++)
                {

                    if (d - putPlaneOnPoint(vertices[iii + jjj].Position, a,b,c) >= EQUALITY_THRESHOLD)
                    {
                        for (int kkk(0) ; kkk < 3 ; kkk++)
                        {
                            verticesS1.push_back(vertices[iii + kkk]);
                            indicesS1.push_back(indices[iii + kkk]);
                        }
                        break;
                    }
                    else
                    {
                        for (int kkk(0) ; kkk < 3 ; kkk++)
                        {
                            verticesS2.push_back(vertices[iii + kkk]);
                            indicesS2.push_back(indices[iii + kkk]);
                        }
                        break;
                    }
                }
            }
            else if (interNumber == 1)
            {
                Vertex layout[4];
                Vertex finalLayout[6];

                layout[0] = vertices[iii +  edgeIntersected[0]];
                layout[1] = newVertices[0];
                layout[2] = vertices[iii + ((edgeIntersected[0] + 1) % 3)];
                layout[3] = vertices[iii + ((edgeIntersected[0] + 2) % 3)];

                finalLayout[0] = layout[2];
                finalLayout[1] = layout[0];
                finalLayout[2] = layout[1];

                finalLayout[3] = layout[3];
                finalLayout[4] = layout[0];
                finalLayout[5] = layout[2];

                if (d - putPlaneOnPoint(layout[1].Position,a,b,c) >= 0)//S1
                {
                    float S1Area = calculateTriangleArea(finalLayout[0].Position, finalLayout[1].Position, finalLayout[2].Position);
                    float S2Area = calculateTriangleArea(finalLayout[3].Position, finalLayout[4].Position, finalLayout[5].Position);

                    std::cout << S1Area << " and " << S2Area << '\n';

                    if (S1Area > S2Area)
                    {
                        for (int kkk(0) ; kkk < 3 ; kkk++)
                        {
                            verticesS1.push_back(vertices[iii + kkk]);
                            indicesS1.push_back(indices[iii + kkk]);
                        }
                    }
                    else
                    {
                        for (int kkk(0) ; kkk < 3 ; kkk++)
                        {
                            verticesS2.push_back(vertices[iii + kkk]);
                            indicesS2.push_back(indices[iii + kkk]);
                        }
                    }
                }
                else//S2
                {
                    float S1Area = calculateTriangleArea(finalLayout[3].Position, finalLayout[4].Position, finalLayout[5].Position);
                    float S2Area = calculateTriangleArea(finalLayout[0].Position, finalLayout[1].Position, finalLayout[2].Position);

                    if (S1Area > S2Area)
                    {
                        for (int kkk(0) ; kkk < 3 ; kkk++)
                        {
                            verticesS1.push_back(vertices[iii + kkk]);
                            indicesS1.push_back(indices[iii + kkk]);
                        }
                    }
                    else
                    {
                        for (int kkk(0) ; kkk < 3 ; kkk++)
                        {
                            verticesS2.push_back(vertices[iii + kkk]);
                            indicesS2.push_back(indices[iii + kkk]);
                        }
                    }
                }
            }
            else if (interNumber == 2)
            {
                Vertex layout[5];
                Vertex finalLayout[9];

                if (edgeIntersected[1] == ((edgeIntersected[0] + 1)%3))
                {
                    layout[0] = vertices[iii + edgeIntersected[0]];
                    layout[1] = newVertices[0];
                    layout[2] = vertices[iii + edgeIntersected[1]];
                    layout[3] = newVertices[1];
                    layout[4] = vertices[iii + ((edgeIntersected[1] + 1)%3)];
                }
                else if (((edgeIntersected[1] + 1)%3) == edgeIntersected[0])
                {
                    layout[0] = vertices[iii + edgeIntersected[1]];
                    layout[1] = newVertices[1];
                    layout[2] = vertices[iii + edgeIntersected[0]];
                    layout[3] = newVertices[0];
                    layout[4] = vertices[iii + ((edgeIntersected[0] + 1)%3)];
                }
                else
                {
                    std::cout << "NOOOOOOOOOO\n";
                    exit(-1);
                }

                finalLayout[0] = layout[4];
                finalLayout[1] = layout[1];
                finalLayout[2] = layout[0];

                finalLayout[3] = layout[3];
                finalLayout[4] = layout[2];
                finalLayout[5] = layout[1];

                finalLayout[6] = layout[4];
                finalLayout[7] = layout[1];
                finalLayout[8] = layout[3];

                if (d - putPlaneOnPoint(layout[2].Position,a,b,c) >= 0)//S1
                {
                    float S1Area = calculateTriangleArea(finalLayout[3].Position, finalLayout[4].Position, finalLayout[5].Position);

                    float S2Area = calculateTriangleArea(finalLayout[0].Position, finalLayout[1].Position, finalLayout[2].Position)
                                    + calculateTriangleArea(finalLayout[6].Position, finalLayout[7].Position, finalLayout[8].Position);

                    if (S1Area > S2Area)
                    {
                        for (int kkk(0) ; kkk < 3 ; kkk++)
                        {
                            verticesS1.push_back(vertices[iii + kkk]);
                            indicesS1.push_back(indices[iii + kkk]);
                        }
                    }
                    else
                    {
                        for (int kkk(0) ; kkk < 3 ; kkk++)
                        {
                            verticesS2.push_back(vertices[iii + kkk]);
                            indicesS2.push_back(indices[iii + kkk]);
                        }
                    }
                }
                else//S2
                {
                    float S1Area = calculateTriangleArea(finalLayout[0].Position, finalLayout[1].Position, finalLayout[2].Position)
                                    + calculateTriangleArea(finalLayout[6].Position, finalLayout[7].Position, finalLayout[8].Position);

                    float S2Area = calculateTriangleArea(finalLayout[3].Position, finalLayout[4].Position, finalLayout[5].Position);

                    if (S1Area > S2Area)
                    {
                        for (int kkk(0) ; kkk < 3 ; kkk++)
                        {
                            verticesS1.push_back(vertices[iii + kkk]);
                            indicesS1.push_back(indices[iii + kkk]);
                        }
                    }
                    else
                    {
                        for (int kkk(0) ; kkk < 3 ; kkk++)
                        {
                            verticesS2.push_back(vertices[iii + kkk]);
                            indicesS2.push_back(indices[iii + kkk]);
                        }
                    }
                }
            }

        }
        for (int i(0) ; i < indicesS1.size() ; i++)
            indicesS1[i] = i;

        for (int i(0) ; i < indicesS2.size() ; i++)
            indicesS2[i] = i;

        vertices = verticesS1;
        indices = indicesS1;
        textures = texturesS1;

        setupMesh();
        return Mesh(verticesS2, indicesS2, texturesS2);
    }
    else
    {
        std::cout << "Mesh has non-triangular faces, see to fix it.\n";
        return;
    }
}

float calculateTriangleArea(glm::vec3 pointA, glm::vec3 pointB, glm::vec3 pointC)
{
    if (pointA != pointB && pointA != pointC && pointB != pointC)
    {
        float AB = glm::distance(pointA, pointB);
        float BC = glm::distance(pointB, pointC);
        float AC = glm::distance(pointA, pointC);
        float s = (AB + BC + AC)/2;

        return sqrtf(s * (s - AB) * (s - BC) * (s - AC));
    }
    else
        return 0.0f;
}

float putPlaneOnPoint(glm::vec3 point, float a, float b, float c)
{
    return (-(a * point.x + b * point.y + c * point.z));
}
