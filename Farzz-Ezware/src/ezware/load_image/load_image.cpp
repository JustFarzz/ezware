#define _CRT_SECURE_NO_WARNINGS
#define STB_IMAGE_IMPLEMENTATION

#include "load_image.h"
#include "../../../include/stb/stb_image.h"



bool LoadTextureFromMemory(DX11Device* device, const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
    int e_image_Width = 0;
    int e_image_Height = 0;

    unsigned char* image_data = stbi_load_from_memory((const unsigned char*)data, (int)data_size, &e_image_Width, &e_image_Height, NULL, 4);
    if (image_data == NULL)
        return false;

    D3D11_TEXTURE2D_DESC desc;

    ZeroMemory(&desc, sizeof(desc));
    desc.Width = e_image_Width;
    desc.Height = e_image_Height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.CPUAccessFlags = 0;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    ID3D11Texture2D* pTexture = NULL;
    D3D11_SUBRESOURCE_DATA subResource;
    subResource.pSysMem = image_data;
    subResource.SysMemPitch = desc.Width * 4;
    subResource.SysMemSlicePitch = 0;
    device->GetDevice()->CreateTexture2D(&desc, &subResource, &pTexture);

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = desc.MipLevels;
    srvDesc.Texture2D.MostDetailedMip = 0;
    device->GetDevice()->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
    pTexture->Release();

    *out_width = e_image_Width;
    *out_height = e_image_Height;
    stbi_image_free(image_data);

    return true;
}

bool LoadTextureFromFile(DX11Device* device, const char* file_name, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
{
    FILE* f = fopen(file_name, "rb");
    if (f == NULL)
        return false;
    fseek(f, 0, SEEK_END);
    size_t file_size = (size_t)ftell(f);
    if (file_size == -1)
        return false;
    fseek(f, 0, SEEK_SET);
    void* file_data = IM_ALLOC(file_size);
    fread(file_data, 1, file_size, f);
    bool ret = LoadTextureFromMemory(device, file_data, file_size, out_srv, out_width, out_height);
    IM_FREE(file_data);
    return ret;
}