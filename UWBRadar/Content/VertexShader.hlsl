struct VertexShaderInput
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	output.pos = input.pos;
	output.color = input.color;

	return output;
}