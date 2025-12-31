# Bonato3D, a basic model loader
This is my low-level exploration into 3D graphics.
When you launch main, you should see suzanne the monkey (albeit rotated).

If you check out the old code folder, you will see various past pieces of code where I used python and pygame or c++ and opengl to render things in. After a while I eventually settled on doing things per-pixel and using glDrawPixels().

Note: the compile command used was ```g++ main.cpp -lopengl32 -lglfw3 -lassimp -std=c++23 -O3 -o main```

