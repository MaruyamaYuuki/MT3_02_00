#include <Novice.h>
#include<imgui.h>
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Expantion4x4.h"
#include "ExpantionVector3.h"
#include <array>
#include <cstdint>

const char kWindowTitle[] = "LE2C_25_マルヤマ_ユウキ_MT3_02_00";

const int kWindowWidth = 1280;
const int kWindowHeight = 720;

Expantion4x4* expantion4x4_ = new Expantion4x4();
ExpantionVector3* expantionVector3_ = new ExpantionVector3();


struct Sphere {
	Vector3 center;
	float radius;
};
struct Plane {
	Vector3 normal;
	float distance;
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

struct Triangle {
	Vector3 vertices[3];
};

struct AABB {
	Vector3 min; //!< 最小点
	Vector3 max; //!< 最大点
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
	const uint32_t kSubdivision = 20;    // 分割数
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

Vector3 Project(const Vector3& v1, const Vector3& v2) {
	// v1とv2の内積を計算
	float dotProduct = expantionVector3_->Dot(v1, v2);
	// v2とv2の内積を計算
	float v2lengthSquared = expantionVector3_->Dot(v2, v2);
	// 比率を計算
	float scalar = dotProduct / v2lengthSquared;
	// 比率をv2に掛ける
	return expantionVector3_->Multiply(scalar, v2);
}
Vector3 ClosestPoint(const Vector3& point, const Segment& segment) {
	// 線分をパラメーター化
	// t = [(point - origin)・diff] / |diff|^2
	float t = ((point.x - segment.origin.x) * segment.diff.x +
		(point.y - segment.origin.y) * segment.diff.y +
		(point.z - segment.origin.z) * segment.diff.z) /
		(segment.diff.x * segment.diff.x +
			segment.diff.y * segment.diff.y +
			segment.diff.z * segment.diff.z);

	// パラメーター t が [0, 1] の範囲外にある場合、線分の端点が最近接点
	if (t < 0.0f) {
		return segment.origin;  // 線分の始点が最近接点
	}
	else if (t > 1.0f) {
		return Vector3{ segment.origin.x + segment.diff.x,
					   segment.origin.y + segment.diff.y,
					   segment.origin.z + segment.diff.z };  // 線分の終点が最近接点
	}

	// 線分上の点を計算
	float closestX = segment.origin.x + t * segment.diff.x;
	float closestY = segment.origin.y + t * segment.diff.y;
	float closestZ = segment.origin.z + t * segment.diff.z;

	// 最近接点のベクトルを返す
	return Vector3{ closestX, closestY, closestZ };
}

Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result;
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

Vector3 Prependicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return{ -vector.z,vector.x,0.0f };
	}
	return { 0.0f,-vector.z,vector.y };
}

void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 center = expantionVector3_->Multiply(plane.distance, plane.normal);
	Vector3 perpendiculars[4];
	perpendiculars[0] = expantionVector3_->Normalize(Prependicular(plane.normal));
	perpendiculars[1] = { -perpendiculars[0].x,-perpendiculars[0].y,-perpendiculars[0].z };
	perpendiculars[2] = Cross(plane.normal, perpendiculars[0]);
	perpendiculars[3] = { -perpendiculars[2].x,-perpendiculars[2].y,-perpendiculars[2].z };

	Vector3 points[4];
	for (uint32_t index = 0; index < 4; ++index) {
		Vector3 extend = expantionVector3_->Multiply(2.0f, perpendiculars[index]);
		Vector3 point = expantionVector3_->Add(center, extend);
		points[index] = expantion4x4_->Transform(expantion4x4_->Transform(point, viewProjectionMatrix), viewportMatrix);
	}

	Novice::DrawLine(int(points[0].x), int(points[0].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[2].x), int(points[2].y), color);
	Novice::DrawLine(int(points[3].x), int(points[3].y), int(points[1].x), int(points[1].y), color);
	Novice::DrawLine(int(points[3].x), int(points[3].y), int(points[0].x), int(points[0].y), color);
}

void DrawTriangle(const Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 points[3];
	for (uint32_t index = 0; index < 3; ++index) {
		points[index] = expantion4x4_->Transform(expantion4x4_->Transform(triangle.vertices[index], viewProjectionMatrix), viewportMatrix);
	}

	Novice::DrawTriangle(int(points[0].x), int(points[0].y), int(points[1].x), int(points[1].y), int(points[2].x), int(points[2].y), color, kFillModeWireFrame);
}

