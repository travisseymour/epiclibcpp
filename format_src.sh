# find src -name "*.cpp" -o -name "*.h" | xargs clang-format -i
find src -name "*.cpp" -o -name "*.h" | xargs -I {} sh -c 'echo "Formatting: {}"; clang-format -i {}'
