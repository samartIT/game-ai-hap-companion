#!/usr/bin/env bash
# คอมไพล์และรันตัวอย่างทั้งหมดของบทที่ 1 ด้วย g++ (Linux/macOS/WSL/MinGW)
# ใช้: bash build_all.sh
set -e
cd "$(dirname "$0")"
mkdir -p build
for f in 01_hello 02_syntax 03_pointers 04_struct 05_class 06_inheritance 07_polymorphism; do
  echo "=== $f ==="
  g++ -std=c++17 -Wall -Wextra -o "build/$f" "$f.cpp"
  "./build/$f"
  echo
done
echo "=== 08_header_source ==="
g++ -std=c++17 -Wall -Wextra -o build/08_header_source \
    08_header_source/main.cpp 08_header_source/GameCharacter.cpp
./build/08_header_source
