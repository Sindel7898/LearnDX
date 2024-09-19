#pragma once
#include "Bindable.h"
#include <DirectXMath.h>

template<typename T> 

class VertexBuffer : public Bindable {
public :

	VertexBuffer(ID3D11Device* Device, std::vector<T> VertexData) {


        D3D11_BUFFER_DESC VertexBufferDesc = {}; 
        VertexBufferDesc.ByteWidth = static_cast<UINT>( sizeof(T) * VertexData.size());

        VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        VertexBufferDesc.CPUAccessFlags = 0;
        VertexBufferDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA VertexBufferSubSurfaceData = {};
        VertexBufferSubSurfaceData.pSysMem = VertexData.data();


        //Create Vertex Buffer
        HRESULT hr =   Device->CreateBuffer(&VertexBufferDesc, &VertexBufferSubSurfaceData, &vVertexBuffer);
        
        if (SUCCEEDED(hr)) {
            vertexBufferCount++; // Increment the counter
            std::cout << "Vertex buffer created. Total count: " << vertexBufferCount << std::endl;
        }
	}

	void Bind(ID3D11DeviceContext* context) override{

        UINT Stride = sizeof(T) * 1;
        UINT Offset = 0;

        context->IASetVertexBuffers(0, 1, vVertexBuffer.GetAddressOf(), &Stride, &Offset);
    }


private:

    Microsoft::WRL::ComPtr<ID3D11Buffer> vVertexBuffer;
     int vertexBufferCount;

};