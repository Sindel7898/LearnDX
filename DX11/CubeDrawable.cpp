#include "CubeDrawable.h"


Window* windowContextHolderHolder;

std::vector<int> CubeIndex = {
     
      0, 1, 2,   0, 2, 3,

      // Back face
      4, 6, 5,   4, 7, 6,

      // Left face
      4, 5, 1,   4, 1, 0,

      // Right face
      3, 2, 6,   3, 6, 7,

      // Top face
      1, 5, 6,   1, 6, 2,

      // Bottom face
      4, 0, 3,   4, 3, 7,

};



CubeDrawable::CubeDrawable(ID3D11Device* device, ID3D11DeviceContext* D3DDeviceContext, Window* windowContextHolder, DirectX::XMFLOAT3 location)
    : Location(location)
{
 
   
    windowContextHolderHolder = windowContextHolder;
   
    //Vertex Data Containing informatoin for vertex positions and texture coords
    struct Vertex {

        DirectX::XMFLOAT3A position;   // Vertex position
        DirectX::XMFLOAT3A normal;
        DirectX::XMFLOAT3A color;


    };


     std::vector<Vertex> CubeData = {

        { DirectX::XMFLOAT3A( - 1.0f, -1.0f, -1.0f),  DirectX::XMFLOAT3A(0.0f, 0.0f, -1.0f),  DirectX::XMFLOAT3A(1.0f, 0.0f, 0.0f) }, // Bottom-left-front (Red)
        { DirectX::XMFLOAT3A(-1.0f,  1.0f, -1.0f ),   DirectX::XMFLOAT3A(0.0f, 0.0f, -1.0f ), DirectX::XMFLOAT3A(1.0f, 0.0f, 0.0f) }, // Top-left-front (Green)
        { DirectX::XMFLOAT3A(1.0f,  1.0f, -1.0f ),    DirectX::XMFLOAT3A(0.0f, 0.0f, -1.0f ), DirectX::XMFLOAT3A(1.0f, 0.0f, 0.0f) }, // Top-right-front (Blue)
        { DirectX::XMFLOAT3A(1.0f, -1.0f, -1.0f ),    DirectX::XMFLOAT3A(0.0f, 0.0f, -1.0f ), DirectX::XMFLOAT3A(1.0f, 0.0f, 0.0f) }, // Bottom-right-front (Yellow)

    // Back face
        { DirectX::XMFLOAT3A(-1.0f, -1.0f,  1.0f ),   DirectX::XMFLOAT3A(0.0f, 0.0f, 1.0f ),  DirectX::XMFLOAT3A(1.0f, 0.0f, 0.0f) }, // Bottom-left-back (Magenta)
        { DirectX::XMFLOAT3A(-1.0f,  1.0f,  1.0f ),   DirectX::XMFLOAT3A(0.0f, 0.0f, 1.0f ),  DirectX::XMFLOAT3A(1.0f, 0.0f, 0.0f) }, // Top-left-back (Cyan)
        { DirectX::XMFLOAT3A(1.0f,  1.0f,  1.0f ),    DirectX::XMFLOAT3A(0.0f, 0.0f, 1.0f ),  DirectX::XMFLOAT3A(1.0f, 0.0f, 0.0f) }, // Top-right-back (White)
        { DirectX::XMFLOAT3A(1.0f, -1.0f,  1.0f ),    DirectX::XMFLOAT3A(0.0f, 0.0f, 1.0f ),  DirectX::XMFLOAT3A(1.0f, 0.0f, 0.0f) } // Bo
     
     };


     auto vertexBuffer =  std::make_shared<VertexBuffer<Vertex>>(device, CubeData);
     AddBindable(vertexBuffer);
     ///////////////////////////////////////////////////////////////////////////////////////////
     
  //INITIALMATIXDATA////////////////////////////////////////////////////////////////////////////
     DirectX::XMMATRIX WorldPosition = DirectX::XMMatrixIdentity();
     DirectX::XMMATRIX projectionmatrix = DirectX::XMMatrixIdentity();
     DirectX::XMMATRIX viewMatrix = DirectX::XMMatrixIdentity();


     std::vector<DirectX::XMMATRIX> initialTransformations = {

          WorldPosition,
          projectionmatrix,
          viewMatrix

       };


   Matrix = std::make_shared<ConstantBuffer<DirectX::XMMATRIX>>(device, initialTransformations, "Vertex",0);
   AddBindable(Matrix);
   


   std::vector<LightData> LightDatainfo = {

      { DirectX::XMFLOAT3A(0.0f,0.0f,0.0f), DirectX::XMFLOAT4(0.0f,0.0f,0.0f,0.0f)}

   };

   LightBuffer = std::make_shared<ConstantBuffer<LightData>>(device, LightDatainfo, "Pixel",0);
   AddBindable(LightBuffer);



   /// Create index Buffer/////////////////////////////////////////////////////////////////

   auto indexBuffer = std::make_shared<IndexBuffer>(device, CubeIndex);
   AddBindable(indexBuffer);

   ////////////////////////////////////////////////////////////////////

   ////Create pixel and vertex shaders//////////////////////////////////////////////////////
   auto pixelShader = std::make_shared<PixelShader>(device, L"DiffuseLighting.hlsl");
   AddBindable(pixelShader);


   auto vertexShader = std::make_shared<VertexShader>(device, L"BoxVertexShader.hlsl", pixelShader.get() );
   AddBindable(vertexShader);
   ///////////////////////////////////////////////////////////////////////////////////////////


   ///INPUT LAYOUT/////////////////////////////////////////////////////////////////////////////
   D3D11_INPUT_ELEMENT_DESC ied[]{

        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}


   };

   auto inputLayout = std::make_shared<InputLayout>(device, ied, std::size(ied), pixelShader.get());
   AddBindable(inputLayout);

   ////////////////////////////////////////////////////////////////////////////////////////////////////  
   
   

}


