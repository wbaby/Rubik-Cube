#include "Rubik.h"
#include "d3dUtil.h"
#include "Vertex.h"
using namespace DirectX;
using namespace Microsoft::WRL;



DirectX::XMMATRIX Cube::GetWorldMatrix() const
{
	XMVECTOR posVec = XMLoadFloat3(&pos);
	// rotation��Ȼ���ֻ��һ�������Ƿ�0����֤��ֻ��������һ���������ת
	XMMATRIX R = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	posVec = XMVector3TransformCoord(posVec, R);
	// ������ת�������յ�λ��
	XMFLOAT3 finalPos;
	XMStoreFloat3(&finalPos, posVec);

	return XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
		XMMatrixTranslation(finalPos.x, finalPos.y, finalPos.z);
}

Rubik::Rubik()
	: mRotationSpeed(XM_2PI)
{
}

void Rubik::InitResources(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
{

	// ��ʼ����������
	mTexArray = CreateDDSTexture2DArrayFromFile(
		device,
		deviceContext,
		std::vector<std::wstring>{
			L"Resource/Black.dds",
			L"Resource/Orange.dds",
			L"Resource/Red.dds",
			L"Resource/Green.dds",
			L"Resource/Blue.dds",
			L"Resource/Yellow.dds",
			L"Resource/White.dds",
	});

	//
	// ��ʼ������������ģ��
	//

	VertexPosTex vertices[] = {
		// +X��
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		// -X��
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		// +Y��
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		// -Y��
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		// +Z��
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		// -Z��
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
	};

	// ���ö��㻺��������
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof vertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// �½����㻺����
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertices;
	HR(device->CreateBuffer(&vbd, &initData, mVertexBuffer.ReleaseAndGetAddressOf()));
	

	WORD indices[] = { 0, 1, 2, 2, 3, 0 };
	// ������������������
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof indices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// �½�����������
	initData.pSysMem = indices;
	HR(device->CreateBuffer(&ibd, &initData, mIndexBuffer.ReleaseAndGetAddressOf()));

	// ��ʼ��ħ��������
	Reset();

	// Ԥ�Ȱ󶨶���/��������������Ⱦ����
	UINT strides[1] = { sizeof(VertexPosTex) };
	UINT offsets[1] = { 0 };
	deviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), strides, offsets);
	deviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

}

void Rubik::Reset()
{
	mIsLocked = false;
	mIsPressed = false;

	// ��ʼ��ħ������λ�ã���������Ĭ������ɫ
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			for (int k = 0; k < 3; ++k)
			{
				mCubes[i][j][k].pos = XMFLOAT3(-2.0f + 2.0f * i,
					-2.0f + 2.0f * j, -2.0f + 2.0f * k);
				mCubes[i][j][k].rotation = XMFLOAT3();
				memset(mCubes[i][j][k].faceColors, 0, 
					sizeof mCubes[i][j][k].faceColors);
			}
	
	// +X��Ϊ��ɫ��-X��Ϊ��ɫ
	// +Y��Ϊ��ɫ��-Y��Ϊ��ɫ
	// +Z��Ϊ��ɫ��-Z��Ϊ��ɫ
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
		{
			mCubes[2][i][j].faceColors[RubikFace_PosX] = RubikFaceColor_Orange;
			mCubes[0][i][j].faceColors[RubikFace_NegX] = RubikFaceColor_Red;

			mCubes[j][2][i].faceColors[RubikFace_PosY] = RubikFaceColor_Green;
			mCubes[j][0][i].faceColors[RubikFace_NegY] = RubikFaceColor_Blue;

			mCubes[i][j][2].faceColors[RubikFace_PosZ] = RubikFaceColor_Yellow;
			mCubes[i][j][0].faceColors[RubikFace_NegZ] = RubikFaceColor_White;
		}	


}

void Rubik::Update(float dt)
{
	if (mIsLocked)
	{
		int finishCount = 0;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				for (int k = 0; k < 3; ++k)
				{
					// ��x��y, z������ת�Ƕ��𽥹�0
					// x��
					float dTheta = (signbit(mCubes[i][j][k].rotation.x) ? -1.0f : 1.0f) * dt * mRotationSpeed;
					if (fabs(mCubes[i][j][k].rotation.x) < fabs(dTheta))
					{
						mCubes[i][j][k].rotation.x = 0.0f;
						finishCount++;
					}
					else
					{
						mCubes[i][j][k].rotation.x -= dTheta;
					}
					// y��
					dTheta = (signbit(mCubes[i][j][k].rotation.y) ? -1.0f : 1.0f) * dt * mRotationSpeed;
					if (fabs(mCubes[i][j][k].rotation.y) < fabs(dTheta))
					{
						mCubes[i][j][k].rotation.y = 0.0f;
						finishCount++;
					}
					else
					{
						mCubes[i][j][k].rotation.y -= dTheta;
					}
					// x��
					dTheta = (signbit(mCubes[i][j][k].rotation.z) ? -1.0f : 1.0f) * dt * mRotationSpeed;
					if (fabs(mCubes[i][j][k].rotation.z) < fabs(dTheta))
					{
						mCubes[i][j][k].rotation.z = 0.0f;
						finishCount++;
					}
					else
					{
						mCubes[i][j][k].rotation.z -= dTheta;
					}
				}
			}
		}

		// ���з��鶼�����������ܽ���
		if (finishCount == 81)
			mIsLocked = false;
	}
}

