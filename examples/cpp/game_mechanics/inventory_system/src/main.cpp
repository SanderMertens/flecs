#include <inventory_system.h>
#include <iostream>

// This example shows one possible way to implement an inventory system using
// ECS relationships.

// Inventory tags/relationships
struct Item { };        // Base item type
struct Container { };   // Container tag
struct Inventory { };   // Inventory tag
struct ContainedBy { }; // ContainedBy relationship

// Item / unit properties
struct Active { };      // Item is active/worn

struct Amount {
    int value;          // Number of items the instance represents
};

struct Health {        
    int value;          // Health of the item
};

struct Attack {
    int value;          // Amount of damage an item deals per use
};

// Items
struct Sword : Item { };
struct Armor : Item { };
struct Coin : Item { };

// Item prefab types
struct WoodenSword { };
struct IronSword { };
struct WoodenArmor { };
struct IronArmor { };

// Find Item kind of entity
flecs::entity item_kind(flecs::entity item) {
    flecs::world world = item.world();
    flecs::entity result;

    item.each([&](flecs::id id) {
        if (id.is_entity()) { 
            // If id is a plain entity (component), check if component inherits
            // from Item
            if (id.entity().has(flecs::IsA, world.id<Item>())) {
                result = id.entity();
            }
        } else if (id.is_pair()) {
            // If item has a base entity, check if the base has an attribute
            // that is an Item.
            if (id.first() == flecs::IsA) {
                flecs::entity base_kind = item_kind(id.second());
                if (base_kind) {
                    result = base_kind;
                }
            }
        }
    });

    return result;
}

// Almost the same as item_kind, but return name of prefab vs item kind. This
// returns a more user-friendly name, like "WoodenSword" vs. just "Sword"
const char* item_name(flecs::entity item) {
    flecs::world world = item.world();
    const char *result = NULL;

    item.each([&](flecs::id id) {
        if (id.is_entity()) { 
            if (id.entity().has(flecs::IsA, world.id<Item>())) {
                result = id.entity().name();
            }
        } else if (id.is_pair()) {
            if (id.first() == flecs::IsA) {
                flecs::entity base_kind = item_kind(id.second());
                if (base_kind) {
                    result = id.second().name();
                }
            }
        }
    });

    return result;
}

// If entity is not a container, get its inventory
flecs::entity get_container(flecs::entity container) {
    if (container.has<Container>()) {
        return container;
    }
    return container.target<Inventory>();
}

// Iterate all items in an inventory
template <typename Func>
void for_each_item(flecs::entity container, const Func& func) {
    container.world().filter_builder()
        .term<ContainedBy>(container)
        .build()
        .each(func);
}

// Find item in inventory of specified kind
flecs::entity find_item_w_kind(
    flecs::entity container, flecs::entity kind, bool active_required = false) 
{
    flecs::entity result;

    container = get_container(container);

    for_each_item(container, [&](flecs::entity item) {
        // Check if we should only return active items. This is useful when
        // searching for an item that needs to be equipped.
        if (active_required) {
            if (!item.has<Active>()) {
                return;
            }
        }

        flecs::entity ik = item_kind(item);
        if (ik == kind) {
            result = item;
        }
    });

    return result;
}

// Transfer item to container
void transfer_item(flecs::entity container, flecs::entity item) {
    const Amount *amt = item.get<Amount>();
    if (amt) {
        // If item has amount we need to check if the container already has an
        // item of this kind, and increase the value.
        auto ecs = container.world();
        auto ik = item_kind(item);
        auto dst_item = find_item_w_kind(container, ik);
        if (dst_item) {
            // If a matching item was found, increase its amount
            Amount *dst_amt = dst_item.get_mut<Amount>();
            dst_amt->value += amt->value;
            item.destruct(); // Remove the src item
            return;
        } else {
            // If no matching item was found, fallthrough which will move the
            // item from the src container to the dst container
        }
    }

    // Move item to target container (replaces previous ContainedBy, if any)
    item.add<ContainedBy>(container);
}

// Move items from one container to another
void transfer_items(flecs::entity dst, flecs::entity src) {
    std::cout << ">> Transfer items from " 
        << src.name() << " to " << dst.name() << "\n\n";

    // Defer, because we're adding/removing components while we're iterating
    dst.world().defer([&] {
        dst = get_container(dst); // Make sure to replace players with container
        src = get_container(src);

        for_each_item(src, [&](flecs::entity item) {
            transfer_item(dst, item);
        });
    });
}

