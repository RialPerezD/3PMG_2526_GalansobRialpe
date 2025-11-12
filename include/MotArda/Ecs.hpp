#include <iostream>
#include <string>
#include <unordered_map>

#include <MotArda/Components/Transform.hpp>
#include <MotArda/Components/Render.hpp>

class ECSListBase {
public:
    virtual void grow() = 0;
    virtual size_t size() = 0;
};


template<typename T>
class ECSList : public ECSListBase {
public:
    virtual void grow() { list.emplace_back(); }
    virtual size_t size() { return list.size(); }

    std::vector<std::pair<size_t, T>> list;
};


class ECSManager {
    std::unordered_map<std::size_t, std::unique_ptr<ECSListBase>> component_map_;

public:
    template<typename T> void AddComponentType();

    unsigned long AddEntity();
    void RemoveEntity(unsigned long);

    template<typename T> std::vector<T>& GetComponentList();

    template<typename T> T* GetComponent(unsigned long entity);
    template<typename T> T* AddComponent(unsigned long entity);
    template<typename T> void RemoveComponent(unsigned long entity);


};

std::unordered_map<std::size_t, std::unique_ptr<ECSListBase>> component_map_;