void Rubik::Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect& effect)
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			for (int k = 0; k < 3; ++k)
			{
				effect.SetWorldMatrix(mCubes[i][j][k].GetWorldMatrix());
				for (int face = 0; face < 6; ++face)
				{
					effect.SetTexIndex(mCubes[i][j][k].faceColors[face]);
					effect.Apply(deviceContext);
					deviceContext->DrawIndexed(6, 0, 4 * face);
				}
			}
		}
	}	
}

bool Rubik::IsLocked() const
{
	return mIsLocked;
}

void Rubik::RotateX(int pos, float dTheta, bool isPressed)
{
	if (!mIsLocked)
	{
		// ���鵱ǰ�Ƿ�Ϊ���̲���
		// ������Ϊ�������̲���ʱ�Ż��������ֵΪpi/2��˲ʱֵ
		bool isKeyOp = (fabs(fabs(dTheta) - XM_PIDIV2) < 10e-5f);
		// ������������������⣬�ܾ����̵Ĳ���
		if (mIsPressed && isKeyOp)
		{
			return;
		}

		// ������ת״̬
		for (int j = 0; j < 3; ++j)
			for (int k = 0; k < 3; ++k)
				mCubes[pos][j][k].rotation.x += dTheta;

		// ������̲������
		if (!isPressed)
		{
			
			// ��ʼ������ʾ״̬
			mIsPressed = false;
			mIsLocked = true;
			
			// ���ڴ�ʱ����ת������з�����ת�Ƕȶ���һ���ģ����Դ���ȡһ�������㡣
			// �����λ��[-pi/4, pi/4)������Ҫ˳ʱ����ת90�ȵĴ���
			int times = static_cast<int>(round(mCubes[pos][0][0].rotation.x / XM_PIDIV2));
			// ����λ����ӳ�䵽[0, 3]���Լ�����С����˳ʱ����ת90�ȵĴ���
			int minTimes = (times % 4 + 4) % 4;
	
			// �������б���ת����ĳ�ʼ�Ƕ�
			for (int j = 0; j < 3; ++j)
			{
				for (int k = 0; k < 3; ++k)
				{
					// ���̰��º�ı仯
					if (isKeyOp)
					{
						// ˳ʱ����ת90��--->ʵ�������-90�ȼӵ�0��
						// ��ʱ����ת90��--->ʵ�������90�ȼ���0��
						mCubes[pos][j][k].rotation.x *= -1.0f;
					}
					// ����ͷź�ı仯
					else
					{
						// ��λ��[-pi/4, pi/4)������
						mCubes[pos][j][k].rotation.x -= times * XM_PIDIV2;
					}
				}
			}

			std::vector<XMINT2> indices1, indices2;
			GetSwapIndexArray(minTimes, indices1, indices2);
			size_t swapTimes = indices1.size();
			for (size_t i = 0; i < swapTimes; ++i)
			{
				// �������������尴���������Ľ���
				XMINT2 srcIndex = indices1[i];
				XMINT2 targetIndex = indices2[i];
				// ��Ϊ2��˳ʱ����ת����ֻ��4�ζԽǵ���
				// ������Ҫ6���ڽ�(��)�Ի�
				for (int face = 0; face < 6; ++face)
				{
					std::swap(mCubes[pos][srcIndex.x][srcIndex.y].faceColors[face],
						mCubes[pos][targetIndex.x][targetIndex.y].faceColors[
							GetTargetSwapFaceRotationX(static_cast<RubikFace>(face), minTimes)]);
				}
			}
		}
	}
}

