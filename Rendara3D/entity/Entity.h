#pragma once
#include <iostream>
#include <vector>

#include "../Model.h"
#include "../Games/Transform.h"

namespace slotmap
{
	typedef long long object_id;


	class HasID
	{
	public:
		object_id id;

	};

	template <class HasID>
	class SlotMap
	{
		const size_t chunk_size = 256;
		std::vector<HasID*> object_table;
		std::vector<int> free_list;

	public:
		object_id create_object() {
			if (free_list.empty()) {
				HasID* chunk = new HasID[chunk_size];
				for (int i = chunk_size - 1; i >= 0; --i) {
					chunk[i].id = object_table.size() * chunk_size + i;
					free_list.push_back(object_table.size() * chunk_size + i);
				}
				object_table.push_back(chunk);
			}

			int free = free_list.back();
			free_list.pop_back();
			return object_table[free / chunk_size][free % chunk_size].id;
		}

		HasID* get_object(object_id id) {
			HasID* obj = object_table[(id & 0xFFFFFFFF) / chunk_size] + ((id & 0xFFFFFFFF) % chunk_size);
			return obj->id != id ? nullptr : obj;
		}

		void destroy_object(object_id id) {
			HasID* obj = get_object(id);
			obj->id = (obj->id & 0xFFFFFFFF) | (((obj->id >> 32) + 1) << 32);
			free_list.push_back(id & 0xFFFFFFFF);
		}


	};

}





class Entity : public slotmap::HasID
{
public:
	Entity() : has_transform(false), has_model(false) {}

	void setTransform(const Transform& part)
	{
		transform = part;
		has_transform = true;
	}

	void setModel(Model* rend)
	{
		model = rend;
		has_model = true;
	}

	Transform* getTransform()
	{
		return &transform;
	}

	Model* getModel()
	{
		return model;
	}

	bool hasTransform() const
	{
		return has_transform;
	}
	bool hasModel() const
	{
		return has_model;
	}


	void unsetModel()
	{
		has_model = false;
	}

	void unsetTransform()
	{
		has_transform = false;
	}


private:
	bool has_transform;
	bool has_model;

	Transform   transform;
	Model* model;

};



class MovementSystem
{
public:
	void update(Entity& e)
	{
		if (e.hasTransform())
		{
			Transform* p = e.getTransform();
			p->position += 0.1f;
		}
	}

};




