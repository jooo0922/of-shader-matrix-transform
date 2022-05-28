#include "ofApp.h"

// 캐릭터 텍스쳐를 입힐 쿼드 메쉬 생성 함수를 따로 밖으로 빼서 정리함.
void buildMesh(ofMesh& mesh, float w, float h, glm::vec3 pos) {
    // 캐릭터 메쉬를 위치시키려는 vec3 pos 값과 width, height 값으로 계산하여
    // 4개 버텍스의 x, y, z 좌표값들을 verts 라는 float 배열에 담아놓음.
    float verts[] = {
        -w + pos.x, -h + pos.y, pos.z,
        -w + pos.x, h + pos.y, pos.z,
        w + pos.x, h + pos.y, pos.z,
        w + pos.x, -h + pos.y, pos.z,
    };
    
    // 4개 버텍스의 uv좌표값들을 uvs 라는 float 배열에 담아놓음
    float uvs[] = {
        0, 0,
        0, 1,
        1, 1,
        1, 0
    };
    
    // for loop 를 돌면서 verts, uvs 배열에 담긴 좌표값들을 꺼내와서 인자로 전달받은 ofMesh 에 버텍스 및 uv좌표를 추가해 줌.
    for (int i = 0; i < 4; ++i) {
        int idx = i * 3;
        int uvIdx = i * 2;
        mesh.addVertex(glm::vec3(verts[idx], verts[idx + 1], verts[idx + 2]));
        mesh.addTexCoord(glm::vec2(uvs[uvIdx], uvs[uvIdx + 1]));
    }
    
    // 4개의 버텍스 순서를 인덱싱해서 어떻게 쿼드 메쉬의 삼각형들을 그려줄 것인지 결정함.
    ofIndexType indices[6] = {0, 1, 2, 2, 3, 0};
    mesh.addIndices(indices, 6);
};

// 이동, 회전 각도, 크기 벡터를 받아 세 개를 모두 합친 변환행렬을 리턴해주는 함수
glm::mat4 buildMatrix(glm::vec3 trans, float rot, glm::vec3 scale) {
    using glm::mat4; // 이후 코드에서 등장하는 mat4 키워드는 glm 라이브러리의 mat4 타입이라고 선언하는 거겠지
    
    mat4 translation = glm::translate(trans); // glm 이동행렬 함수로 이동행렬을 만듦
    mat4 rotation = glm::rotate(rot, glm::vec3(0.0, 0.0, 1.0)); // glm 회전행렬 함수로 회전행렬을 만듦 (인자 2개는 각각 회전각도, 회전축)
    mat4 scaler = glm::scale(scale); // glm 크기행렬 함수로 크기행렬을 만듦.
    
    /**
     3개의 행렬을 곱해서 하나로 합침.
     
     glm 라이브러리는 열 우선 행렬을 따르므로,
     원하는 행렬 계산 순서 (1. 크기 먼저, 2. 회전 그 다음, 3. 이동 마지막)의
     반대 순서로 곱해줘서 합쳐야 함.
     */
    return translation * rotation * scaler; // 최종적으로 곱셈하여 합친 변환행렬 리턴
}

