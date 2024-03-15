#pragma once

#include <xhash>

namespace Hazel {

    class UUID
    {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID&) = default;

        operator uint64_t() const { return m_UUID; }
    private:
        uint64_t m_UUID;
    };
}

namespace std {

    //ģ���ػ���ʹUUID������std::unordered_map
    template<>
    struct hash<Hazel::UUID>
    {
        std::size_t operator()(const Hazel::UUID& uuid) const
        {
            return std::hash<uint64_t>()((uint64_t)uuid);
        }
    };
}
