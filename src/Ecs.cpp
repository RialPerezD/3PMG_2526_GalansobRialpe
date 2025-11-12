#include "Motarda/Ecs.hpp"
#include <typeindex>
#include <memory>
#include <vector>
#include <cassert>


template<typename T>
void ECSManager::AddComponentType() {
    std::size_t type_id = typeid(T).hash_code();
    if (component_map_.find(type_id) == component_map_.end()) {
        component_map_[type_id] = std::make_unique<ECSList<T>>();
    }
}


unsigned long ECSManager::AddEntity() {
    static unsigned long next_id = 0;
    return next_id++;
}


void ECSManager::RemoveEntity(unsigned long entity) {
    for (auto& [_, list_ptr] : component_map_) {}
}


template<typename T>
std::vector<T>& ECSManager::GetComponentList(){
    std::size_t type_id = typeid(T).hash_code();
    auto it = component_map_.find(type_id);
    assert(it != component_map_.end());
    auto* list = static_cast<ECSList<T>*>(it->second.get());
    return list->list;
}

template<typename T>
T* ECSManager::GetComponent(unsigned long entity) {
    auto& list = GetComponentList<T>();
    for (auto& [id, comp] : list) {
        if (id == entity) return &comp;
    }
    return nullptr;
}


template<typename T>
T* ECSManager::AddComponent(unsigned long entity) {
    auto& list = GetComponentList<T>();
    list.emplace_back(entity, T());
    return &list.back().second;
}


template<typename T>
void ECSManager::RemoveComponent(unsigned long entity) {
    auto& list = GetComponentList<T>();
    list.erase(std::remove_if(list.begin(), list.end(),
        [entity](auto& p) { return p.first == entity; }),
        list.end());
}


