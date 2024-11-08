#pragma once
#include "OneOff.h"
#include "DataSuper.h"

class Explosion :
    public OneOff
{
public:
    Explosion(Scene& scene, const string& type, float2 atPosition);
    ~Explosion();

    void Tick() override;

    class Data : 
        public DataSuper<Data>
    {
    private:
        friend DataSuper;
        Data(const string& type);

    public:
        float lifeSpan{};

        string spriteType{};
    };

private:
    Data* explosionData{};
};

