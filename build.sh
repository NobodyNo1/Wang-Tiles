gcc main.c -o main

if [ $? -ne 0 ]; then
    echo "Build failed"
    exit 1
fi

./main