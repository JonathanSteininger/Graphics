#include <ctime>
#include <functional>
#include <ratio>
#include <chrono>

class DeltaTime{
    public:
        std::chrono::high_resolution_clock::time_point pastTime;
        float deltaTime = 0.000001f;
        DeltaTime(){
            pastTime = std::chrono::high_resolution_clock::now();
        }
        void Tick(){
            std::chrono::high_resolution_clock::time_point tempTime = pastTime;
            pastTime = std::chrono::high_resolution_clock::now();
            deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(pastTime - tempTime ).count();
            if(deltaTime == 0){
                deltaTime = 0.000001f;
            }
        }
};
