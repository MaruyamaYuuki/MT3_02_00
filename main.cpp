#include <Novice.h>
#include<imgui.h>
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Expantion4x4.h"
#include "ExpantionVector3.h"

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

bool IsCollision(const Triangle& triangle, const Segment& segment) {
	// 三角形の平面の法線を計算
	Vector3 edge1 = expantionVector3_->Subtract(triangle.vertices[1], triangle.vertices[0]);
	Vector3 edge2 = expantionVector3_->Subtract(triangle.vertices[2], triangle.vertices[0]);
	Vector3 normal = Cross(edge1, edge2);

	// 平面方程式の距離項を計算
	float d = expantionVector3_->Dot(normal, triangle.vertices[0]);

	// 線分の始点と終点の位置を計算
	float startDist = expantionVector3_->Dot(normal, segment.origin) - d;
	float endDist = expantionVector3_->Dot(normal, { segment.origin.x + segment.diff.x, segment.origin.y + segment.diff.y, segment.origin.z + segment.diff.z }) - d;

	// 線分が平面の両側にあるかどうかを判定
	if ((startDist * endDist) > 0.0f) {
		return false; // 両側にない場合は衝突しない
	}

	// 平面との交点を計算
	float t = startDist / (startDist - endDist);
	Vector3 intersection = {
		segment.origin.x + t * segment.diff.x,
		segment.origin.y + t * segment.diff.y,
		segment.origin.z + t * segment.diff.z
	};

	// 三角形の内部にあるかを確認
	for (int i = 0; i < 3; ++i) {
		Vector3 edge = expantionVector3_->Subtract(triangle.vertices[(i + 1) % 3], triangle.vertices[i]);
		Vector3 toPoint = expantionVector3_->Subtract(intersection, triangle.vertices[i]);
		Vector3 cross = Cross(edge, toPoint);
		if (expantionVector3_->Dot(normal, cross) < 0.0f) {
			return false;
		}
	}

	return true;
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


	uint32_t color = WHITE;
	uint32_t tColor = WHITE;

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

		if (IsCollision(triangle,segment)) {
			color=RED;
		}
		else {
			color=WHITE;
		}

		Vector3 screenStart = expantion4x4_->Transform(expantion4x4_->Transform(segment.origin, worldViewProjectionMatrix), viewPortMatrix);
		Vector3 screenEnd = expantion4x4_->Transform(expantion4x4_->Transform(expantionVector3_->Add(segment.origin, segment.diff), worldViewProjectionMatrix), viewPortMatrix);

		ImGui::Begin("Window");
		ImGui::DragFloat3("Triangle.v0", &triangle.vertices[0].x, 0.01f);
		ImGui::DragFloat3("Triangle.v1", &triangle.vertices[1].x, 0.01f);
		ImGui::DragFloat3("Triangle.v2", &triangle.vertices[2].x, 0.01f);
		ImGui::DragFloat3("Segment.Origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("Segment.Diff", &segment.diff.x, 0.01f);
		ImGui::End();



		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
		
		DrawGrid(worldViewProjectionMatrix, viewPortMatrix);

		DrawTriangle(triangle, worldViewProjectionMatrix, viewPortMatrix, tColor);
		Novice::DrawLine(int(screenStart.x), int(screenStart.y), int(screenEnd.x), int(screenEnd.y), color);

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