// 카메라 위치, 회전값을 프로퍼티로 갖는 CameraDat 타입 변수 cam 을 인자로 받은 뒤, 뷰행렬을 생성해 리턴해주는 함수.
glm::mat4 buildViewMatrix(CameraData cam) {
    using namespace glm; // 이제부터 현재 블록 내에서 glm 라이브러리 키워드는 'glm::' 을 생략해서 사용해도 됨.
    
    /**
     우선 상단의 모델행렬 계산에 사용되던 buildMatrix 함수를 가져와서 뷰행렬 계산에 사용함.
     
     '뷰행렬'은 카메라 움직임에 따라 좌표계의 나머지 대상들을 정확히 카메라 반대방향으로 움직이기 위한
     변환행렬이므로, 대상들을 움직일 때 사용하는 변환행렬인 '모델행렬' 을 그대로 사용할 수는 없음.
     
     그래서 inverse() 오픈프레임웍스 내장함수로
     리턴받은 모델행렬의 역행렬을 구해서 최종적인 뷰행렬을 리턴해주는 것.
     */
    return inverse(buildMatrix(cam.position, cam.rotation, vec3(1, 1, 1))); // 카메라 위치, 카메라 회전값, 카메라 크기(크기는 의미가 없으므로, (1, 1, 1)로 고정이랬지?)을 전달.
}

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex(); // 스크린 픽셀 좌표를 사용하는 텍스쳐 관련 오픈프레임웍스 레거시 지원 설정 비활성화
    ofEnableDepthTest(); // 깊이테스트를 활성화하여 z좌표값을 깊이버퍼에 저장해서 z값을 기반으로 앞뒤를 구분하여 렌더링할 수 있도록 함.
    
    buildMesh(charMesh, 0.1, 0.2, glm::vec3(0.0, -0.2, 0.0)); // 캐릭터메쉬 생성
    // buildMesh(backgroundMesh, 1.0, 1.0, glm::vec3(0.0, 0.0, 0.5)); // 배경메쉬 생성
    buildMesh(backgroundMesh, 1.0, 1.0, glm::vec3(0.0, 0.0, -0.5)); // 아래 draw() 에서 직교투영행렬의 near, far값에 의해 프러스텀의 z축 범위가 0 ~ -10 으로 설정됨에 따라, 배경메쉬의 z값이 0.5가 되면 프러스텀을 벗어나서 화면에 렌더가 안됨. 그러므로, 배경메쉬 생성 시 버텍스의 z값을 -0.5 로 바꿔서 하던지, 아니면 초기에 0으로 설정을 해놓고 draw() 함수에서 translate(0.0, 0.0, -0.5) 이런식으로 z값만 -0.5만큼 이동시키는 이동변환행렬을 만들어서 배경메쉬가 사용하는 버텍스 셰이더에 쏴줘서 적용할 수 있도록 하던지 해야 함.
    buildMesh(cloudMesh, 0.25, 0.15, glm::vec3(0.0, 0.0, 0.0)); // 구름메쉬 생성
    buildMesh(sunMesh, 1.0, 1.0, glm::vec3(0.0, 0.0, 0.4)); // 태양메쉬 생성
    
    alienImg.load("walk_sheet.png"); // 캐릭터메쉬에 사용할 스프라이트시트 텍스쳐 로드
    backgroundImg.load("forest.png"); // 배경메쉬에 사용할 텍스쳐 로드
    cloudImg.load("cloud.png"); // 구름메쉬에 사용할 텍스쳐 로드
    sunImg.load("sun.png"); // 태양메쉬에 사용할 텍스쳐 로드
    
    // 모든 메쉬를 변환행렬로 처리하기 위해 passthrough.vert 를 cloud.vert 처럼 변환행렬로 곱해주는 로직으로 변경하고, 버텍스 셰이더를 통일함.
    spritesheetShader.load("spritesheet.vert", "alphaTest.frag"); // 스프라이트시트 기법을 사용할 캐릭터메쉬에 적용할 셰이더 파일 로드
    alphaTestShader.load("passthrough.vert", "alphaTest.frag"); // 알파테스트 셰이더를 사용할 메쉬들에 적용할 셰이더 파일 로드
    cloudShader.load("passthrough.vert", "cloud.frag"); // 구름메쉬에 적용할 셰이더 파일 로드
}

