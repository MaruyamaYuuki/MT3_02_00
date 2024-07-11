#define NOMINMAX
#include <Novice.h>
#include<imgui.h>
#include "Vector2.h"
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Expantion4x4.h"
#include "ExpantionVector3.h"
#include <array>
#include <cstdint>
#include <algorithm>

const char kWindowTitle[] = "LE2C_25_マルヤマ_ユウキ_MT3_02_00";

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

Expantion4x4* expantion4x4_ = new Expantion4x4();
ExpantionVector3* expantionVector3_ = new ExpantionVector3();

struct Sphere {
	Vector3 center;
	float radius;
};

struct Line {
	Vector3 origin;
	Vector3 diff;
};

struct Ray {
	Vector3 origin;
	Vector3 diff;
};

struct Segment {
	Vector3 origin;
	Vector3 diff;
};


void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {
	const float kGridHalfWidth = 2.0f;
	const uint32_t kSubdivision = 10;
	const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubdivision);

	for (uint32_t xIndex = 0; xIndex <= kSubdivision; ++xIndex) {
		float x = -kGridHalfWidth + (xIndex * kGridEvery);
		unsigned int color = 0xAAAAAAFF;
		Vector3 start{ x, 0.0f, -kGridHalfWidth };
		Vector3 end{ x, 0.0f, kGridHalfWidth };

		Vector3 screenStart = expantion4x4_->Transform(expantion4x4_->Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 screenEnd = expantion4x4_->Transform(expantion4x4_->Transform(end, viewProjectionMatrix), viewportMatrix);


		if (x == 0.0f) {
			color = BLACK;
		}

		Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), color);

	}

	for (uint32_t zIndex = 0; zIndex <= kSubdivision; ++zIndex) {
		float z = -kGridHalfWidth + (zIndex * kGridEvery);
		unsigned int color = 0xAAAAAAFF;
		Vector3 start{ -kGridHalfWidth, 0.0f, z };
		Vector3 end{ kGridHalfWidth, 0.0f, z };

		Vector3 screenStart = expantion4x4_->Transform(expantion4x4_->Transform(start, viewProjectionMatrix), viewportMatrix);
		Vector3 screenEnd = expantion4x4_->Transform(expantion4x4_->Transform(end, viewProjectionMatrix), viewportMatrix);


		if (z == 0.0f) {
			color = BLACK;
		}

		Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), color);
	}
}
float pi = float(M_PI);
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 16;    // 分割数
	const float kLonEvery = 2 * pi / kSubdivision;    // 経度分割1つ分の角度
	const float kLatEvery = pi / kSubdivision;    // イド分割1つ分の角度
	// 緯度の方向に分割 -π/2~π/2
	for (uint32_t latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;
		// 経度の咆哮に分割 0~π/2
		for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;
			// world座標系でのa,b,cを求める
			Vector3 a = {
				sphere.center.x + sphere.radius * cos(lat) * cos(lon),
				sphere.center.y + sphere.radius * sin(lat),
				sphere.center.z + sphere.radius * cos(lat) * sin(lon)
			};


			// 次の緯度での頂点bの計算
			Vector3 b = {
				sphere.center.x + sphere.radius * cos(lat + kLatEvery) * cos(lon),
				sphere.center.y + sphere.radius * sin(lat + kLatEvery),
				sphere.center.z + sphere.radius * cos(lat + kLatEvery) * sin(lon)
			};


			// 次の経度での頂点cの計算
			Vector3 c = {
				sphere.center.x + sphere.radius * cos(lat) * cos(lon + kLonEvery),
				sphere.center.y + sphere.radius * sin(lat),
				sphere.center.z + sphere.radius * cos(lat) * sin(lon + kLonEvery)
			};

			// a, b, cをスクリーン座標系に変換
			Vector3 screenA = expantion4x4_->Transform(a, viewProjectionMatrix);
			screenA = expantion4x4_->Transform(screenA, viewportMatrix);
			Vector3 screenB = expantion4x4_->Transform(b, viewProjectionMatrix);
			screenB = expantion4x4_->Transform(screenB, viewportMatrix);
			Vector3 screenC = expantion4x4_->Transform(c, viewProjectionMatrix);
			screenC = expantion4x4_->Transform(screenC, viewportMatrix);

			// ab, acで線を引く
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenB.x), int(screenB.y), color);
			Novice::DrawLine(int(screenA.x), int(screenA.y), int(screenC.x), int(screenC.y), color);
		}
	}
}

void DrawLine(const Segment& segment, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 screenStart = expantion4x4_->Transform(expantion4x4_->Transform(segment.origin, viewProjectionMatrix), viewportMatrix);
	Vector3 screenEnd = expantion4x4_->Transform(expantion4x4_->Transform(expantionVector3_->Add(segment.origin, segment.diff), viewProjectionMatrix), viewportMatrix);

	Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), color);
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = {0};
	char preKeys[256] = {0};

	Vector3 cameraTranslate{ 0.0f,1.9f,-6.49f };
	Vector3 cameraRotate{ 0.26f,0.0f,0.0f };

	Vector3 a{ 0.2f,1.0f,0.0f };
	Vector3 b{ 2.4f,3.1f,1.2f };
	Vector3 c = a + b;
	Vector3 d = a - b;
	Vector3 e = a * 2.4f;
	Vector3 rotate{ 0.4f,1.43f,-0.8f };
	Matrix4x4 rotateXMatrix = expantion4x4_->MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = expantion4x4_->MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = expantion4x4_->MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		Matrix4x4 worldMatrix = expantion4x4_->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f });
		Matrix4x4 cameraMatrix = expantion4x4_->MakeAffineMatrix({ 1.0f,1.0f,1.0f }, cameraRotate, cameraTranslate);
		Matrix4x4 viewMatrix = expantion4x4_->Inverse(cameraMatrix);
		Matrix4x4 projectionMatrix = expantion4x4_->MakePrespectiveFovMatrix(0.45f, float(kWindowWidth) / float(kWindowHeight), 0.1f, 100.0f);
		Matrix4x4 worldViewProjectionMatrix = expantion4x4_->Multiply(worldMatrix, expantion4x4_->Multiply(viewMatrix, projectionMatrix));
		Matrix4x4 viewportMatrix = expantion4x4_->MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		ImGui::Begin("Window");
		ImGui::Text("c:%f, %f, %f", c.x, c.y, c.z);
		ImGui::Text("d:%f, %f, %f", d.x, d.y, d.z);
		ImGui::Text("e:%f, %f, %f", e.x, e.y, e.z);
		ImGui::Text(
			"Matrix:\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f, %f\n",
			rotateMatrix.m[0][0], rotateMatrix.m[0][1], rotateMatrix.m[0][2],
			rotateMatrix.m[0][3], rotateMatrix.m[1][0], rotateMatrix.m[1][1],
			rotateMatrix.m[1][2], rotateMatrix.m[1][3], rotateMatrix.m[2][0],
			rotateMatrix.m[2][1], rotateMatrix.m[2][2], rotateMatrix.m[2][3],
			rotateMatrix.m[3][0], rotateMatrix.m[3][1], rotateMatrix.m[3][2],
			rotateMatrix.m[3][3]);
		ImGui::End();



		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		DrawGrid(worldViewProjectionMatrix, viewportMatrix);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	delete expantion4x4_;
	delete expantionVector3_;

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
