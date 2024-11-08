#include "precomp.h"
#include "Projectile.h"

#include "MySprite.h"
#include "PlayerData.h"
#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "TimeManager.h"
#include "EntityManager.h"
#include "ColliderComponent.h"
#include "Military.h"
#include "TileMap.h"
#include "Quadtree.h"
#include "Explosion.h"
#include "Scene.h"

Projectile::Projectile(Scene& scene, string type) : 
    Entity(scene),
    projData{Projectile::Data::GetData(type)}
{
    AddComponent<TransformComponent>();
    AddComponent<SpriteComponent>("projectiles");
}

Projectile::~Projectile()
{

}

void Projectile::Tick()
{
    float deltaTime = TimeManager::GetDeltaTime();
    float moveForwardBy = deltaTime * vel;

    distanceLeft -= moveForwardBy;

    if (distanceLeft <= 0.0f)
    {
        Hit();
        return;
    }
    TransformComponent* myTransform = GetComponent<TransformComponent>();
    myTransform->MoveForward(moveForwardBy);

    SpriteComponent* mySprite = GetComponent<SpriteComponent>();
    BoundingBox myBox = { myTransform->GetWorldPos(), make_float2(mySprite->GetSize()) };
    Quadtree* quadTree = scene.quadTree.get();

    if (!quadTree->GetMaxBounds().Contains(myBox))
    {
        Hit();
        return;
    }

    float timePassed = TimeManager::GetTotalTimePassed();
    const uint fps = 16;
    const uint numOfFramesPerDir = 4;
    uint animationOffset = static_cast<uint>(fmodf(timePassed, static_cast<float>(numOfFramesPerDir) / static_cast<float>(fps)) * fps);

    const uint directions = 8;
    uint rotationOffset = SpriteComponent::GetRotationOffset(myTransform->GetRotation(), directions);

    uint frame = static_cast<uint>(rotationOffset * numOfFramesPerDir + animationOffset);

    mySprite->GetSprite()->SetFrame(uint2{ frame, projData->spriteRow });


    // Check for collisions
    vector<ColliderComponent*> collisions = quadTree->Query(myBox, layer);

    for (ColliderComponent* collider : collisions)
    {
        Military* colliderOwner = dynamic_cast<Military*>(collider->GetOwner());

        if (colliderOwner == nullptr 
            || colliderOwner->GetArmyId() == firedBy)
        {
            continue;
        }

        // Pixel collision check here
        SpriteComponent* otherSprite = colliderOwner->GetComponent<SpriteComponent>();

        if (otherSprite == nullptr 
            || !mySprite->IsColliding(otherSprite))
        {
            continue;
        }

        // Should deal damage instead
        colliderOwner->TakeDamage(projData->damage, this);
        Hit();
        return;
    }

    
}


void Projectile::Hit()
{
    EntityManager* entityManager = scene.entityManager.get();
    float2 centre = GetComponent<TransformComponent>()->GetWorldPos() + make_float2(GetComponent<SpriteComponent>()->GetSize()) / 2.0f;

    if (projData->explosionType != "none")
    {
        entityManager->AddEntity<Explosion>(projData->explosionType, centre);
    }

    entityManager->RemoveEntity(GetId());
}

void Projectile::Fire(float2 position, float rotation, ArmyId a_firedBy, CollisionLayer a_layer, float startingVel, float turretRange)
{
    TransformComponent* transform = GetComponent<TransformComponent>();
    SpriteComponent* sprite = GetComponent<SpriteComponent>();
    uint2 centreOffset = sprite->GetSize() / 2u;
   
    transform->SetWorldPos(position - make_float2(centreOffset));
    transform->SetRotation(rotation);

    origin = position;
    vel = startingVel;
    firedBy = a_firedBy;
    layer = a_layer;
    distanceLeft = turretRange;
}

Projectile::Data::Data(const string& type)
{
    PlayerData playerData("entity.projectile." + type);

    damage = stof(playerData.GetVariableValue("damage"));
    explosionType = playerData.GetVariableValue("explosiontype");
    spriteRow = static_cast<uint>(stoi(playerData.GetVariableValue("spriterow")));
}