//--------------------------------------------------------------
void ofApp::update(){
    if (walkRight) { // 오른쪽 화살표 키 입력을 감지하여 true 이면 조건문 블록을 수행함.
        float speed = 0.5 * ofGetLastFrameTime(); // 이전 프레임과 현재 프레임의 시간 간격인 '델타타임'을 가져와서 속도값을 구함.
        charPos += glm::vec3(speed, 0, 0); // 속도값 만큼을 x좌표에 더해서 charPos 값을 누적계산함.
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    using namespace glm; // 하단에서 buildMatrix() 함수로 변환행렬 계산 후 리턴받는 코드 작성 시, 'glm::' 을 안붙이고도 mat4, vec3 등의 변수타입을 사용할 수 있도록 한 것.
    
    cam.position = vec3(-1, 0, 0); // 매 프레임마다 카메라 위치 데이터를 (-1, 0, 0) 으로 할당함. (x축 방향으로 -1이면, NDC 좌표계의 x축이 -1 ~ 1 사이니까 정확히 x축 방향으로 왼쪽으로 전체 좌표계의 절반만큼 움직이도록 한 것. )
    // mat4 view = buildViewMatrix(cam); // 위치 데이터가 할당된 CameraData 타입의 변수를 인자로 넘겨서 뷰행렬을 리턴받음.
    mat4 view = mat4(); // 투영행렬 적용결과를 관찰하기 위해, 뷰행렬을 단위행렬로 바꿔줌으로써 카메라를 움직이지 않도록 함. (glm::mat4() 를 그냥 호출하면 단위행렬이 나온다고 했었지?)
    
    /**
     glm 라이브러리의 ortho() 함수를 이용해서 직교투영에 사용할 투영행렬을 리턴받음.
     
     이 때, left, right 값인 첫 번째와 두 번째 인자의 값을 -1, 1이 아닌 -1.33, 1.33 으로 해줘야 함.
     왜냐하면, 지금 직교투영을 하는 목적 자체가, 배경메쉬를 예로 들어 설명하자면
     setuo 함수에서 배경메쉬 생성할 때 배경메쉬의 width, height 을 둘 다 1씩 줘서
     정사각형 메쉬로 만들기로 했는데, 실제 화면에 렌더링되는건 직사각형이잖아?
     
     왜냐하면, 현재 NDC 좌표계를 기준으로 예제를 구현하고 있는데,
     NDC 좌표계는 실행창의 왼쪽 끝과 오른쪽 끝을 각각 -1, 1 로 인식하고, 아래, 위도 각각 -1, 1 로 인식하기 때문에
     실행창의 사이즈 따라서 배경메쉬도 똑같이 늘어날 수 밖에 없는거임. 아무리 정사각형으로 정의했다고 해도...
     
     그래서, 투영행렬을 곱해서 변환한 클립공간을 만들 때에는,
     실행창의 가로/세로비, 즉 종횡비(aspect ratio) 만큼을 곱해줘서
     가로방향을 늘리던지, 아니면 세로방향을 줄이던지 해야 함.
     
     그래서 가로방향을 늘리기 위해 투영행렬에서 left, right의 값에
     main.cpp 에서 설정한 실행창 해상도인 1024/768 의 종횡비 1.33 을 곱해준 것임.
     
     또한, 근평면(near) 와 원평면(far) 값을 각각 0, 10 으로 설정했는데
     얘는 프러스텀(클립공간)의 z축 상의 범위를 near 에서 시작해서 마이너스 방향으로 far까지 설정하는 것임.
     
     즉, 이 투영행렬에 의해 변환된 프러스텀, 즉 클립공간은
     z축 상에서 0 에서 시작해서 -10에서 끝난다는 뜻이고,
     이 범위를 벗어나는 메쉬들은 렌더링되지 않을 거라는 의미임.
     */
    mat4 proj = glm::ortho(-1.33f, 1.33f, -1.0f, 1.0f, 0.0f, 10.0f);
    
    ofDisableBlendMode(); // 이전 프레임에서 사용한 블렌딩 모드를 비활성화함.
    ofEnableDepthTest(); // 구름메쉬의 투명픽셀에 의해 태양메쉬가 가리는 문제를 해결하고자 밑에서 비활성화했던 깊이테스트를 다시 활성화함. (캐릭터메쉬, 배경메쉬는 깊이를 구분해줘야 하니까)
    
    static float frame = 0.0; // 프레임 변수 초기화
    frame = (frame > 10) ? 0.0 : frame += 0.2; // frame의 정수부분이 5번의 draw() 함수 호출 이후 바뀌도록 프레임 계산
    glm::vec2 spriteSize = glm::vec2(0.28, 0.19); // 스프라이트시트 텍스쳐 사이즈를 프레임 하나 만큼의 사이즈로 조절할 때 필요한 값
    glm::vec2 spriteFrame = glm::vec2((int)frame % 3, (int)frame / 3); // 스프라이트시트 텍스쳐 offset(각각 u, v 방향으로) 적용 시 사용할 vec2값
    
    // spritesheetShader 바인딩하여 사용 시작
    spritesheetShader.begin();
    
    spritesheetShader.setUniform2f("size", spriteSize); // 텍스쳐 사이즈 조절값 유니폼 변수에 전송
    spritesheetShader.setUniform2f("offset", spriteFrame); // 프레임 offset 값 유니폼 변수에 전송
    spritesheetShader.setUniformTexture("tex", alienImg, 0); // 스프라이트시트 텍스쳐 이미지 유니폼 변수에 전송
    spritesheetShader.setUniformMatrix4f("model", translate(charPos)); // 캐릭터메쉬의 이동도 변환행렬로 처리해야 하므로, 키 입력에 따라 갱신되는 vec3 charPos 벡터로 이동행렬을 만들어서 변환행렬(이제부터는 '모델행렬' 이라고 부를 것!) 유니폼 변수에 전송해 줌.
    spritesheetShader.setUniformMatrix4f("view", view); // 카메라 움직임의 정반대 방향으로 캐릭터메쉬의 움직임을 조정하는 뷰행렬도 유니폼 변수에 전송해 줌.
    spritesheetShader.setUniformMatrix4f("proj", proj); // 뷰 좌표(공간)에 곱해줘서 클립공간으로 변환하기 위해 필요한 투영행렬도 유니폼 변수에 전송해 줌.
    charMesh.draw(); // 캐릭터메쉬 드로우콜 호출하여 그려줌
    
    spritesheetShader.end();
    // spritesheetShader 사용 중단
    
    
    // alphaTestShader 바인딩하여 사용 시작
    alphaTestShader.begin();
    
    alphaTestShader.setUniformTexture("tex", backgroundImg, 0); // 배경 텍스쳐 이미지 유니폼 변수에 전송
    alphaTestShader.setUniformMatrix4f("model", glm::mat4()); // 배경메쉬도 변환행렬 (이제부터는 '모델행렬') 로 처리함. 배경메쉬는 아무런 변환이 필요 없으므로, glm::mat4() 호출을 통해 간단하게 '단위행렬'을 생성한 뒤 유니폼 변수에 전송함. 이처럼, 단위행렬은 별도의 변환이 필요없는 배경메쉬 등에 유용하게 사용됨.
    alphaTestShader.setUniformMatrix4f("view", view); // 카메라 움직임의 정반대 방향으로 배경메쉬의 움직임을 조정하는 뷰행렬도 유니폼 변수에 전송해 줌.
    alphaTestShader.setUniformMatrix4f("proj", proj); // 뷰 좌표(공간)에 곱해줘서 클립공간으로 변환하기 위해 필요한 투영행렬도 유니폼 변수에 전송해 줌.
    backgroundMesh.draw(); // 배경메쉬 드로우콜 호춣여 그려줌
    
    alphaTestShader.end();
    // alphaTestShader 사용 중단
    
    
    ofDisableDepthTest(); // 투명 픽셀이 깊이버퍼값을 가져서 뒤에 있던 태양메쉬가 가려지지 않도록, 태양메쉬와 구름메쉬를 그리기 전 깊이테스트 비활성화
    ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_ALPHA); // 새로운 프래그먼트의 알파값을 이용해서 백 버퍼의 프래그먼트와 색상값을 섞어주는 알파 블렌딩 모드 활성화
    
    // 변환행렬로 회전 애니메이션을 구현하기 위해 매 프레임마다 회전행렬 생성에 필요한 각도값을 갱신함.
    static float rotation = 1.0f; // 매 프레임마다 갱신할 회전행렬의 각도값
    rotation += 1.0f * ofGetLastFrameTime(); // 이전 프레임과 현재 프레임의 시간 간격인 '델타타임'만큼 매 프레임마다 각도값을 더해줌.
    
    // 위에 glm 네임스페이스를 지정해줬기 때문에 mat4, vec3 등의 타입을 바로 사용할 수 있음.
    
    // 변환행렬 A 는 회전행렬을 제외하고 연산하기 위해 buildMatrix() 를 거치지 않고 직접 만들어 줌.
    mat4 translationA = translate(vec3(-0.55, 0.0, 0.0)); // 이동행렬
    mat4 scaleA = scale(vec3(1.5, 1, 1)); // 크기행렬
    mat4 transformA = translationA * scaleA; // 크기연산 -> 이동연산 순으로 처리하는 변환핸렬
    
    // 위 변환행렬 A 에 원하는 회전행렬을 적용함
    // 이 때, 정반대 이동행렬인 translationA 의 역행렬을 곱해줘서 transformA 에서의 이동연산을 상쇄시킴.
    // 크기연산 -> 회전연산 -> 이동연산 순으로 처리해주려면 중간에 낀 이동연산을 정반대 이동연산으로 상쇄시켜줘야 함!
    mat4 ourRotation = rotate(rotation, vec3(0.0, 0.0, 1.0)); // 원하는 회전행렬 (매 프레임마다 갱신됨)
    mat4 newMatrix = translationA * ourRotation * inverse(translationA); // 정반대 이동연산(glm 내장함수 inverse() 로 역행렬을 구함) -> 원하는 회전연산 -> 이동연산 순으로 처리하는 변환행렬
    mat4 finalMatrixA = newMatrix * transformA; // 첫번째 구름메쉬에 최종적으로 적용할 변환행렬 (크기연산 -> (이동연산 -> 정반대 이동연산 (상쇄됨)) -> 원하는 회전연산 -> 이동연산) 순으로 처리됨.
    
    mat4 transformB = buildMatrix(vec3(0.4, 0.2, 0.0), 1.0f, vec3(1, 1, 1)); // 버텍스 셰이더로 직접 계산했던 두 번째 구름메쉬의 변환행렬을 계산하여 리턴받음.
    
    
    // cloudShader 바인딩하여 사용 시작
    cloudShader.begin();
    
    cloudShader.setUniformTexture("tex", cloudImg, 0); // 구름 텍스쳐 이미지 유니폼 변수에 전송
    cloudShader.setUniformMatrix4f("view", view); // 카메라 움직임의 정반대 방향으로 구름메쉬들의 움직임을 조정하는 뷰행렬도 유니폼 변수에 전송해 줌.
    cloudShader.setUniformMatrix4f("proj", proj); // 뷰 좌표(공간)에 곱해줘서 클립공간으로 변환하기 위해 필요한 투영행렬도 유니폼 변수에 전송해 줌.
    
    cloudShader.setUniformMatrix4f("model", finalMatrixA); // 버텍스 셰이더에 mat4 변환행렬을 받는 유니폼변수에 첫번째 구름메쉬 변환행렬(이제부터는 '모델행렬')을 전송함.
    cloudMesh.draw(); // 구름메쉬 드로우콜 호출하여 그려줌
    
    cloudShader.setUniformMatrix4f("model", transformB); // 버텍스 셰이더에 mat4 변환행렬을 받는 유니폼변수에 두번째 구름메쉬 변환행렬(이제부터는 '모델행렬')을 전송함.
    cloudMesh.draw(); // 구름메쉬 드로우콜 호출하여 그려줌 -> 하나의 메쉬를 가지고 여러 번 드로우콜을 호출하여 화면 상에 여러 개의 메쉬를 복사하여 그려줄 수 있음.
    
    cloudShader.end();
    // cloudShader 사용 중단
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ofKey::OF_KEY_RIGHT) {
        walkRight = true; // 오른쪽 화살표 키 입력 감지 시 walkRight 을 활성화함.
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == ofKey::OF_KEY_RIGHT) {
        walkRight = false; // 오른쪽 화살표 키 입력을 뗏을 때 walkRight 을 비활성화함.
    }
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
