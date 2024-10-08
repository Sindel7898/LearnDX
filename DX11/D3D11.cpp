#include "D3D11.h"
#include "MACROS.h"
#include <random>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")


D3D11::D3D11(Window* windowApp){
    
    windowContextHolder = windowApp;
    
    //CREATING DEVICE AND SWAPCHAIN  /////////////////////////////////////////////////////////
    DXGI_RATIONAL MonitorRefreshRatedData{};

    MonitorRefreshRatedData.Numerator = 60;
    MonitorRefreshRatedData.Denominator = 1;

    DXGI_MODE_DESC SwapChainBufferData{};

    SwapChainBufferData.Width = windowApp->GetWindowWidth();
    SwapChainBufferData.Height = windowApp->GetWindowHeight();
    SwapChainBufferData.RefreshRate = MonitorRefreshRatedData;
    SwapChainBufferData.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    SwapChainBufferData.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    SwapChainBufferData.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    DXGI_SAMPLE_DESC SamplingData{};
 
    SamplingData.Count = 1;
    SamplingData.Quality = 0;

    HWND window = glfwGetWin32Window(windowApp->GetWindow());
  
    DXGI_SWAP_CHAIN_DESC SwapChainDesc{};

    SwapChainDesc.BufferDesc = SwapChainBufferData;
    SwapChainDesc.SampleDesc = SamplingData;
    SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    SwapChainDesc.BufferCount = 1;
    SwapChainDesc.OutputWindow = window;
    SwapChainDesc.Windowed = TRUE;
    SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    SwapChainDesc.Flags = 0;

    CHECK_HRESULT(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
          D3D11_CREATE_DEVICE_DEBUG,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &SwapChainDesc,
        &SwapChain, 
        &D3DDevice,
        nullptr,
        &D3DDeviceContext));

   
    //GET BackBuffer and Create a render TargetView to add color to the buffer

    CHECK_HRESULT(SwapChain->GetBuffer(0, __uuidof(ID3D11Resource), &Buffer));

    CHECK_HRESULT(D3DDevice->CreateRenderTargetView(Buffer.Get(), nullptr, &RenderTargetView));



    //// Creating a depth stencil state and setting it to the OUT PUT MERGER //////////////////////////////////////////////////// 
   
    D3D11_DEPTH_STENCIL_DESC DepthStencilStateDesc{};
    DepthStencilStateDesc.DepthEnable = TRUE;
    DepthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    DepthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthSterncilState;

    CHECK_HRESULT(D3DDevice->CreateDepthStencilState(&DepthStencilStateDesc, &DepthSterncilState));
    D3DDeviceContext->OMSetDepthStencilState(DepthSterncilState.Get(), 1);

    
    //Creating a depthstencil view and setting it to the output merger
    D3D11_TEXTURE2D_DESC  TextureDesc{};
    Microsoft::WRL::ComPtr<ID3D11Texture2D> DepthStencilTexture;

    TextureDesc.Width = windowApp->GetWindowWidth();
    TextureDesc.Height = windowApp->GetWindowHeight();
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_D32_FLOAT;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    
    CHECK_HRESULT(D3DDevice->CreateTexture2D(&TextureDesc, nullptr, &DepthStencilTexture));


    D3D11_DEPTH_STENCIL_VIEW_DESC DepthStencilViewDesc {};
    DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
    DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    DepthStencilViewDesc.Texture2D.MipSlice = 0;

    CHECK_HRESULT(D3DDevice->CreateDepthStencilView(DepthStencilTexture.Get(), &DepthStencilViewDesc, &DepthSentcilView));

    D3DDeviceContext->OMSetRenderTargets(1, RenderTargetView.GetAddressOf(), DepthSentcilView.Get());

    load();

    }

    void D3D11::load() {

        DirectX::XMFLOAT3A Lightlocation = { -2.0f,1.0f,1.0f };

        DirectX::XMFLOAT3A Lightlocation2 = { -30.0f,1.0f,1.0f };

        DirectX::XMFLOAT3 Cubelocation = { 1.0f,-10.0f,20.0f };

       Light1 = std::make_shared<Light>(D3DDevice.Get(), D3DDeviceContext.Get(), windowContextHolder, Lightlocation,0);
        //Light2 = std::make_shared <Light>(D3DDevice.Get(), D3DDeviceContext.Get(), windowContextHolder, Lightlocation2,1);


        float spacing = 4.0f;
        
        for (int i = 0; i <= 4; i++) {

            std::random_device rd;  // Obtain a random number from hardware
            std::mt19937 eng(rd()); // Seed the generator

            // Define the range
            std::uniform_int_distribution<> distr(-25, 25); // Define the range [1, 100]

            // Generate and output a random number
            Lightlocation.x = distr(eng);

            Lightlocation.z = distr(eng);

          Lights.push_back(std::make_shared<Light>(D3DDevice.Get(), D3DDeviceContext.Get(), windowContextHolder, Lightlocation, i));
        }
    
        Cube.push_back(std::make_shared<CubeDrawable>(D3DDevice.Get(), D3DDeviceContext.Get(), windowContextHolder, Cubelocation, Lights)); // Adjust parameters as needed

        int visibleCubeCount = Cube.size();
        std::cout << "Total number of cubes on screen: " << visibleCubeCount << std::endl;

    }

     float clearColor[3] = { 0.1f, 0.1f, 0.1f};

    void D3D11::Update() {

        ClearBuffer(0.0f, 0.0f, 0.0f);

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("FPS Display");
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::End();
       

      
        for (int i = 0; i < Cube.size(); i++) {

            Cube[i]->Update();
         
            Cube[i]->Draw();
            
        }


        for (int i = 0; i < Lights.size(); i++) {

            Lights[i]->Update();

            Lights[i]->Draw();

        }

       
     

       /* Light1->Update();
        Light2->Update();*/

        //Light1->Draw();
        //Light2->Draw();

        ImGui::Render();
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

D3D11::~D3D11()
{
    RenderTargetView->Release();
    DepthSentcilView->Release();

    SwapChain->Release();
    D3DDevice->Release();
    D3DDeviceContext->Release();
}

void D3D11::ClearBuffer(float red, float green, float blue)
{
    float clour[] = { red,green,blue,1.0f };

    D3DDeviceContext->ClearRenderTargetView(RenderTargetView.Get(), clour);
    D3DDeviceContext->ClearDepthStencilView(DepthSentcilView.Get(), D3D11_CLEAR_DEPTH, 1, 0);
    
 }


void D3D11::EndFrame()
{
    SwapChain->Present(1, 0);
}


