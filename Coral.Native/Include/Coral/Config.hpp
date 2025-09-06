#pragma once

#ifdef CORAL_CUSTOM_CONFIG_FILE
    #include CORAL_CUSTOM_CONFIG_FILE
#else
    #include <string>
    #include <vector>
    #include <array>
    #include <unordered_map>
    #include <optional>
    #include <functional>
    #include <utility>
    #include <shared_mutex>
    #include <mutex>
    #include <memory>
    #include <atomic>

namespace Coral {
    using StdString = std::string;
    using StdWString = std::wstring;
    using StdStringView = std::string_view;
    using StdWStringView = std::wstring_view;
    template <typename T, size_t S>
    using StdArray = std::array<T, S>;
    template <typename T>
    using StdVector = std::vector<T>;
    template <typename K, typename V>
    using StdUnorderedMap = std::unordered_map<K, V>;
    template <typename T>
    using StdOptional = std::optional<T>;
    constexpr inline auto StdNullOpt = std::nullopt;
    template <typename F>
    using StdFunction = std::function<F>;

    template <typename T, typename U>
    using StdPair = std::pair<T, U>;

    template <typename T>
    using StdInitializerList = std::initializer_list<T>;

    using StdSharedMutex = std::shared_mutex;
    using StdMutex = std::mutex;

    template <typename T>
    using StdUniquePtr = std::unique_ptr<T>;
    template <typename T>
    using StdSharedPtr = std::shared_ptr<T>;
    template <typename T>
    using StdList = std::list<T>;

    template <typename T>
    using StdAtomic = std::atomic<T>;
}
#endif
