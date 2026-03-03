#pragma once
#include "boost/dynamic_bitset_fwd.hpp"
#include "types.h"
#include <boost/dynamic_bitset.hpp>
#include <cstddef>
#include <expected>
#include <stack>
#include <unordered_map>
#ifndef ENTITY_H
#define ENTITY_H

#ifndef INITIAL_ENTITIES
#define INITIAL_ENTITIES 1024
#endif

/**
 * Class that specializes in creating, destroying, tracking, and tracking components tied to an entity
 */
class EntityManager {
public:
    EntityManager();

    ~EntityManager();

    /**
     * Creates an entity, and starts tracking it
     * @return The new entity_id
     */
    entity_id create_entity();

    /**
     * Destroys an entity and stops tracking it
     * @param e_id Entity id to destroy
     * @return An error if the entity was not being tracked (aka alive)
     */
    std::expected<void, EcsError> destroy_entity(entity_id e_id);

    /**
     * Adds the component to the signature of an entity
     * @param e_id Entity that received the component
     * @param c_id Component id added to the entity
     * @return An error if the entity was not being tracked
     */
    std::expected<void, EcsError> component_added(entity_id e_id,
                                                  component_id c_id);

    /**
      * Removes the component from the signature of an entity
     * @param e_id Entity that lost the component
     * @param c_id Component id removed from the entity
     * @return An error if the entity was not being tracked
     */
    std::expected<void, EcsError> component_removed(entity_id e_id,
                                                    component_id c_id);

    /**
     * Retrieves the component signature tied to an entity
     * @param e_id Entity to retrieve the bitset from
     * @return The signature representing the components an entity has, an error if the entity is not being tracked
     */
    std::expected<boost::dynamic_bitset<>, EcsError> entity_bitset(entity_id e_id);

    /**
     *
     * @param e_id Entity to query
     * @param c_id Component id to query
     * @return true if the entity has the component, an error if the entity is not being tracked
     */
    std::expected<bool, EcsError> has_component(entity_id e_id,
                                                component_id c_id);

private:
    /**
     * Expands the amount of entities that can be held
     * @param start index to start
     * @param finish index to finish
     */
    void expand_entities(int start, int finish);

    std::stack<entity_id> available_entities;

    //Tracks which entities are alive
    boost::dynamic_bitset<> alive_entities;

    //Maps an entity's id to its signature
    std::unordered_map<entity_id, boost::dynamic_bitset<> > entity_bitsets;

    size_t num_alive_entities;
};

#endif
