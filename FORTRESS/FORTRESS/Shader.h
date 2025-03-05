#pragma once
class Shader {
public:
	Shader(ID3D11Device* device) : _device(device) {};
	virtual ~Shader() {
		_blob->Release();
	};

	virtual void Create(const std::wstring& path, const std::string& name, const std::string& version) abstract;
	ID3DBlob* GetBlob() { return _blob; }

protected:
	void LoadShaderFromFile(const std::wstring& path, const std::string& name, const std::string& version);

protected:
	std::wstring _path;
	std::string _name;
	ID3D11Device* _device;
	ID3DBlob* _blob;
};

class VertexShader : public Shader {
	using Super = Shader;
public:
	VertexShader(ID3D11Device* device) : Super(device) {};
	~VertexShader() {
		_vertexShader->Release();
	};

	void Create(const std::wstring& path, const std::string& name, const std::string& version) override;
	ID3D11VertexShader* Get() { return _vertexShader; }

private:
	ID3D11VertexShader* _vertexShader;
};

class PixelShader : public Shader {
	using Super = Shader;
public:
	PixelShader(ID3D11Device* device) : Super(device) {};
	~PixelShader() {
		_pixelShader->Release();
	};

	void Create(const std::wstring& path, const std::string& name, const std::string& version) override;
	ID3D11PixelShader* Get() { return _pixelShader; }

private:
	ID3D11PixelShader* _pixelShader;
};
