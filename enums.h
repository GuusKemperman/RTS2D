#pragma once

enum class ArmyId : short int { null, army1, army2, numberOfArmies, player = army1, enemy = army2 };
enum class ComponentType : short int { collider, selecting, sprite, transform, turret, numberOfTypes };
enum class UIElementType : short int { null, sprite, button, text, minimap };

using CollisionLayer = uchar;
constexpr CollisionLayer allLayersMask = 0xff;

using EntityId = size_t;