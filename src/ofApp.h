#pragma once

#include "ofMain.h"

// 카메라의 현재 위치, 회전값을 받는 구조체 타입 지정. (카메라 뷰 행렬 연산에서 크기값은 의미가 없으므로, 항상 (1, 1, 1) 로 가정함.)
struct CameraData {
    glm::vec3 position;
    float rotation;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
    // ofApp.cpp 에서 사용할 멤버 변수들을 헤더파일에 선언해놓음.
    ofShader alphaTestShader; // 캐릭터 및 배경 메쉬를 draw 할 때 바인딩해서 사용할 셰이더 객체 변수 선언

    ofMesh charMesh; // 캐릭터 텍스쳐를 씌워서 사용할 메쉬 객체 변수 선언
    ofImage alienImg; // 캐릭터 텍스쳐를 로드한 뒤 프래그먼트 셰이더로 전송할 때 사용할 이미지 객체 변수 선언
    
    ofMesh backgroundMesh; // 배경 텍스쳐를 씌워서 사용할 메쉬 객체 변수 선언
    ofImage backgroundImg; // 배경 텍스쳐를 로드한 뒤, 프래그먼트 셰이더로 전송할 때 사용할 이미지 객체 변수 선언
    
    ofShader cloudShader; // 알파 블렌딩 기법을 사용하기 위한 셰이더를 따로 만들어 사용할 것이므로 해당 셰이더 객체 변수 선언
    ofMesh cloudMesh; // 구름 텍스쳐를 씌워서 사용할 메쉬 객체 변수 선언
    ofImage cloudImg; // 구름 텍스쳐를 로드한 뒤, 프래그먼트 셰이더로 전송할 때 사용할 이미지 객체 변수 선언
    
    ofMesh sunMesh; // 태양 텍스쳐를 씌워서 사용할 메쉬 객체 변수 선언
    ofImage sunImg; // 태양 텍스쳐를 로드한 뒤, 프래그먼트 셰이더로 전송할 때 사용할 이미지 객체 변수 선언
    
    ofShader spritesheetShader; // 스프라이트 시트 텍스쳐의 uv 좌표값의 전체 사이즈 및 오프셋을 조절하기 위한 셰이더를 바인딩하기 위해 새로운 셰이더 객체 변수 선언
    
    // 버텍스 셰이더를 이용해 캐릭터 메쉬를 움직이기 위해 필요한 멤버변수들
    bool walkRight;
    glm::vec3 charPos;
    
    CameraData cam; // 카메라 위치 및 회전의 현재 상태값을 나타내는 구조체를 타입으로 갖는 멤버변수 cam 을 선언함.
};