void Rubik::RotateY(int pos, float dTheta, bool isPressed)
{
	if (!mIsLocked)
	{
		// ���鵱ǰ�Ƿ�Ϊ���̲���
		// ������Ϊ�������̲���ʱ�Ż��������ֵΪpi/2��˲ʱֵ
		bool isKeyOp = (fabs(fabs(dTheta) - XM_PIDIV2) < 10e-5f);
		// ������������������⣬�ܾ����̵Ĳ���
		if (mIsPressed && isKeyOp)
		{
			return;
		}

		for (int k = 0; k < 3; ++k)
			for (int i = 0; i < 3; ++i)
				mCubes[i][pos][k].rotation.y += dTheta;

		// ������̲������
		if (!isPressed)
		{
			// ��ʼ������ʾ״̬
			mIsPressed = false;
			mIsLocked = true;

			// ���ڴ�ʱ����ת������з�����ת�Ƕȶ���һ���ģ����Դ���ȡһ�������㡣
			// �����λ��[-pi/4, pi/4)������Ҫ˳ʱ����ת90�ȵĴ���
			int times = static_cast<int>(round(mCubes[0][pos][0].rotation.y / XM_PIDIV2));
			// ����λ����ӳ�䵽[0, 3]���Լ�����С����˳ʱ����ת90�ȵĴ���
			int minTimes = (times % 4 + 4) % 4;

			// �������б���ת����ĳ�ʼ�Ƕ�
			for (int k = 0; k < 3; ++k)
			{
				for (int i = 0; i < 3; ++i)
				{
					// ������Ϊ�������̲���ʱ�Ż��������ֵΪpi/2��˲ʱֵ
					// ���̰��º�ı仯
					if (fabs(fabs(dTheta) - XM_PIDIV2) < 10e-5f)
					{
						// ˳ʱ����ת90��--->ʵ�������-90�ȼӵ�0��
						// ��ʱ����ת90��--->ʵ�������90�ȼ���0��
						mCubes[i][pos][k].rotation.y *= -1.0f;
					}
					// ����ͷź�ı仯
					else
					{
						// ��λ��[-pi/4, pi/4)������
						mCubes[i][pos][k].rotation.y -= times * XM_PIDIV2;
					}
				}
			}

			std::vector<XMINT2> indices1, indices2;
			GetSwapIndexArray(minTimes, indices1, indices2);
			size_t swapTimes = indices1.size();
			for (size_t i = 0; i < swapTimes; ++i)
			{
				// �������������尴���������Ľ���
				XMINT2 srcIndex = indices1[i];
				XMINT2 targetIndex = indices2[i];
				// ��Ϊ2��˳ʱ����ת����ֻ��4�ζԽǵ���
				// ������Ҫ6���ڽ�(��)�Ի�
				for (int face = 0; face < 6; ++face)
				{
					std::swap(mCubes[srcIndex.y][pos][srcIndex.x].faceColors[face],
						mCubes[targetIndex.y][pos][targetIndex.x].faceColors[
							GetTargetSwapFaceRotationY(static_cast<RubikFace>(face), minTimes)]);
				}
			}
		}
	}
}

void Rubik::RotateZ(int pos, float dTheta, bool isPressed)
{
	if (!mIsLocked)
	{
		// ���鵱ǰ�Ƿ�Ϊ���̲���
		// ������Ϊ�������̲���ʱ�Ż��������ֵΪpi/2��˲ʱֵ
		bool isKeyOp = (fabs(fabs(dTheta) - XM_PIDIV2) < 10e-5f);
		// ������������������⣬�ܾ����̵Ĳ���
		if (mIsPressed && isKeyOp)
		{
			return;
		}

		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				mCubes[i][j][pos].rotation.z += dTheta;

		// ������̲������
		if (!isPressed)
		{
			// ��ʼ������ʾ״̬
			mIsPressed = false;
			mIsLocked = true;

			// ���ڴ�ʱ����ת������з�����ת�Ƕȶ���һ���ģ����Դ���ȡһ�������㡣
			// �����λ��[-pi/4, pi/4)������Ҫ˳ʱ����ת90�ȵĴ���
			int times = static_cast<int>(round(mCubes[0][0][pos].rotation.z / XM_PIDIV2));
			// ����λ����ӳ�䵽[0, 3]���Լ�����С����˳ʱ����ת90�ȵĴ���
			int minTimes = (times % 4 + 4) % 4;

			// �������б���ת����ĳ�ʼ�Ƕ�
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					// ������Ϊ�������̲���ʱ�Ż��������ֵΪpi/2��˲ʱֵ
					// ���̰��º�ı仯
					if (fabs(fabs(dTheta) - XM_PIDIV2) < 10e-5f)
					{
						// ˳ʱ����ת90��--->ʵ�������-90�ȼӵ�0��
						// ��ʱ����ת90��--->ʵ�������90�ȼ���0��
						mCubes[i][j][pos].rotation.z *= -1.0f;
					}
					// ����ͷź�ı仯
					else
					{
						// ��λ��[-pi/4, pi/4)������
						mCubes[i][j][pos].rotation.z -= times * XM_PIDIV2;
					}
				}
			}

			std::vector<XMINT2> indices1, indices2;
			GetSwapIndexArray(minTimes, indices1, indices2);
			size_t swapTimes = indices1.size();
			for (size_t i = 0; i < swapTimes; ++i)
			{
				// �������������尴���������Ľ���
				XMINT2 srcIndex = indices1[i];
				XMINT2 targetIndex = indices2[i];
				// ��Ϊ2��˳ʱ����ת����ֻ��4�ζԽǵ���
				// ������Ҫ6���ڽ�(��)�Ի�
				for (int face = 0; face < 6; ++face)
				{
					std::swap(mCubes[srcIndex.x][srcIndex.y][pos].faceColors[face],
						mCubes[targetIndex.x][targetIndex.y][pos].faceColors[
							GetTargetSwapFaceRotationZ(static_cast<RubikFace>(face), minTimes)]);
				}
			}
		}
	}
}

