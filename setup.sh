#!/bin/bash

# ==========================================================
# 1. 전역 변수 설정
# ==========================================================
GIT_REPO_URL="https://github.com/mit-pdos/xv6-public"
CLONE_DIR_NAME="xv6-public"
DATA_DIR_NAME="data"

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
DATA_DIR="$SCRIPT_DIR/$DATA_DIR_NAME"
TARGET_DIR="$SCRIPT_DIR/$CLONE_DIR_NAME"

# ==========================================================
# 2. 함수 정의
# ==========================================================

# Git 레포지토리를 초기 상태로 되돌리는 함수
cleanup_git_repo() {
    if [ ! -d "$TARGET_DIR" ]; then
        echo "오류: 대상 디렉토리 ($TARGET_DIR)가 존재하지 않습니다." >&2
        return 1
    fi

    if cd "$TARGET_DIR"; then
        echo "-> Git 레포지토리의 모든 로컬 변경사항을 강제로 제거합니다."
        git reset --hard
        echo "-> 추적되지 않은 파일 및 디렉토리들을 제거합니다."
        git clean -fdx
        
        cd ..
        echo "-> Git 레포지토리를 깨끗한 초기로 되돌렸습니다."
        return 0
    else
        echo "오류: $TARGET_DIR 디렉토리로 이동할 수 없습니다. Git 초기화 실패." >&2
        return 1
    fi
}

# Makefile을 복사하는 함수
copy_makefile() {
    echo "-> $DATA_DIR/Makefile을 $TARGET_DIR에 덮어씌웁니다."
    cp -f "$DATA_DIR/Makefile" "$TARGET_DIR/"
    
    if [ $? -eq 0 ]; then
        echo "$DATA_DIR/Makefile 덮어쓰기 완료."
        return 0
    else
        echo "오류: $DATA_DIR/Makefile 덮어쓰기 실패." >&2
        return 1
    fi
}

# 특정 서브 디렉토리의 파일을 복사하는 함수
copy_files() {
    local SOURCE_SUBDIR="$DATA_DIR/$1"
    
    if [ ! -d "$SOURCE_SUBDIR" ]; then
        echo "오류: $SOURCE_SUBDIR 디렉토리가 존재하지 않습니다." >&2
        return 1
    fi
    
    echo "-> $SOURCE_SUBDIR 디렉토리 내부의 파일을 $TARGET_DIR에 복사합니다. (복사 목록 출력)"

    # rsync를 사용하여 복사되는 파일 목록을 자세히 출력
    rsync -av --progress "$SOURCE_SUBDIR/" "$TARGET_DIR"

    if [ $? -eq 0 ]; then
        echo "$SOURCE_SUBDIR 내용 덮어쓰기 완료."
        return 0
    else
        echo "오류: $SOURCE_SUBDIR 내용 덮어쓰기 실패." >&2
        return 1
    fi
}

# ==========================================================
# 3. 사전 검사 (데이터 디렉토리)
# ==========================================================
if [ ! -d "$DATA_DIR" ]; then
    echo "오류: 사전에 준비된 파일 디렉토리 ($DATA_DIR)가 현재 위치에 없습니다." >&2
    exit 1
fi

# ==========================================================
# 4. 주 로직 분기 (인자 개수에 따라)
# ==========================================================

case $# in

    0) # 인자가 없는 경우 (Git Clone 및 Makefile 덮어쓰기)
        echo "--- 인자 0개: Git 초기 설정 모드 ---"
        if [ -d "$TARGET_DIR" ]; then
            echo "대상 디렉토리 ($TARGET_DIR)가 이미 존재합니다. Git 초기화 명령을 수행합니다."
            if ! cleanup_git_repo; then exit 1; fi
        else
            echo "-> Git Repository ($GIT_REPO_URL) 클론 시작..."
            git clone $GIT_REPO_URL
            
            if [ $? -ne 0 ]; then
                echo "오류: Git Clone 실패" >&2
                exit 1
            fi
            echo "-> Git Clone 성공."
        fi

        if ! copy_makefile; then exit 1; fi
        
        echo ""
        echo "=========================================================="
        echo "원하는 인자 번호(1, 2, 3, 5)와 함께 스크립트를 실행하십시오."
        echo "  $0 [번호] [옵션이 있다면 옵션 입력]"
        echo "=========================================================="
        ;;

    1) # 인자가 1개인 경우 (Git 초기화 후 파일 덮어쓰기)
        PARAM=$1
        echo "--- 인자 1개: $PARAM 프로젝트 모드 ---"

        if [[ "$PARAM" =~ ^[1235]$ ]]; then
            # 대상 디렉토리 존재 확인 및 Git 초기화
            if ! cleanup_git_repo; then 
                echo "오류: 대상 디렉토리가 없거나 Git 초기화 실패. 먼저 인자 없이 스크립트를 실행해야 합니다." >&2
                exit 1
            fi
            
            # 파일 복사
            if ! copy_files "$PARAM"; then exit 1; fi
        else
            echo "오류: 허용되지 않은 인자입니다. 인자는 1, 2, 3, 5 중 하나여야 합니다." >&2
            exit 1
        fi
        ;;

    2) # 인자가 2개인 경우 (특별 처리: '3 n' 확인)
        echo "--- 인자 2개: 특별 처리 모드 ---"
        
        if [ "$1" = "3" ] && [ "$2" = "n" ]; then
            
            # 대상 디렉토리 존재 확인 및 Git 초기화
            if ! cleanup_git_repo; then 
                echo "오류: 대상 디렉토리가 없거나 Git 초기화 실패. 먼저 인자 없이 스크립트를 실행해야 합니다." >&2
                exit 1
            fi

            # 특별 경로 '3_normal' 파일 복사
            if ! copy_files "3_normal"; then exit 1; fi
            echo "$SOURCE_SUBDIR 내용 덮어쓰기 완료. (project 3의 일반 스케줄러 버전)"

        else
            echo "오류: 허용되지 않는 인자 조합입니다. '3 n' 조합만 허용됩니다." >&2
            exit 1
        fi
        ;;

    *) # 그 외의 모든 인자 개수 (오류 처리)
        echo "오류: 지원하지 않는 인자 개수입니다. 인자는 없거나, 1개 또는 '3 n' 조합의 2개만 허용됩니다." >&2
        echo "사용법: $0 <인자> 또는 $0 3 n"
        exit 1
        ;;
esac

echo "--- 프로그램 실행 완료. ---"