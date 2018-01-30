#define GLM_ENABLE_EXPERIMENTAL
#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\euler_angles.hpp>
#include <glm\gtx\projection.hpp>
#include <iostream>

using namespace std;
using namespace glm;

int main() {
	//----Vectors----
	//First 6 vectors
	vec2 u1(1.0, 0.0);
	vec2 u2(1.0, 0.0);	
	vec3 v1(1.0, 0.0, 0.0);
	vec3 v2(1.0, 0.0, 0.0);
	vec4 w1(1.0, 0.0, 0.0, 1.0);
	vec4 w2(1.0, 0.0, 0.0, 1.0);

	//Second 6 vectors
	vec2 u3(2.0, 0.0);
	vec2 u4(4.0, 0.0);
	vec3 v3(2.0, 0.0, 0.0);
	vec3 v4(4.0, 0.0, 0.0);
	vec4 w3(2.0, 0.0, 0.0, 1.0);
	vec4 w4(4.0, 0.0, 0.0, 1.0);

	//2D to 3D
	vec3 u3((u3), 5.0);
	//2D to 4D
	vec3 u4((u4), 0.0, 3.0);
	//3D to 2D
	vec2 v3(v3);
	//3D to 4D
	vec4 v4(v4, 1.0);
	//4D to 2D
	vec2 w3(w3);
	//4D to 3D
	vec3 w4(w4);
	
	//-Vector Addition and Subtraction-
	//Third 6 vectors
	vec2 u5(1.0, 0.0);
	vec2 u6(2.0, 0.0);
	vec3 v5(1.0, 0.0, 0.0);
	vec3 v6(2.0, 0.0, 0.0);
	vec4 w5(1.0, 0.0, 0.0, 1.0);
	vec4 w6(2.0, 0.0, 0.0, 1.0);

	//2D + 2D
	vec2 res1 = u5 + u6;
	//2D - 2D
	vec2 res2 = u6 - u5;
	//3D + 3D
	vec3 res3 = v5 + v6;
	//3D - 3D
	vec3 res4 = v6 - v5;
	//4D + 4D
	vec4 res3 = w5 + w6;
	//4D - 4D
	vec4 res4 = w6 - w5;

	//-Vector Scaling-
	//Fourth 6 Vectors
	vec2 u7(1.0, 0.0);
	vec2 u8(2.0, 0.0);
	vec3 v7(1.0, 0.0, 0.0);
	vec3 v8(2.0, 0.0, 0.0);
	vec4 w7(1.0, 0.0, 0.0, 1.0);
	vec4 w8(2.0, 0.0, 0.0, 1.0);

	//2D * scalar
	vec2 res5 = u7 * 2.0f;
	//2D / scalar
	vec2 res6 = u8 / 3.0f;
	//3D * scalar
	vec3 res7 = v7 * 2.5f;
	//3D / scalar
	vec3 res8 = v8 / 4.5f;
	//4D * scalar
	vec4 res9 = w7 * 2.1f;
	//4D / scalar
	vec4 res10 = w8 / 2.6f;

	//-Vector Length-
	vec2 u9(1.0, 0.0);
	vec3 v9(4.0, 0.0, 0.0);
	vec4 w9(2.0, 0.0, 0.0, 1.0);
	//2D, 3D, 4D
	float L1 = length(u9);
	float L2 = length(v9);
	float L3 = length(w9);

	//-Normalizing Vectors-
	vec2 u10(1.0, 0.0);
	vec3 v10(4.0, 0.0, 0.0);
	vec4 w10(2.0, 0.0, 0.0, 1.0);
	//2D, 3D, 4D
	vec2 N1 = normalize(u10);
	vec3 N2 = normalize(v10);
	vec4 N3 = normalize(w10);

	//-Dot Product-
	vec2 u11(1.0, 0.0);
	vec3 v11(4.0, 0.0, 0.0);
	vec4 w11(2.0, 0.0, 0.0, 1.0);
	//2D dot prod
	float D1 = dot(u11, u10);
	//3D dot prod
	float D2 = dot(v10, v11);
	//4D dot prod
	float D3 = dot(w10, w11);

	//-Vector Projection-
	vec2 P1 = proj(u10, u11);
	vec3 P2 = proj(v10, v11);
	vec3 P3 = proj(w10, w11);

	//-Cross Product-
	//3D x 3D
	vec3 C1 = cross(v10, v11);

	//----Matrices----
	//4x4 Identity matrix
	mat4 m(1.0f);

	//Matrice conversion
	mat3 n(mat4); //3x3 to 4x4

	//Matrix Addition
	mat4 o(1.0f);

	mat4 n = m + o;

	//Matrix Scaling
	mat4 m = 4.0f * o;

	//Matrix Multiplication
	mat4 m1;
	mat4 m2;

	mat4 m3 = m1 * m2;

	//Matrix . Vector Multiplications
	mat4 T;
	vec3 u;
	//We can't multiply a 3D vector by a 4x4 matrix,
	//we must therefore convert to a 4D vector:
	vec4 v = T * vec4(u, 1.0f);	
	//It's always possible to retrieve thee 3D version if required:
	vec3 w = vec3(T * vec4(u, 1.0f));

	//3D vector * 4x4 Matrix
	vec4 w20;
	w20 = w10 * T;

	//-Transformations-
	mat4 T = translate(mat4(1.0f), vec3(1.0, 3.0, 0.0));

	vec3 vOne(1.0, 2.0, 0.0);
	vec3 Res;

	Res = T * vec4(vOne, 1.0);	//Convert to 4D and Translate
	Res = vec3(Res);			//Return to 3D

	//Rotation Matrix
	//We can create a rotation matrix for any axis using the same technique by
	//defining the axis we with to rotate around

	//X Rotation
	//mat4 Rx = rotate(mat4(1.0f), angle, vec3(1.0f, 0.0f, 0.0f);
	//Y Rotation
	//mat4 Ry = rotate(mat4(1.0f), angle, vec3(0.0f, 1.0f, 0.0f);
	//Z Rotation 
	//mat4 Ry = rotate(mat4(1.0f), angle, vec3(0.0f, 0.0f, 1.0f);

	//Euler Angles
	//We can also create a transformation matrix which uses all 
	//three axis in one operation using eulerAngleYXZ as fololws:
	mat4 R = eulerAngleYXZ(1, 0, 1); //Yaw, Pitch, Roll

	//Scale Matrix
	mat4 S = scale(mat4(1.0f), vec3(10, 0, 5));

	Res = vec4(vOne, 1.0f) * S;

	//Comb




}