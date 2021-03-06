#ifndef TREEMODELITEM_H
#define TREEMODELITEM_H

#include <QtGlobal>

#include <memory>
#include <vector>

template <typename T>
class TreeModelItem
{
public:
    using Derived = T;
    using Base = TreeModelItem<T>;

    using ItemPointer = Derived*;
    using ItemHolder = std::unique_ptr<Derived>;

    TreeModelItem() noexcept = default;

    TreeModelItem(const TreeModelItem &) = delete;
    TreeModelItem(TreeModelItem &&) noexcept = default;
    ~TreeModelItem() = default;

    TreeModelItem &operator=(const TreeModelItem &) = delete;
    TreeModelItem &operator=(TreeModelItem &&) noexcept = default;

    ItemPointer createChild(qsizetype row = -1)
    {
        insert(row == - 1 ? childCount() : row, std::make_unique<Derived>());
        return ItemPointer(m_children.back().get());
    }

    ItemPointer parent() const noexcept { return m_parent; }

    qsizetype row() const noexcept
    {
        if (!m_parent)
            return 0;
        const auto pred = [this](const ItemHolder &p) { return this == p.get(); };
        const auto it = std::find_if(m_parent->m_children.begin(), m_parent->m_children.end(), pred);
        return {it - m_parent->m_children.begin()};
    }

    std::vector<ItemHolder> &children() noexcept { return m_children; }
    const std::vector<ItemHolder> &children() const noexcept { return m_children; }

    ItemPointer child(qsizetype row) const noexcept
    {
        return ItemPointer(m_children[size_t(row)].get());
    }

    qsizetype childCount() const noexcept { return qsizetype(m_children.size()); }

    void append(ItemHolder item) { insert(childCount(), std::move(item)); }

    void insert(qsizetype row, ItemHolder item)
    {
        Q_ASSERT(row >= 0 && row <= childCount());
        item->m_parent = ItemPointer(static_cast<Derived *>(this));
        m_children.insert(m_children.begin() + row, std::move(item));
    }

    void remove(ItemPointer item)
    {
        const auto pred = [item](const ItemHolder &p) { return item == p.get(); };
        const auto it = std::find_if(m_children.begin(), m_children.end(), pred);
        if (it != m_children.end())
            m_children.erase(it);
    }

    void remove(qsizetype row)
    {
        Q_ASSERT(row >= 0 && row <= childCount());
        m_children.erase(m_children.begin() + row);
    }

private:
    ItemPointer m_parent;
    std::vector<ItemHolder> m_children;
};

#endif // TREEMODELITEM_H