// Attack player
void attack(flecs::entity player, flecs::entity weapon) {
    flecs::world ecs = player.world();

    std::cout << ">> " << player.name() << " is attacked with a " 
        << item_name(weapon) << "!\n";

    const Attack *att = weapon.get<Attack>();
    if (!att) {
        // A weapon without Attack power? Odd.
        std::cout << " - the weapon is a dud\n";
        return;
    }

    int att_value = att->value;

    // Get armor item, if player has equipped any
    flecs::entity armor = find_item_w_kind(player, ecs.entity<Armor>(), true);
    if (armor) {
        Health *armor_health = armor.get_mut<Health>();
        if (!armor_health) {
            // Armor without Defense power? Odd.
            std::cout << " - the " << item_name(armor) << " armor is a dud\n";
        } else {
            std::cout << " - " << player.name() << " defends with " 
                << item_name(armor) << "\n";

            // Subtract attack from armor health. If armor health goes below
            // zero, delete the armor and carry over remaining attack points.
            armor_health->value -= att_value;
            if (armor_health->value <= 0) {
                att_value += armor_health->value;
                armor.destruct();
                std::cout << " - " << item_name(armor) << " is destroyed!\n";
            } else {
                std::cout << " - " << item_name(armor) << " has " 
                    << armor_health->value << " health left after taking "
                    << att_value << " damage\n";
                att_value = 0;
            }
        }
    } else {
        // Brave but stupid
        std::cout << " - " << player.name() << " fights without armor!\n";
    }

    // For each usage of the weapon, subtract one from its health
    Health *weapon_health = weapon.get_mut<Health>();
    if (!--weapon_health->value) {
        std::cout << " - " << item_name(weapon) << " is destroyed!\n";
        weapon.destruct();
    } else {
        std::cout << " - " << item_name(weapon) << " has " 
            << weapon_health->value << " uses left";
    }

    // If armor didn't counter the whole attack, subtract from the player health
    if (att_value) {
        Health *player_health = player.get_mut<Health>();
        if (!(player_health->value -= att_value)) {
            std::cout << " - " << player.name() << " died!\n";
            player.destruct();
        } else {
            std::cout << " - " << player.name() << " has " 
                << player_health->value << " health left after taking " 
                << att_value << " damage\n";
        }
    }

    std::cout << "\n";
}

// Print items in inventory
void print_items(flecs::entity container) {
    std::cout << "-- " << container.name() << "'s inventory:\n";
    
    // In case the player entity was provided, make sure we're working with its
    // inventory entity.
    container = get_container(container);

    int32_t count = 0;
    for_each_item(container, [&](flecs::entity item) {
        // Items with an Amount component fill up a single inventory slot but
        // represent multiple instances, like coins.
        int32_t amount = 1;
        item.get([&](const Amount& amt) {
            amount = amt.value;
        });
        
        std::cout << " - " << amount << " " <<  item_name(item);
        if (amount > 1) std::cout << "s";
        std::cout << " (" << item_kind(item).name() << ")\n";
        
        count ++;
    });

    if (!count) {
        std::cout << " - << empty >>\n";
    }

    std::cout << "\n";
}

int main(int, char *[]) {
    flecs::world ecs;

    // Register ContainedBy relationship
    ecs.component<ContainedBy>()
        .add(flecs::Exclusive); // Item can only be contained by one container

    // Register item kinds
    ecs.component<Sword>().is_a<Item>();
    ecs.component<Armor>().is_a<Item>();
    ecs.component<Coin>().is_a<Item>();

    // Register item prefabs
    ecs.prefab<WoodenSword>().add<Sword>()
        .set<Attack>({ 1 })
        .set_override<Health>({ 5 }); // copy to instance, don't share

    ecs.prefab<IronSword>().add<Sword>()
        .set<Attack>({ 2 })
        .set_override<Health>({ 10 });

    ecs.prefab<WoodenArmor>().add<Armor>()
        .set_override<Health>({ 10 });

    ecs.prefab<IronArmor>().add<Armor>()
        .set_override<Health>({ 20 });

    // Create a loot box with items
    auto loot_box = ecs.entity("Chest").add<Container>().with<ContainedBy>([&]{
        ecs.entity().is_a<IronSword>();
        ecs.entity().is_a<WoodenArmor>();
        ecs.entity().add<Coin>().set<Amount>({ 30 });
    });

    // Create a player entity with an inventory
    auto player = ecs.entity("Player").set<Health>({10}).add<Inventory>(
        ecs.entity().add<Container>().with<ContainedBy>([&]{
            ecs.entity().add<Coin>().set<Amount>({ 20 });
        })
    );

    // Print items in loot box
    print_items(loot_box);

    // Print items in player inventory
    print_items(player);

    // Copy items from loot box to player inventory
    transfer_items(player, loot_box);

    // Print items in player inventory after transfer
    print_items(player);

    // Print items in loot box after transfer
    print_items(loot_box);

    // Find armor entity & equip it
    flecs::entity armor = find_item_w_kind(player, ecs.entity<Armor>());
    if (armor) {
        armor.add<Active>();
    }

    // Create a weapon to attack the player with
    flecs::entity my_sword = ecs.entity()
        .is_a<IronSword>();

    // Attack player
    attack(player, my_sword);

    std::cout << "\n";

    // Output
    // -- Chest's inventory:
    //  - 1 IronSword (Sword)
    //  - 1 WoodenArmor (Armor)
    //  - 30 Coins (Coin)

    // -- Player's inventory:
    //  - 20 Coins (Coin)

    // >> Transfer items from Chest to Player

    // -- Player's inventory:
    //  - 50 Coins (Coin)
    //  - 1 IronSword (Sword)
    //  - 1 WoodenArmor (Armor)

    // -- Chest's inventory:
    //  - << empty >>

    // >> Player is attacked with a IronSword!
    //  - Player defends with WoodenArmor
    //  - WoodenArmor has 8 health left after taking 2 damage
    //  - IronSword has 9 health left
}
