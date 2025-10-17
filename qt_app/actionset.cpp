#include "actionset.h"
#include <algorithm>
#include <utility>

/*
 * Internal helpers for vector-of-IDs maintenance:
 *  - containsId : membership test
 *  - eraseId    : remove all occurrences
 *  - remapEdgeList:
 *      * drop references to a removed node (victimId)
 *      * shift IDs that were numerically above victimId down by 1
 *        (because we renumber nodes after deletion)
 *      * also drop self-references (selfNewId) if any
 *      * deduplicate and sort for a clean, stable edge list
 */
namespace {
inline bool containsId(const std::vector<int>& v, int id){
    return std::find(v.begin(), v.end(), id) != v.end();
}
inline void eraseId(std::vector<int>& v, int id){
    v.erase(std::remove(v.begin(), v.end(), id), v.end());
}

void remapEdgeList(std::vector<int>& v, int victimId, int selfNewId){
    // Drop all edges pointing to the removed node
    v.erase(std::remove(v.begin(), v.end(), victimId), v.end());

    // Shift IDs that were above the removed ID
    for (int& x : v) if (x > victimId) x -= 1;

    // Ensure we don't create/keep a self-loop after renumbering
    v.erase(std::remove(v.begin(), v.end(), selfNewId), v.end());

    // Keep edges sorted and unique for deterministic behavior
    std::sort(v.begin(), v.end());
    v.erase(std::unique(v.begin(), v.end()), v.end());
}
} // namespace

/*
 * Construct a new ActionSet with a mandatory START node (ID = 0).
 * - The START node is the root; orphan nodes are optionally re-linked to it.
 */
ActionSet::ActionSet(){
    auto s = std::make_unique<StartAction>();
    s->id = 0;
    nodes_.push_back(std::move(s));
    indexRebuild_();
    lastId_ = 0;
}

/*
 * Rebuild the ID -> index map from scratch.
 * Must be called after any operation that can reorder 'nodes_' or change IDs.
 */
void ActionSet::indexRebuild_(){
    id2idx_.clear();
    for (size_t i=0;i<nodes_.size();++i) id2idx_[nodes_[i]->id] = i;
}

// Static wrappers that delegate to the anonymous-namespace helpers
bool ActionSet::containsId_(const std::vector<int>& v, int id){ return ::containsId(v,id); }
void ActionSet::eraseId_(std::vector<int>& v, int id){ ::eraseId(v,id); }

/*
 * Non-const lookup: return pointer to node with given ID, or nullptr.
 */
Action* ActionSet::find(int id){
    auto it = id2idx_.find(id);
    return (it==id2idx_.end()) ? nullptr : nodes_[it->second].get();
}

/*
 * Const lookup: return pointer to node with given ID, or nullptr.
 */
const Action* ActionSet::find(int id) const{
    auto it = id2idx_.find(id);
    return (it==id2idx_.end()) ? nullptr : nodes_.at(it->second).get();
}

/*
 * Link 'parentId' -> 'childId' (append child to parent's 'runAfterMe' list).
 * - No self-links
 * - No duplicate edges
 */
bool ActionSet::link(int parentId, int childId){
    if (parentId == childId) return false;
    Action* p = find(parentId);
    Action* c = find(childId);
    if (!p || !c) return false;
    if (!containsId_(p->runAfterMe, c->id)) p->runAfterMe.push_back(c->id);
    return true;
}

/*
 * Unlink 'parentId' -> 'childId' if present.
 */
bool ActionSet::unlink(int parentId, int childId){
    Action* p = find(parentId);
    if (!p) return false;
    eraseId_(p->runAfterMe, childId);
    return true;
}

/*
 * Collect all parents that list 'id' in their 'runAfterMe'.
 */
std::vector<int> ActionSet::parentsOf(int id) const{
    std::vector<int> out; out.reserve(nodes_.size());
    for (auto& up : nodes_) if (containsId_(up->runAfterMe, id)) out.push_back(up->id);
    return out;
}

