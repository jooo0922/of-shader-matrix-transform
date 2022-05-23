#version 410

layout(location = 0) in vec3 pos; // 오픈프레임웍스에서 각 버텍스 당 추가해 준 위치 데이터가 담김.
layout(location = 3) in vec2 uv; // 오픈프레임웍스에서 각 버텍스 당 추가해 준 uv 좌표 데이터가 담김.

uniform vec2 size; // 오픈프레임웍스에서 쏴준 전체적인 uv좌표값(즉, x 컴포넌트 0~1, y 컴포넌트 0~1 사이의 값)에 곱해줘서 전체 스프라이트시트 텍스쳐 중 하나의 프레임에 해당하는 부분만큼의 uv좌표값으로 변환하는데 사용할 값
uniform vec2 offset; // 위에서 size 로 uv좌표값을 변환한 뒤, 몇번째, 어느 위치의 프레임으로 이동해서 그려줄 것인지 결정할 x방향 및 y방향의 offset값이 담긴 vec2 데이터 (마찬가지로 오픈프레임웍스에서 쏴줌.)
uniform vec3 translation; // ofApp.cpp 의 update() 함수에서 계산된 charPos 값을 매 프레임마다 전달받아서 pos 값에 더해줌으로써, 오른쪽 화살표 키 입력때마다 캐릭터메쉬를 x축 오른쪽 방향으로 이동시킬거임.

out vec2 fragUV;

void main() {
  gl_Position = vec4(pos + translation, 1.0);

  /*
    아래와 같이,
    기본 버텍스 uv데이터(0 ~ 1 사이의 기본 범위)에
    (0.28, 0.19)에 해당하는 만큼의 값을 곱해줌으로써,
    가로는 0 ~ 0.28, 세로는 0 ~ 0.19 사이의 uv좌표값만 갖도록 uv좌표값 범위를 변환해 줌.

    이렇게 하면, 하나의 프레임 부분만 렌더링할 만큼의 uv좌표값을 얻게 됨.

    여기에, offset 이라는, 가로 몇번째, 세로 몇번째 프레임으로
    이동할 것인지에 대한 정보가 담긴 vec2 데이터를 vec2(0.28, 0.19) 에 곱해줌으로써,

    몇 번째 프레임으로 이동해서 해당 프레임 영역의 uv좌표값들을
    계산해줄 것인지를 정할 수 있음. 
  */
  fragUV = vec2(uv.x, 1.0 - uv.y) * size + (offset * size);
}