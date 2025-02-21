#include "../ezware.h"

bool LoadTextureFromMemory(DX11Device* device, const void* data, size_t data_size, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
bool LoadTextureFromFile(DX11Device* device, const char* file_name, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);