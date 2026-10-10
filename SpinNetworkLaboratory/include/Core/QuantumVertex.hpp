#pragma once
#include "Vector2D.hpp"
#include "SpinEdge.hpp"
#include "DataStructures/SinglyLinkedList.hpp"
namespace snl {struct QuantumVertex {std::uint64_t id=0;double mass=1,volume=1;Vec2 position,previousPosition,velocity,acceleration,force;bool pinned=false;SinglyLinkedList<SpinEdge> edges;};}
