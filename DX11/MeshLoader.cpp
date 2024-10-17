#include "MeshLoader.h"

void MeshLoader::LoadModel(const std::string& pFile)
{
    const aiScene* scene = importer.ReadFile(pFile,
        aiProcess_CalcTangentSpace |
        aiProcess_Triangulate |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType |
        aiProcess_MakeLeftHanded |
        aiProcess_FlipUVs | aiProcess_FlipWindingOrder);
    

   /* if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return;
    }*/

    // Process the root node to start loading the model's meshes
    ProcessNode(scene->mRootNode, scene);
}

void MeshLoader::ProcessNode(aiNode* node, const aiScene* scene)
{
    // Process each mesh located at this node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);
    }

    // After processing all meshes, process the children nodes recursively
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        this->ProcessNode(node->mChildren[i], scene);
    }
}

void MeshLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        ModelVertex vertex;


        vertex.vert.x = mesh->mVertices[i].x;
        vertex.vert.y = mesh->mVertices[i].y;
        vertex.vert.z = mesh->mVertices[i].z;
        vertex.Color.x = 1.0f; 
        vertex.Color.y = 1.0f; // Set green channel
        vertex.Color.z = 1.0f; // Set blue channel

        if (mesh->HasTextureCoords(0))
        {
            vertex.text.x = (float)mesh->mTextureCoords[0][i].x;
            vertex.text.y = (float)mesh->mTextureCoords[0][i].y;
        }

        if (mesh->HasNormals())
        {
            vertex.norm.x = mesh->mNormals[i].x;
            vertex.norm.y = mesh->mNormals[i].y;
            vertex.norm.z = mesh->mNormals[i].z;
        }
        vertices.push_back(vertex);
    }

    // Process indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {

        aiFace face = mesh->mFaces[i];

        for (unsigned int j = 0; j < face.mNumIndices; j++) {

            indices.push_back(face.mIndices[j]);

        }
    }
}

 std::vector<MeshLoader::ModelVertex>& MeshLoader::GetVertices() {
    return vertices;
}

 std::vector<unsigned int>& MeshLoader::GetIndices()  {
    return indices;
}
