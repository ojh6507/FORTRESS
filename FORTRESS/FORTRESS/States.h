#pragma once
class State {
public:
	State(ID3D11Device* device) : _device(device) {};
	virtual ~State() {};

protected:
	ID3D11Device* _device;
};

class SamplerState : public State {
	using Super = State;
public:
	SamplerState(ID3D11Device* device) : Super(device) {};
	~SamplerState() { _samplerState->Release(); };
	ID3D11SamplerState* Get() { return _samplerState; }

	void Create();

private:
	ID3D11SamplerState* _samplerState;
};

class RasterizerState : public State {
	using Super = State;
public:
	RasterizerState(ID3D11Device* device) : Super(device) {};
	~RasterizerState() { _rasterizerState->Release(); };
	ID3D11RasterizerState* Get() { return _rasterizerState; }

	void Create();
private:
	ID3D11RasterizerState* _rasterizerState;
};
