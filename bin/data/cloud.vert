// 구름메쉬에 적용할 버텍스 셰이더를 새로 하나 파줌.

#version 410

layout(location = 0) in vec3 pos;
layout(location = 3) in vec2 uv;

uniform mat4 transform; // ofApp.cpp 에서 전달받은 구름메쉬의 4*4 변환행렬을 저장하는 유니폼 변수
out vec2 fragUV;

void main() {
  // 버텍스 위치값 pos에 transform 을 곱해줌으로써 크기, 회전, 이동 연산을 한 줄로 끝내버림 
  // -> 코드의 간결함 + 3차원 회전 계산 용이 + GPU 계산 최적화 의 장점을 모두 갖춤. (참고로 GPU는 행렬계산에 최적화 되어있음.)
  gl_Position = transform * vec4(pos, 1.0);

  fragUV = vec2(uv.x, 1.0 - uv.y);
}

/*
  버텍스 셰이더에서 회전 처리 계산은
  이동이나 스케일 계산보다는 복잡하지만, 
  구체적으로 설명할 필요는 없음.

  왜냐면, 실제로 이런 연산은 변환 행렬을 이용해서 
  한 줄의 코드로 처리하는 간단한 공식을 사용할 것이기 때문!

  대신 여기서 기억해야 할 것은,
  항상 크기 연산 먼저,
  그 다음 회전 연산,
  마지막으로 이동 연산

  이 순서로 연산을 해줘야 한다는 게 중요함!
*/