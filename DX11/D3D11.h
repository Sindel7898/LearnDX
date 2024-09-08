#pragma once
#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl.h>
#include "Window.h"

#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "comdef.h"

#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <DirectXMath.h>
#include "CubeDrawable.h"

class D3D11
{

public:

    D3D11(Window* windowApp);
    void load();
    void Update();
    ~D3D11();

    void ClearBuffer(float red, float green, float blue);

    D3D11& operator=(const D3D11&) = delete;
    D3D11(const D3D11&) = delete;

    void EndFrame();


    Window* windowContextHolder;

    
    float rotaion = 1.0f;


private:

    Microsoft::WRL::ComPtr<IDXGISwapChain> SwapChain;
    Microsoft::WRL::ComPtr<ID3D11Device> D3DDevice;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> D3DDeviceContext;
   
    Microsoft::WRL::ComPtr<ID3D11Resource> Buffer ;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> RenderTargetView ;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> DepthSentcilView;

    std::vector<std::shared_ptr<CubeDrawable>> Cube;



};