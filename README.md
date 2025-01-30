# Tiny Renderer

C++로 구현한 소프트웨어 렌더러입니다.

## 결과물
<img width="885" alt="Screenshot 2025-01-30 at 10 37 06 PM" src="https://github.com/user-attachments/assets/ffe859ba-e6b8-4cbf-aa60-07d8a9bbd906" />


## 참고 자료

[이득우 - 수식없이 게임 수학 이해하기](https://www.youtube.com/watch?v=nY6cZOY3VOs)

## 기술 스택

- C++17
- CMake

## 프로젝트 구조

.
├── CMakeLists.txt      # CMake 설정 파일
├── main.cpp            # 메인 소스 코드
├── tgaimage.cpp        # TGA 이미지 처리 관련 코드
├── output.tga          # Diablo3 렌더링 결과
├── african_head_output.tga  # 아프리카 헤드 렌더링 결과
├── skull_output.tga    # 해골 모델 렌더링 결과
└── obj/               # 3D 모델 파일들
    ├── diablo3_pose/  # Diablo3 캐릭터 모델
    ├── Skull/         # 해골 모델
    ├── african_head/  # 아프리카 헤드 모델
    └── boggie/        # Boggie 캐릭터 모델

## 빌드 방법

1. CMake 설치
2. 빌드 디렉토리 생성:
