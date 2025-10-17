#ifndef ACTIONSET_H
#define ACTIONSET_H

#include <memory>
#include <vector>
#include <unordered_map>
#include "action.h"

class ActionSet {
public:
    /**
     * @brief Construct an empty ActionSet.
     */
    ActionSet();

    /**
     * @brief Find a node by ID (mutable).
     * @return Pointer to node or nullptr if not found.
     */
    Action*       find(int id);

    /**
     * @brief Find a node by ID (const).
     * @return Pointer to node or nullptr if not found.
     */
    const Action* find(int id) const;

    /**
     * @brief Add a node and connect it after the given parents.
     *
     * Behavior:
     *   - Takes ownership of @p node.
     *   - Inserts into `nodes_` and updates `id2idx_`.
     *   - For each parent in @p parentIds, appends this node's id to
     *     parent.runAfterMe (if not already present).
     *
     * @param node       Newly created Action (unique_ptr).
     * @param parentIds  IDs of nodes that should precede this node.
     * @return The ID of the inserted node (== node->id).
     */
    int  add(std::unique_ptr<Action> node, const std::vector<int>& parentIds);

    /**
     * @brief Replace an existing node (by ID) and update parent links.
     *
     * Behavior:
     *   - If @p id exists, the old node is replaced by @p replacement
     *     (ownership transferred).
     *   - Rebuilds parent links according to @p newParentIds (removes old
     *     parent→child edges to this id, adds the new ones).
     *
     * @param id             Target node ID to replace.
     * @param replacement    New Action with the SAME id field (or will be set).
     * @param newParentIds   New set of parent IDs.
     * @return true if successful, false if id not found.
     */
    bool edit(int id, std::unique_ptr<Action> replacement, const std::vector<int>& newParentIds);

    /**
     * @brief Remove a node by ID. Optionally relink its children to Start.
     *
     * Behavior:
     *   - Erases node from `nodes_` and `id2idx_`.
     *   - Removes all edges from its parents to this node.
     *   - Removes all edges from this node to its children.
     *   - If @p relinkToStartIfOrphan is true, children that become orphaned
     *     will be linked from a StartAction (id=0), if present.
     *
     * @param id                       Node to remove.
     * @param relinkToStartIfOrphan    If true, ensure children remain reachable.
     * @return true if removed, false if id not found.
     */
    bool remove(int id, bool relinkToStartIfOrphan = true);

    /**
     * @brief Create a parent → child edge.
     *
     * Adds `childId` to parent.runAfterMe if not already present.
     * Returns false if either node is missing.
     */
    bool link(int parentId, int childId);

    /**
     * @brief Remove a parent → child edge.
     *
     * Erases `childId` from parent.runAfterMe if present.
     * Returns false if either node is missing.
     */
    bool unlink(int parentId, int childId);

    /**
     * @brief Get all parents of a node (IDs).
     *
     * Scans the graph and returns any node whose `runAfterMe` contains @p id.
     * Complexity: O(n).
     */
    std::vector<int> parentsOf(int id) const;

    /**
     * @brief Get all children of a node (IDs).
     *
     * Returns a copy of `runAfterMe` for node @p id.
     * If id is not found, returns empty vector.
     */
    std::vector<int> childrenOf(int id) const;

    /**
     * @brief Access all nodes for iteration/serialization.
     */
    const std::vector<std::unique_ptr<Action>>& nodes() const { return nodes_; }

    /**
     * @brief Largest ID currently present (useful for UI ID allocation).
     */
    int lastId() const { return lastId_; }

private:
    /* Storage of owned nodes; index positions are transient and can change
       after removals. Use `id2idx_` for lookups. */
    std::vector<std::unique_ptr<Action>> nodes_;

    /* ID → index mapping into `nodes_`. */
    std::unordered_map<int, size_t> id2idx_;

    /* Tracks the maximum assigned ID to keep UI generation monotonic. */
    int lastId_{0};

    /**
     * @brief Rebuild `id2idx_` from `nodes_`.
     *
     * Call after operations that may reorder `nodes_` (e.g., erase/compact).
     */
    void indexRebuild_();

    /**
     * @brief Utility: check if vector contains an id.
     */
    static bool containsId_(const std::vector<int>& v, int id);

    /**
     * @brief Utility: erase an id from vector if present.
     */
    static void eraseId_(std::vector<int>& v, int id);
};

#endif // ACTIONSET_H