void Rubik::SetRotationSpeed(float rad)
{
	assert(rad > 0.0f);
	mRotationSpeed = rad;
}

ComPtr<ID3D11ShaderResourceView> Rubik::GetTexArray() const
{
	return mTexArray;
}

void Rubik::GetSwapIndexArray(int minTimes, std::vector<DirectX::XMINT2>& outArr1, std::vector<DirectX::XMINT2>& outArr2) const
{
	// ����һ��˳ʱ��90����ת�൱��ʱ�뽻��6��(���Ǻ����3��)
	// 1   2   4   2   4   2   4   1
	//   *   ->  *   ->  *   ->  *
	// 4   3   1   3   3   1   3   2
	if (minTimes == 1)
	{
		outArr1 = { XMINT2(0, 0), XMINT2(0, 1), XMINT2(0, 2), XMINT2(1, 2), XMINT2(2, 2), XMINT2(2, 1) };
		outArr2 = { XMINT2(0, 2), XMINT2(1, 2), XMINT2(2, 2), XMINT2(2, 1), XMINT2(2, 0), XMINT2(1, 0) };
	}
	// ����һ��˳ʱ��90����ת�൱��ʱ�뽻��4��(���Ǻ����2��)
	// 1   2   3   2   3   4
	//   *   ->  *   ->  *  
	// 4   3   4   1   2   1
	else if (minTimes == 2)
	{
		outArr1 = { XMINT2(0, 0), XMINT2(0, 1), XMINT2(0, 2), XMINT2(1, 2) };
		outArr2 = { XMINT2(2, 2), XMINT2(2, 1), XMINT2(2, 0), XMINT2(1, 0) };
	}
	// ����һ��˳ʱ��90����ת�൱��ʱ�뽻��6��(���Ǻ����3��)
	// 1   2   4   2   4   2   4   1
	//   *   ->  *   ->  *   ->  *
	// 4   3   1   3   3   1   3   2
	else if (minTimes == 3)
	{
		outArr1 = { XMINT2(0, 0), XMINT2(1, 0), XMINT2(2, 0), XMINT2(2, 1), XMINT2(2, 2), XMINT2(1, 2) };
		outArr2 = { XMINT2(2, 0), XMINT2(2, 1), XMINT2(2, 2), XMINT2(1, 2), XMINT2(0, 2), XMINT2(0, 1) };
	}
	// 0��˳ʱ����ת���䣬�����쳣��ֵҲ����
	else
	{
		outArr1.clear();
		outArr2.clear();
	}
	
}

RubikFace Rubik::GetTargetSwapFaceRotationX(RubikFace face, int times) const
{
	if (face == RubikFace_PosX || face == RubikFace_NegX)
		return face;
	while (times--)
	{
		switch (face)
		{
		case RubikFace_PosY: face = RubikFace_NegZ; break;
		case RubikFace_PosZ: face = RubikFace_PosY; break;
		case RubikFace_NegY: face = RubikFace_PosZ; break;
		case RubikFace_NegZ: face = RubikFace_NegY; break;
		}
	}
	return face;
}

RubikFace Rubik::GetTargetSwapFaceRotationY(RubikFace face, int times) const
{
	if (face == RubikFace_PosY || face == RubikFace_NegY)
		return face;
	while (times--)
	{
		switch (face)
		{
		case RubikFace_PosZ: face = RubikFace_NegX; break;
		case RubikFace_PosX: face = RubikFace_PosZ; break;
		case RubikFace_NegZ: face = RubikFace_PosX; break;
		case RubikFace_NegX: face = RubikFace_NegZ; break;
		}
	}
	return face;
}

RubikFace Rubik::GetTargetSwapFaceRotationZ(RubikFace face, int times) const
{
	if (face == RubikFace_PosZ || face == RubikFace_NegZ)
		return face;
	while (times--)
	{
		switch (face)
		{
		case RubikFace_PosX: face = RubikFace_NegY; break;
		case RubikFace_PosY: face = RubikFace_PosX; break;
		case RubikFace_NegX: face = RubikFace_PosY; break;
		case RubikFace_NegY: face = RubikFace_NegX; break;
		}
	}
	return face;
}