/*
 * Return direct children (the 'runAfterMe' list) of 'id'.
 * If not found, return an empty list.
 */
std::vector<int> ActionSet::childrenOf(int id) const{
    const Action* a = find(id);
    return a ? a->runAfterMe : std::vector<int>{};
}

/*
 * Add a new node:
 *  - Assign a fresh ID = ++lastId_
 *  - Insert into storage and index
 *  - Link from provided parents; if none, link from START (ID 0) by default
 * Returns the assigned ID on success, -1 on error.
 */
int ActionSet::add(std::unique_ptr<Action> node, const std::vector<int>& parentIds){
    if (!node) return -1;

    node->id = ++lastId_;
    nodes_.push_back(std::move(node));
    id2idx_[lastId_] = nodes_.size()-1;

    if (parentIds.empty()) link(0, lastId_);
    else for (int pid : parentIds) link(pid, lastId_);

    return lastId_;
}

/*
 * Replace the node with ID 'id' by 'replacement' (preserving its children).
 *  - Not allowed for START (ID 0).
 *  - Remove incoming edges to 'id', then re-link according to 'newParentIds'
 *    (default: link from START if empty).
 *  - Keep the original children list.
 */
bool ActionSet::edit(int id, std::unique_ptr<Action> replacement, const std::vector<int>& newParentIds){
    if (!replacement || id == 0) return false;

    auto it = id2idx_.find(id);
    if (it == id2idx_.end()) return false;
    size_t idx = it->second;

    // Preserve outgoing edges (children)
    std::vector<int> children = nodes_[idx]->runAfterMe;

    // Drop all incoming edges to this node
    for (auto& up : nodes_) eraseId_(up->runAfterMe, id);

    // Install replacement with same ID and children
    replacement->id = id;
    replacement->runAfterMe = std::move(children);
    nodes_[idx].reset(replacement.release());

    // Recreate incoming links
    if (newParentIds.empty()) link(0, id);
    else for (int pid : newParentIds) link(pid, id);

    return true;
}

/*
 * Remove node 'id' (cannot remove START).
 * Steps:
 *  1) Gather children and parents of the victim
 *  2) If victim had no parents and 'relinkToStartIfOrphan' is true,
 *     link all victim's children to START; else connect each former
 *     parent directly to each child (except self-links)
 *  3) Erase the node
 *  4) Renumber all node IDs above 'id' (compact IDs)
 *  5) Remap every edge list to reflect the new IDs and drop stale references
 *  6) Update 'lastId_' and rebuild the index
 */
bool ActionSet::remove(int id, bool relinkToStartIfOrphan){
    if (id == 0) return false;

    auto it = id2idx_.find(id);
    if (it == id2idx_.end()) return false;

    size_t idx = it->second;
    Action* victim = nodes_[idx].get();

    // Snapshot edges before we mutate anything
    std::vector<int> children = victim->runAfterMe;
    std::vector<int> parents  = parentsOf(id);

    // Rewire graph around the victim
    if (parents.empty() && relinkToStartIfOrphan) {
        // Orphans: connect their children to START
        for (int cid : children) link(0, cid);
    } else {
        // Connect each parent to each child (no self-links)
        for (int pid : parents) {
            unlink(pid, id);
            for (int cid : children) if (pid != cid) link(pid, cid);
        }
    }

    // Physically remove the node
    nodes_.erase(nodes_.begin() + idx);

    // Compact IDs: shift down all IDs above the removed one
    for (auto& up : nodes_) if (up->id > id) up->id -= 1;

    // Remap all edge lists to updated IDs, drop references to the victim
    for (auto& up : nodes_) remapEdgeList(up->runAfterMe, id, up->id);

    // Keep lastId_ consistent with the highest existing ID
    if (lastId_ > 0) lastId_ -= 1;

    // Rebuild the fast lookup map
    indexRebuild_();
    return true;
}