void DrawLine(const Segment& segment, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 screenStart = expantion4x4_->Transform(expantion4x4_->Transform(segment.origin, viewProjectionMatrix), viewportMatrix);
	Vector3 screenEnd = expantion4x4_->Transform(expantion4x4_->Transform(expantionVector3_->Add(segment.origin, segment.diff), viewProjectionMatrix), viewportMatrix);

	Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), color);
}


void DrawAABB(const AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	// AABBの8つの頂点を計算
	Vector3 v0 = aabb.min;
	Vector3 v1 = { aabb.max.x, aabb.min.y, aabb.min.z };
	Vector3 v2 = { aabb.min.x, aabb.max.y, aabb.min.z };
	Vector3 v3 = { aabb.max.x, aabb.max.y, aabb.min.z };
	Vector3 v4 = { aabb.min.x, aabb.min.y, aabb.max.z };
	Vector3 v5 = { aabb.max.x, aabb.min.y, aabb.max.z };
	Vector3 v6 = { aabb.min.x, aabb.max.y, aabb.max.z };
	Vector3 v7 = aabb.max;

	// エッジを定義
	std::array<Segment, 12> edges = {
		Segment{v0, v1 - v0}, Segment{v1, v3 - v1}, Segment{v3, v2 - v3}, Segment{v2, v0 - v2},
		Segment{v4, v5 - v4}, Segment{v5, v7 - v5}, Segment{v7, v6 - v7}, Segment{v6, v4 - v6},
		Segment{v0, v4 - v0}, Segment{v1, v5 - v1}, Segment{v2, v6 - v2}, Segment{v3, v7 - v3}
	};

	// 各エッジを描画
	for (const auto& edge : edges) {
		DrawLine(edge, viewProjectionMatrix, viewportMatrix, color);
	}
}

bool IsCollision(const AABB& aabb1, const AABB& aabb2) {
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) &&
		(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z)) {
		return true;
	}
	return false;
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

	Segment segment{
		{0.0f,0.5f,-1.0f},
		{0.0f,0.5f,2.0f}
	};
	Triangle triangle{
		{{-1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{1.0f,0.0f,0.0f}}
	};

	AABB aabb1{
		.min{-0.5f,-0.5f,-0.5f},
		.max{0.0f,0.0f,0.0f},
	};
	AABB aabb2{
		.min{0.2f,0.2f,0.2f},
		.max{1.0f,1.0f,1.0f},
	};

	uint32_t color1 = WHITE;
	uint32_t color2 = WHITE;

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
		Matrix4x4 viewPortMatrix = expantion4x4_->MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		if (IsCollision(aabb1,aabb2)) {
			color1=RED;
		}
		else {
			color1=WHITE;
		}

		ImGui::Begin("Window");
		ImGui::DragFloat3("aabb1.min", &aabb1.min.x, 0.01f);
		ImGui::DragFloat3("aabb1.max", &aabb1.max.x, 0.01f);
		ImGui::DragFloat3("aabb2.min", &aabb2.min.x, 0.01f);
		ImGui::DragFloat3("aabb2.max", &aabb2.max.x, 0.01f);
		ImGui::End();

		aabb1.min.x = (std::min)(aabb1.min.x, aabb1.max.x);
		aabb1.max.x = (std::max)(aabb1.min.x, aabb1.max.x);
		aabb1.min.y = (std::min)(aabb1.min.y, aabb1.max.y);
		aabb1.max.y = (std::max)(aabb1.min.y, aabb1.max.y);
		aabb1.min.z = (std::min)(aabb1.min.z, aabb1.max.z);
		aabb1.max.z = (std::max)(aabb1.min.z, aabb1.max.z);

		aabb2.min.x = (std::min)(aabb2.min.x, aabb2.max.x);
		aabb2.max.x = (std::max)(aabb2.min.x, aabb2.max.x);
		aabb2.min.y = (std::min)(aabb2.min.y, aabb2.max.y);
		aabb2.max.y = (std::max)(aabb2.min.y, aabb2.max.y);
		aabb2.min.z = (std::min)(aabb2.min.z, aabb2.max.z);
		aabb2.max.z = (std::max)(aabb2.min.z, aabb2.max.z);

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		DrawGrid(worldViewProjectionMatrix, viewPortMatrix);

		DrawAABB(aabb1, worldViewProjectionMatrix, viewPortMatrix, color1);

		DrawAABB(aabb2, worldViewProjectionMatrix, viewPortMatrix, color2);

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
