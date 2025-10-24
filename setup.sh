#!/bin/bash

# ==========================================================
GIT_REPO_URL="https://github.com/mit-pdos/xv6-public" 
CLONE_DIR_NAME="xv6-public" 
DATA_DIR_NAME="data"
# ==========================================================

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
DATA_DIR="$SCRIPT_DIR/$DATA_DIR_NAME"
TARGET_DIR="$SCRIPT_DIR/$CLONE_DIR_NAME"

# data 디렉토리의 존재 여부 확인
if [ ! -d "$DATA_DIR" ]; then
    echo "오류: 사전에 준비된 파일 디렉토리 ($DATA_DIR)가 현재 위치에 없습니다." >&2
    exit 1
fi

# ==========================================================
# 1. 인자 유무에 따른 주 로직 분기
# ==========================================================

# 인자가 없는 경우 (매개변수 개수가 0개) - Git Clone/Reset 및 Makefile 덮어쓰기 수행
if [ $# -eq 0 ]; then
    # 1-1. 대상 디렉토리 존재 여부 체크 및 Git 초기화/클론
    if [ -d "$TARGET_DIR" ]; then
        echo "대상 디렉토리 ($TARGET_DIR)가 이미 존재합니다. Git 초기화 명령을 수행합니다."
        
        # Git 초기 상태로 되돌리기 (모든 로컬 변경사항 및 untracked 파일 제거)
        if cd "$TARGET_DIR"; then
            echo "-> Git 레포지토리의 모든 로컬 변경사항을 강제로 제거합니다."
            git reset --hard
            echo "-> 추적되지 않은 파일 및 디렉토리들을 제거합니다."
            git clean -fdx
            
            cd ..
            echo "-> Git 레포지토리를 깨끗한 초기로 되돌렸습니다."
        else
            echo "오류: $TARGET_DIR 디렉토리로 이동할 수 없습니다. Git 초기화 실패." >&2
            exit 1
        fi
    else
        echo "-> Git Repository ($GIT_REPO_URL) 클론 시작..."
        git clone $GIT_REPO_URL
        
        if [ $? -ne 0 ]; then
            echo "오류: Git Clone 실패" >&2
            exit 1
        fi
        echo "-> Git Clone 성공."
    fi

    # 1-2. Makefile 덮어쓰기
    if [ ! -d "$TARGET_DIR" ]; then
        echo "오류: 대상 디렉토리 ($TARGET_DIR)를 찾을 수 없습니다. (클론 실패 또는 이름 오류)" >&2
        exit 1
    fi
    
    echo "-> $DATA_DIR/Makefile을 $TARGET_DIR에 덮어씌웁니다."
    cp -f "$DATA_DIR/Makefile" "$TARGET_DIR/"
    
    if [ $? -eq 0 ]; then
        echo "$DATA_DIR/Makefile 덮어쓰기 완료."
        echo ""
        echo "=========================================================="
        echo "원하는 인자 번호(1, 2, 3, 5)와 함께 스크립트를 실행하십시오."
        echo ""
        echo "  $0 [번호]"
        echo "  예시: $0 3"
        echo "=========================================================="
    else
        echo "오류: $DATA_DIR/Makefile 덮어쓰기 실패." >&2
        exit 1
    fi


# 인자가 있는 경우 (1, 2, 3, 5 중 하나) - Git Clone 건너뛰고 해당 디렉토리 파일만 덮어쓰기
elif [ $# -eq 1 ]; then
    PARAM=$1
    if [[ "$PARAM" =~ ^[1235]$ ]]; then
        if [ ! -d "$TARGET_DIR" ]; then
            echo "오류: 대상 디렉토리 ($TARGET_DIR)가 존재하지 않습니다. 먼저 인자 없이 스크립트 ($0)를 실행하여 Git Clone을 수행해야 합니다." >&2
            exit 1
        fi

        # Git 초기 상태로 되돌리기 (모든 로컬 변경사항 및 untracked 파일 제거)
        if cd "$TARGET_DIR"; then
            echo "-> Git 레포지토리의 모든 로컬 변경사항을 강제로 제거합니다."
            git reset --hard
            echo "-> 추적되지 않은 파일 및 디렉토리들을 제거합니다."
            git clean -fdx
            
            cd ..
            echo "-> Git 레포지토리를 깨끗한 초기로 되돌렸습니다."
        else
            echo "오류: $TARGET_DIR 디렉토리로 이동할 수 없습니다. Git 초기화 실패." >&2
            exit 1
        fi

        SOURCE_SUBDIR="$DATA_DIR/$PARAM"
        
        if [ ! -d "$SOURCE_SUBDIR" ]; then
            echo "오류: $SOURCE_SUBDIR 디렉토리가 존재하지 않습니다." >&2
            exit 1
        fi
        
        echo "-> $SOURCE_SUBDIR 디렉토리 내부의 파일을 $TARGET_DIR에 복사합니다. (복사 목록 출력)"

        # rsync를 사용하여 복사되는 파일 목록을 자세히 출력
        rsync -av --progress "$SOURCE_SUBDIR/" "$TARGET_DIR"

        if [ $? -eq 0 ]; then
            echo "$SOURCE_SUBDIR 내용 덮어쓰기 완료."
        else
            echo "오류: $SOURCE_SUBDIR 내용 덮어쓰기 실패." >&2
            exit 1
        fi

    else
        echo "오류: 허용되지 않은 인자입니다. 인자는 1, 2, 3, 5 중 하나여야 합니다." >&2
        exit 1
    fi

# 인자가 1개 초과인 경우
else
    echo "오류: 인자는 없거나 1, 2, 3, 5 중 하나만 지정해야 합니다." >&2
    exit 1
fi

echo "--- 프로그램 실행 완료. ---"
