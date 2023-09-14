#include <vector>
class Vector2{
    public:
        float X,Y;
        Vector2(float X = 0, float Y = 0){
            this->X = X;
            this->Y = Y;
        }
        Vector2 operator+(Vector2 obj){
            Vector2 tmp;
            tmp.X = X + obj.X;
            tmp.Y = Y + obj.Y;
            return tmp;
        }
        
        Vector2 operator-(Vector2 obj){
            Vector2 tmp;
            tmp.X = X - obj.X;
            tmp.Y = Y - obj.Y;
            return tmp;
        }
        
        Vector2 operator*(Vector2 obj){
            Vector2 tmp;
            tmp.X = X * obj.X;
            tmp.Y = Y * obj.Y;
            return tmp;
        }
        Vector2 operator/(Vector2 obj){
            Vector2 tmp;
            tmp.X = X / obj.X;
            tmp.Y = Y / obj.Y;
            return tmp;
        }
        Vector2 operator*(int num){
            Vector2 tmp;
            tmp.X = X * num;
            tmp.Y = Y * num;
            return tmp;
        }
};
class GameBlock{
    private:
        unsigned int indecies[6] = {
            0,1,3,
            2,1,3
        };
    public:
        float Scale = 1;
        Vector2 position;
        Vector2 size;
        static const int VERTEX_COUNT = 8;
        static const int INDECIE_COUNT = 6;
        GameBlock(float x, float y, float width, float height){
            position = Vector2(x,y);
            size = Vector2(width, height);
        }
        GameBlock(Vector2 Position, Vector2 Size){
            position = Position;
            size = Size;
        }
        
        float Left(){ return position.X - size.X / 2;}
        float Right(){ return position.X + size.X / 2;}
        float Top(){ return position.Y - size.Y / 2;}
        float Bottom(){ return position.Y + size.Y / 2;}

        void Indecies(unsigned int offset, unsigned int * outArray){
            for(int i = 0; i < INDECIE_COUNT; i++){
                outArray[i] = offset + indecies[i];
            }
        }
        

        void Vertecies(float *outArray){
            outArray[0] = Left();
            outArray[1] = Top();
            outArray[2] = Right();
            outArray[3] = Top();
            outArray[4] = Right();
            outArray[5] = Bottom();
            outArray[6] = Left();
            outArray[7] = Bottom();
        }

};
class BufferMaintainer{

    public:
        std::vector<GameBlock*> blocks;
        unsigned int amount = 0;
        unsigned int vertexBlockCount;
        unsigned int indeciesBlockCount;
        unsigned int indeciesOffset;

        BufferMaintainer(unsigned int vertexBlockCount, unsigned int indeciesBlockCount){
            this->vertexBlockCount = vertexBlockCount;
            this->indeciesBlockCount = indeciesBlockCount;
            this->indeciesOffset = vertexBlockCount/2;
        }
        void addPointer(GameBlock* gameBlockPointer){
            amount++;
            blocks.push_back(gameBlockPointer);
        }
        unsigned int GetVertexBufferByteSize(){
            return GetVertexCount() * sizeof(float);
        }
        unsigned int GetVertexCount(){
            if(amount == 0) return 0;
            return amount * vertexBlockCount;
        }
        unsigned int GetIndeciesByteSize(){
            return GetIndeciesCount() * sizeof(unsigned int);
        }
        unsigned int GetIndeciesCount(){
            if(amount == 0) return 0;
            return amount * indeciesBlockCount;
        }
        void GetVerteciesBuffer(float* BufferOut) {
            for(int i = 0; i < amount; i++){
                blocks[i]->Vertecies(&BufferOut[i * vertexBlockCount]);
            }
        }
        void GetIndeciesBuffer(unsigned int* BufferOut) {
            for(int i = 0; i < amount; i++){
                blocks[i]->Indecies(i*indeciesOffset, &BufferOut[i*indeciesBlockCount]);
            }
        }
};
enum class Direction{
    UP = 1, NONE = 0, DOWN = -1
};
class paddle{
    public: 
        GameBlock transform;
        float ySpeed;
        paddle(float xpos, float ypos, float width, float height, float ySpeed) : transform(xpos, ypos, width, height) {
            this->ySpeed = ySpeed;
        }
        void movePaddle(Direction direction, float deltaTime){
            transform.position.Y += ySpeed * (int)direction * deltaTime;
            if(transform.Top() < -1) {
                transform.position.Y = -1 + transform.size.Y / 2;
            }
            if(transform.Bottom() > 1) {
                transform.position.Y = 1 - transform.size.Y / 2;
            }
        }
        bool checkCollision(GameBlock *otherBlock){
            bool collisionHorizontal = otherBlock->Left() <= transform.Right() && otherBlock->Right() >= transform.Left();
            bool collisionVertical =  otherBlock->Top() <= transform.Bottom() && otherBlock->Bottom() >= transform.Top();
            return collisionHorizontal && collisionVertical;
        }
};

class ball{
    private:
        Vector2 startSpeed;
        float maxSpeed = 1;
    public:
        GameBlock transform;
        Vector2 speed;
        ball(float xpos, float ypos, float size, float xspeed, float yspeed) : transform(xpos, ypos, size, size) {
            speed.Y = yspeed;
            speed.X = xspeed;
            this->startSpeed.Y = yspeed;
            this->startSpeed.X = xspeed;
        }
        void move(float deltaTime, paddle *paddleLeft, paddle *paddleRight){
            if(transform.position.X > 1 || transform.position.X < -1){
                reset();
            }
            if(checkCollision(&paddleLeft->transform) && speed.X > 0){
                speed.X *= speed.X > maxSpeed ? -1 : -1.3;
            }
            if(checkCollision(&paddleRight->transform) && speed.X < 0){
                speed.X *= speed.X < -maxSpeed ? -1 : -1.3;
            }
            if(transform.position.Y > 1 && speed.Y > 0){
                speed.Y *= -1;
            }
            if(transform.position.Y < -1 && speed.Y < 0){
                speed.Y *= -1;
            }
            transform.position.X += speed.X * deltaTime;
            transform.position.Y += speed.Y * deltaTime;
        }
        void reset(){
            transform.position.X = 0; 
            transform.position.Y = 0; 
            speed.Y = startSpeed.Y;
            speed.X = startSpeed.X;
        }
        bool checkCollision(GameBlock *otherBlock){
            bool collisionHorizontal = otherBlock->Left() <= transform.Right() && otherBlock->Right() >= transform.Left();
            bool collisionVertical =  otherBlock->Top() <= transform.Bottom() && otherBlock->Bottom() >= transform.Top();
            return collisionHorizontal && collisionVertical;
        }
};
struct playerKeys{
    public:
    int upKey;
    int downKey;
    bool upKeyPressed = false;
    bool downKeyPressed = false;

    playerKeys(int keyCodeUp, int KeyCodeDown){
        upKey = keyCodeUp;
        downKey = KeyCodeDown;
    }
};
