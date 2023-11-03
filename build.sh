gcc main.c -Iexternal/include -o build/main

if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

./build/main