// stuff to be updated every frame
void CubeDrawable::Update(ID3D11DeviceContext* context, ID3D11Device* device, Window* windowApp, float updateRotation, 
    DirectX::XMFLOAT3A uLightPosition,
    DirectX::XMFLOAT4 uLightColor){

    

    DirectX::XMMATRIX WorldMatrix = DirectX::XMMatrixTranspose(
      
        DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f) *
        DirectX::XMMatrixRotationZ(updateRotation) *
        DirectX::XMMatrixRotationX(0) *
        DirectX::XMMatrixTranslation(Location.x, Location.y, Location.z));

             
    float aspectRatio = static_cast<int>(windowContextHolderHolder->GetWindowWidth()) /
                        static_cast<int>(windowContextHolderHolder->GetWindowHeight());

    DirectX::XMMATRIX ProjectionMatrix = DirectX::XMMatrixTranspose(
                    DirectX::XMMatrixPerspectiveLH(DirectX::XMConvertToRadians(45),
                     aspectRatio,1.0f, 200.0f));

    DirectX::XMVECTOR cameraPosition = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f); // Camera position
    DirectX::XMVECTOR lookAtPosition = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f); // Point the camera is looking at
    DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // Up direction

    DirectX::XMMATRIX ViewMatrix = XMMatrixTranspose( DirectX::XMMatrixLookAtLH(cameraPosition, lookAtPosition, upDirection));
    
    std::vector<DirectX::XMMATRIX> MatrixData = {

       WorldMatrix,
       ProjectionMatrix,
       ViewMatrix

    };
 

    //update constant buffer data || LOOK A BIT MORE INTO THIS||
    Matrix->Update(context, MatrixData);

    
    std::vector<LightData> LightDatainfo = {

        {uLightPosition,uLightColor}

    };

    LightBuffer->Update(context, LightDatainfo);

    std::cout << "Light position: x =" << uLightPosition.x << " Y=" << uLightPosition.y << " Z=" << uLightPosition.z << std::endl;



}

void CubeDrawable::Draw(ID3D11DeviceContext* context, ID3D11Device* device, Window* windowApp)  {

    Bind(context);//Bind all the bindables to the object 

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_VIEWPORT vp{};

    vp.Width = static_cast<float>(windowApp->GetWindowWidth());
    vp.Height = static_cast<float>(windowApp->GetWindowHeight());
    vp.MaxDepth = 1;
    vp.MinDepth = 0;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    context->RSSetViewports(1, &vp);

    UINT indexCount = static_cast<UINT>(CubeIndex.size());

    context->DrawIndexed(indexCount, 0, 0);

}


