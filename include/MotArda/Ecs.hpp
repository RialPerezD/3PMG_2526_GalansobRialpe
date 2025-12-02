#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <algorithm>
#include <cassert>
#include "MotArda/Components/Transform.hpp"
#include "MotArda/Components/Render.hpp"
#include <MotArda/Components/Movement.hpp>


/**
* @class ECSListBase
* @brief Manages the ECS of the engine.
*/

class ECSListBase {
public:
    /**
    * @brief Destructor
    * @details Default destructor of the ECS class
    */
    virtual ~ECSListBase() = default;
    /**
    * @brief removeEntity
    * @param size_t entity Identifier of the content in the list.
    * @details Removes the content below the specified identifier.
    */
    virtual void removeEntity(size_t entity) = 0;
    /**
    * @brief size
    * @details Returns content's size
    */
    virtual size_t size() const = 0;
};

/**
* @class ECSList
* @brief Class for the ECS's list
*/
template<typename T>
class ECSList : public ECSListBase {
public:
    void removeEntity(size_t entity) override {
        list.erase(std::remove_if(list.begin(), list.end(),
            [entity](auto& p) { return p.first == entity; }),
            list.end());
    }

    size_t size() const override { return list.size(); }

    std::vector<std::pair<size_t, T>> list;
};

/**
* @class ECSManager
* @brief Class that manages the ECS
*/
class ECSManager {
    std::unordered_map<std::size_t, std::unique_ptr<ECSListBase>> component_map_;

public:
    template<typename T> void AddComponentType();
    size_t AddEntity();
    void RemoveEntity(size_t entity);

    template<typename T> std::vector<std::pair<size_t, T>>& GetComponentList();
    template<typename T> T* GetComponent(size_t entity);
    template<typename T> T* AddComponent(size_t entity);
    template<typename T> void RemoveComponent(size_t entity);
    template<typename... Components> std::vector<size_t> GetEntitiesWithComponents();
};


// --- Inline Implementations ---
template<typename T>
void ECSManager::AddComponentType() {
    std::size_t type_id = typeid(T).hash_code();
    if (component_map_.find(type_id) == component_map_.end()) {
        component_map_[type_id] = std::make_unique<ECSList<T>>();
    }
}


inline size_t ECSManager::AddEntity() {
    static size_t next_id = 0;
    return next_id++;
}


inline void ECSManager::RemoveEntity(size_t entity) {
    for (auto& [_, list_ptr] : component_map_) {
        list_ptr->removeEntity(entity);
    }
}


template<typename T>
std::vector<std::pair<size_t, T>>& ECSManager::GetComponentList() {
    std::size_t type_id = typeid(T).hash_code();
    auto it = component_map_.find(type_id);
    assert(it != component_map_.end());
    auto* list = static_cast<ECSList<T>*>(it->second.get());
    return list->list;
}


template<typename T>
T* ECSManager::GetComponent(size_t entity) {
    auto& list = GetComponentList<T>();
    for (auto& [id, comp] : list)
        if (id == entity) return &comp;
    return nullptr;
}


template<typename T>
T* ECSManager::AddComponent(size_t entity) {
    auto& list = GetComponentList<T>();
    list.emplace_back(entity, T());
    return &list.back().second;
}


template<typename T>
void ECSManager::RemoveComponent(size_t entity) {
    auto& list = GetComponentList<T>();
    list.erase(std::remove_if(list.begin(), list.end(),
        [entity](auto& p) { return p.first == entity; }),
        list.end());
}

template<typename ... Components>
inline std::vector<size_t> ECSManager::GetEntitiesWithComponents(){

    auto lists = std::make_tuple(GetComponentList<Components>()...);
    std::vector<size_t> result;
    auto& firstList = std::get<0>(lists);

    for (auto& [entity, _] : firstList) {

        bool hasAll = true;

        (..., (hasAll &= (GetComponent<Components>(entity) != nullptr)));

        if (hasAll)
            result.push_back(entity);
    }

    return result;
}


