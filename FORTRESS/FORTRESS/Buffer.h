#pragma once
template <typename T>
class Buffer {
public:
	Buffer(ID3D11Device* device) : _device(device) {};
	virtual ~Buffer() { _buffer->Release(); };

	ID3D11Buffer* Get() { return _buffer; }
	UINT32 GetStride() { return _stride; }
	UINT32 GetOffset() { return _offset; }
	UINT32 GetCount() { return _count; }

	virtual void Create(const vector<T>&) abstract;

protected:
	ID3D11Device* _device;
	ID3D11Buffer* _buffer = nullptr;

	UINT32 _stride = 0;
	UINT32 _offset = 0;
	UINT32 _count = 0;
};

template <typename T>
class VertexBuffer : public Buffer<T> {
public:
	using Super = Buffer<T>;
	VertexBuffer(ID3D11Device* device) : Super(device) {};
	~VertexBuffer() {};

	void Create(const vector<T>& vertices);
};

template <typename T>
inline void VertexBuffer<T>::Create(const vector<T>& vertices) {
	this->_stride = sizeof(T);
	this->_count = static_cast<UINT32>(vertices.size());

	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_IMMUTABLE;			// immutable: gpu가 읽기 전용으로 접근할 수 있다.
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;	// vertex buffer로 사용하겠다.
	desc.ByteWidth = (UINT32)(this->_stride * this->_count);	// buffer 크기 지정

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = vertices.data();

	HRESULT hr = this->_device->CreateBuffer(&desc, &data, &this->_buffer);
	assert(SUCCEEDED(hr));
}

class IndexBuffer : public Buffer<UINT32> {
public:
	using Super = Buffer<UINT32>;
	IndexBuffer(ID3D11Device* device) : Super(device) {};
	~IndexBuffer() {};
	void Create(const vector<UINT32>& indices);
};

inline void IndexBuffer::Create(const vector<UINT32>& indices) {
	this->_stride = sizeof(UINT32);
	this->_offset = 0;
	this->_count = static_cast<UINT32>(indices.size());

	D3D11_BUFFER_DESC desc = {};						// buffer의 종류, 용도 등을 지정
	desc.Usage = D3D11_USAGE_IMMUTABLE;			// immutable: gpu가 읽기 전용으로 접근할 수 있다.
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;	// index buffer로 사용하겠다.
	desc.ByteWidth = (UINT32)(sizeof(UINT32) * this->_count);	// buffer 크기 지정

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = indices.data();

	HRESULT hr = this->_device->CreateBuffer(&desc, &data, &this->_buffer);
}