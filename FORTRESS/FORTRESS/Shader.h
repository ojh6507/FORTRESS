#pragma once
class Shader {
public:
	Shader(ID3D11Device* device) : _device(device) {};
	virtual ~Shader() {};

	virtual void Create(const wstring& path, const string& name, const string& version) abstract;
	ID3DBlob* GetBlob() { return _blob; }

protected:
	void LoadShaderFromFile(const wstring& path, const string& name, const string& version);

protected:
	wstring _path;
	string _name;
	ID3D11Device* _device;
	ID3DBlob* _blob;
};

class VertexShader : public Shader {
	using Super = Shader;
public:
	VertexShader(ID3D11Device* device) : Super(device) {};
	~VertexShader() {};

	void Create(const wstring& path, const string& name, const string& version) override;
	ID3D11VertexShader* Get() { return _vertexShader; }

private:
	ID3D11VertexShader* _vertexShader;
};

class PixelShader : public Shader {
	using Super = Shader;
public:
	PixelShader(ID3D11Device* device) : Super(device) {};
	~PixelShader() {};

	void Create(const wstring& path, const string& name, const string& version) override;
	ID3D11PixelShader* Get() { return _pixelShader; }

private:
	ID3D11PixelShader* _pixelShader;
};
